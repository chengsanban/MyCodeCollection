//
//  highestAVConverter.m
//  Created by chengsanban on 2018/8/2.

#import "highestAVConverter.h"

@implementation HighestAVConverter

- (id)initWithSrcPath:(NSString *)srcVideoPath
              dstPath:(NSString *)dstVideoPath
              dstType:(AVFileType)dstFileType
{
    if (self = [super init]) {
        self.srcVideoURL = [NSURL fileURLWithPath:srcVideoPath];
        self.dstVideoURL = [NSURL fileURLWithPath:dstVideoPath];
        self.dstFileType = dstFileType;
        self.asset = [AVAsset assetWithURL:self.srcVideoURL];
        self.error = nil;
        self.frameTimeScale = 1000;
        self.duration = kCMTimeZero;
        self.movieStartTime = kCMTimeZero;
        self.isExportSuccess = NO;
        self.audioEncodeSettings = nil;
        self.videoEncodeSettings = nil;
        self.curSampleBuffer = nil;
        self.lastSampleBuffer = nil;
    }
    return self;
}

- (void)dealloc
{
    if(self.curSampleBuffer)
    {
        CFRelease(self.curSampleBuffer);
        self.curSampleBuffer = NULL;
    }
    
    if(self.lastSampleBuffer)
    {
        CFRelease(self.lastSampleBuffer);
        self.lastSampleBuffer = NULL;
    }
    
    [super dealloc];
}

- (void)initEncodeVideoSettings:(NSDictionary*)videoSettings
{
    self.videoEncodeSettings = videoSettings;
}

- (void)initEncodeAudioSettings:(NSDictionary*)audioSettings
{
    self.audioEncodeSettings = audioSettings;
}

- (void)startConvertWithDuration:(int)duration 
                  movieStartTime:(int)movieStartTime 
                        Progress:(ProgressCallBack)progress
{
    if(self.srcVideoURL == nil || self.dstVideoURL == nil || self.dstFileType == nil || self.asset == nil)
        return;
    
    if(self.asset.playable == NO || self.asset.exportable == NO)
        return;
    
    //adjust movie start time
    if(movieStartTime > CMTimeGetSeconds(self.asset.duration) || movieStartTime <= 0)
    {
        self.movieStartTime = kCMTimeZero;
    }
    else
    {
        self.movieStartTime = CMTimeMake(movieStartTime, 1);
    }
    
    //adjust duration
    if (duration > CMTimeGetSeconds(self.asset.duration) || duration <= 0)
    {
        self.duration = self.asset.duration;
        self.movieStartTime = kCMTimeZero;
    }
    else
    {
        if( CMTimeGetSeconds(self.movieStartTime)+duration >= CMTimeGetSeconds(self.asset.duration))
        {
            self.duration = self.asset.duration;
            self.movieStartTime = kCMTimeZero;
        }
        else
        {
            self.duration = CMTimeMake(duration, 1);
        }
    }
    
    self.callback = progress;
    //  主队列
    NSString *serializationQueueDescription = [NSString stringWithFormat:@"%@ serialization queue", self];
    self.mainSerializationQueue = dispatch_queue_create([serializationQueueDescription UTF8String], NULL);
    
    // 音频队列
    NSString *rwAudioSerializationQueueDescription = [NSString stringWithFormat:@"%@ rw audio serialization queue", self];
    self.rwAudioSerializationQueue = dispatch_queue_create([rwAudioSerializationQueueDescription UTF8String], NULL);
    
    // 视频队列
    NSString *rwVideoSerializationQueueDescription = [NSString stringWithFormat:@"%@ rw video serialization queue", self];
    self.rwVideoSerializationQueue = dispatch_queue_create([rwVideoSerializationQueueDescription UTF8String], NULL);
    
    // 加载资源
    self.cancelled = NO;
    NSString *tracksKey = @"tracks";
    // 异步加载需要的资源
    [self.asset loadValuesAsynchronouslyForKeys:[NSArray arrayWithObject:tracksKey] completionHandler:^{
        dispatch_async(self.mainSerializationQueue, ^{
            // 由于是异步加载，先判断用户是否已经取消了
            if (self.cancelled)
            {
                return;
            }
            BOOL success = YES;
            NSError *localError = nil;
            // 检查视频轨道加载成功了吗
            success = ([self.asset statusOfValueForKey:tracksKey error:&localError] == AVKeyValueStatusLoaded);
            if (success)
            {
                // 成功判断是否有同名文件，有就删除
                NSFileManager *fm = [NSFileManager defaultManager];
                NSString *localOutputPath = [self.dstVideoURL path];
                if ([fm fileExistsAtPath:localOutputPath])
                {
                    success = [fm removeItemAtPath:localOutputPath error:&localError];
                }
            }
            if (success)
            {
                success = [self setupAssetReaderAndAssetWriter:&localError];
            }
            if (success)
            {
                success = [self startAssetReaderAndWriter:&localError];
            }
            if (!success)
            {
                [self readingAndWritingDidFinishSuccessfully:success withError:localError];
            }
        });
    }];
}

- (BOOL)setupAssetReaderAndAssetWriter:(NSError**) outError
{
    AVMutableComposition* composition = [AVMutableComposition composition];
    // 创建并且初始化assetReader
    self.assetReader = [[AVAssetReader alloc] initWithAsset:composition error:outError];
    BOOL success = (self.assetReader != nil);
    if (success)
    {
        // assetReader成功，继续创建并初始化assetWriter
        self.assetWriter = [[AVAssetWriter alloc] initWithURL:self.dstVideoURL fileType:self.dstFileType error:outError];
        success = (self.assetWriter != nil);
    }
    
    if (success)
    {
        // 获取想要的轨道
        AVAssetTrack *assetAudioTrack = nil, *assetVideoTrack = nil;
        NSArray *audioTracks = [self.asset tracksWithMediaType:AVMediaTypeAudio];
        if ([audioTracks count] > 0)
        {
            assetAudioTrack = audioTracks.firstObject;
        }
        NSArray *videoTracks = [self.asset tracksWithMediaType:AVMediaTypeVideo];
        if ([videoTracks count] > 0)
        {
            assetVideoTrack = videoTracks.firstObject;
        }
        
        //处理音频
        if (assetAudioTrack)
        {
            AVMutableCompositionTrack* audioCompositionTrack = [composition addMutableTrackWithMediaType:AVMediaTypeAudio preferredTrackID:kCMPersistentTrackID_Invalid];
            [audioCompositionTrack insertTimeRange:CMTimeRangeMake(self.movieStartTime,self.duration/* self.asset.duration*/) ofTrack:assetAudioTrack atTime:kCMTimeZero error:nil];
            // 音频解码设置
            NSDictionary *decodeAudioSettings = [NSDictionary dictionaryWithObjectsAndKeys:
                                                 [NSNumber numberWithUnsignedInt:kAudioFormatLinearPCM], AVFormatIDKey,
                                                 nil];
            self.audioReaderOutput = [AVAssetReaderAudioMixOutput assetReaderAudioMixOutputWithAudioTracks:@[audioCompositionTrack] audioSettings:decodeAudioSettings];
            if ([self.assetReader canAddOutput:self.audioReaderOutput])
            {
                [self.assetReader addOutput:self.audioReaderOutput];
            }
            
            // 音频编码设置
            self.audioWriterInput = [AVAssetWriterInput assetWriterInputWithMediaType:AVMediaTypeAudio outputSettings:self.audioEncodeSettings];
            
            if ([self.assetWriter canAddInput:self.audioWriterInput])
            {
                [self.assetWriter addInput:self.audioWriterInput];
            }
        }
        
        //处理视频
        if (assetVideoTrack)
        {
            AVMutableCompositionTrack* videoCompositionTrack = [composition addMutableTrackWithMediaType:AVMediaTypeVideo preferredTrackID:kCMPersistentTrackID_Invalid];
            [videoCompositionTrack insertTimeRange:CMTimeRangeMake(self.movieStartTime,self.duration/* self.asset.duration*/) ofTrack:assetVideoTrack atTime:kCMTimeZero error:nil];
            // 视频解码设置
            NSDictionary *decodeVideoSettings = [NSDictionary dictionaryWithObjectsAndKeys:
                                                 [NSNumber numberWithUnsignedInt:kCVPixelFormatType_422YpCbCr8], (id)kCVPixelBufferPixelFormatTypeKey,
                                                 [NSDictionary dictionary],(id)kCVPixelBufferIOSurfacePropertiesKey,
                                                 nil];
            //获取图层
            AVMutableVideoCompositionLayerInstruction* layer = [AVMutableVideoCompositionLayerInstruction videoCompositionLayerInstructionWithAssetTrack:videoCompositionTrack];
            CMVideoFormatDescriptionRef videoFormat = (__bridge CMVideoFormatDescriptionRef)(assetVideoTrack.formatDescriptions.firstObject);
            CGSize videoSize;//videoCompositionTrack.naturalSize;
            if(videoFormat)
            {
                //获取视频显示大小
                videoSize = CMVideoFormatDescriptionGetPresentationDimensions(videoFormat, true, true);
            }
            else
            {
                videoSize = assetVideoTrack.naturalSize;
            }

            CGRect assetRect;
            if([self.videoEncodeSettings objectForKey:AVVideoWidthKey] != nil)
            {
                long width = [[self.videoEncodeSettings objectForKey:AVVideoWidthKey] integerValue];
                long height = [[self.videoEncodeSettings objectForKey:AVVideoHeightKey] integerValue];
                assetRect = AVMakeRectWithAspectRatioInsideRect(videoSize, CGRectMake(0,0,width,height));
            }
            else
            {
                long width =  videoCompositionTrack.naturalSize.width;
                long height = videoCompositionTrack.naturalSize.height;
                assetRect = AVMakeRectWithAspectRatioInsideRect(videoSize, CGRectMake(0,0,width,height));
            }

            CGAffineTransform scale = CGAffineTransformMakeScale(NSWidth(assetRect)/videoSize.width, NSHeight(assetRect)/videoSize.height);
            CGAffineTransform translation = CGAffineTransformTranslate(scale, NSMinX(assetRect), NSMinY(assetRect));
            [layer setTransform:translation atTime:kCMTimeZero];

            AVMutableVideoCompositionInstruction* instruction = [AVMutableVideoCompositionInstruction videoCompositionInstruction];
            instruction.timeRange = CMTimeRangeMake(kCMTimeZero, self.duration/*composition.duration*/);
            instruction.layerInstructions = @[layer];

            [self adjustAndCalcFramerate];

            AVMutableVideoComposition* videoComposition = [AVMutableVideoComposition videoComposition];
            videoComposition.frameDuration = CMTimeMake(self.frameTimeScale, (int)(self.frameRate * self.frameTimeScale));
            videoComposition.instructions = @[instruction];
            long tempWidth = [[self.videoEncodeSettings objectForKey:AVVideoWidthKey] integerValue];
            long tempHeight = [[self.videoEncodeSettings objectForKey:AVVideoHeightKey] integerValue];
            videoComposition.renderSize = CGSizeMake(tempWidth, tempHeight);
            
            
            self.videoReaderOutput  = [AVAssetReaderVideoCompositionOutput assetReaderVideoCompositionOutputWithVideoTracks:@[videoCompositionTrack] videoSettings:decodeVideoSettings];
            
            self.videoReaderOutput.videoComposition = videoComposition;
            self.videoReaderOutput.alwaysCopiesSampleData = NO;
            
            if ([self.assetReader canAddOutput:self.videoReaderOutput])
            {
                [self.assetReader addOutput:self.videoReaderOutput];
            }
            
            // 视频编码设置
            self.videoWriterinput = [AVAssetWriterInput assetWriterInputWithMediaType:AVMediaTypeVideo outputSettings:self.videoEncodeSettings];
            if ([self.assetWriter canAddInput:self.videoWriterinput])
            {
                [self.assetWriter addInput:self.videoWriterinput];
            }
        }
    }
    return success;
}

- (BOOL)startAssetReaderAndWriter:(NSError**) outError
{
    BOOL success = YES;
    
    // 启动assetReader
    success = [self.assetReader startReading];
    if (!success)
    {
        *outError = [self.assetReader error];
    }
    
    // 启动assetWriter
    if (success)
    {
        success = [self.assetWriter startWriting];
        if (!success)
        {
            *outError = [self.assetWriter error];
            NSLog(@"LD:%@\n",(*outError).localizedDescription);
            NSLog(@"LF:%@\n",(*outError).localizedFailureReason);
            NSLog(@"LRO:%@\n",(*outError).localizedRecoveryOptions);
            NSLog(@"LRS:%@\n",(*outError).localizedRecoverySuggestion);
            NSLog(@"UI:%@\n",(*outError).userInfo);
        }
    }
    
    if (success)
    {
        self.dispatchGroup = dispatch_group_create();
        [self.assetWriter startSessionAtSourceTime:kCMTimeZero];
        self.audioFinished = NO;
        self.videoFinished = NO;
        
        if (self.audioWriterInput)
        {
            dispatch_group_enter(self.dispatchGroup);
            [self.audioWriterInput requestMediaDataWhenReadyOnQueue:self.rwAudioSerializationQueue usingBlock:^{
                if (self.audioFinished)
                {
                    return;
                }
                BOOL completedOrFailed = NO;
                while ([self.audioWriterInput isReadyForMoreMediaData] && !completedOrFailed)
                {
                    CMSampleBufferRef sampleBuffer = [self.audioReaderOutput copyNextSampleBuffer];
                    if (sampleBuffer != NULL)
                    {
                        if (self.videoWriterinput == nil)
                        {
                            CMTime presTime = CMSampleBufferGetPresentationTimeStamp(sampleBuffer);
                            if (CMTIME_COMPARE_INLINE(presTime, !=, kCMTimeInvalid))
                            {
                                self.callback(CMTimeGetSeconds(presTime)/CMTimeGetSeconds(self.duration));
                            }
                        }
                        
                        BOOL success = [self.audioWriterInput appendSampleBuffer:sampleBuffer];
                        CFRelease(sampleBuffer);
                        sampleBuffer = NULL;
                        completedOrFailed = !success;
                    }
                    else
                    {
                        completedOrFailed = YES;
                    }
                }
                if (completedOrFailed)
                {
                    BOOL oldFinished = self.audioFinished;
                    self.audioFinished = YES;
                    if (oldFinished == NO)
                    {
                        [self.audioWriterInput markAsFinished];
                    }
                    dispatch_group_leave(self.dispatchGroup);
                }
            }];
        }
        
        if (self.videoWriterinput)
        {
            dispatch_group_enter(self.dispatchGroup);
            [self.videoWriterinput requestMediaDataWhenReadyOnQueue:self.rwVideoSerializationQueue usingBlock:^{
                if (self.videoFinished)
                {
                    return;
                }
                BOOL completedOrFailed = NO;
                while ([self.videoWriterinput isReadyForMoreMediaData] && !completedOrFailed)
                {
                    if(self.lastSampleBuffer != NULL)
                    {
                        CMTime curTimeStamp;
                        CMTime lastTimeStamp;
                        lastTimeStamp = CMSampleBufferGetOutputPresentationTimeStamp(self.lastSampleBuffer);
                        if(self.curSampleBuffer == NULL)
                        {
                            self.curSampleBuffer = [self.videoReaderOutput copyNextSampleBuffer];
                        }
                        if(self.curSampleBuffer)
                        {
                            curTimeStamp = CMSampleBufferGetOutputPresentationTimeStamp(self.curSampleBuffer);
                        }
                        if(CMTIME_COMPARE_INLINE(curTimeStamp, !=, kCMTimeInvalid))
                        {
                            self.callback(CMTimeGetSeconds(curTimeStamp)/CMTimeGetSeconds(self.duration));
                        }
                        
                        if(curTimeStamp.timescale == lastTimeStamp.timescale)
                        {
                            if(curTimeStamp.value - lastTimeStamp.value > self.frameTimeScale)  //add frame
                            {
                                CMItemCount count;
                                CMSampleBufferGetSampleTimingInfoArray(self.lastSampleBuffer, 0, nil, &count);
                                CMSampleTimingInfo* pInfo = (CMSampleTimingInfo*) malloc(sizeof(CMSampleTimingInfo) * count);
                                CMSampleBufferGetSampleTimingInfoArray(self.lastSampleBuffer, count, pInfo, &count);
                                for(CMItemCount i=0; i<count; i++)
                                {
                                    pInfo[i].presentationTimeStamp = CMTimeMake(lastTimeStamp.value + self.frameTimeScale, lastTimeStamp.timescale);
                                }
                                
                                CMSampleBufferRef tempSampleBuffer;
                                CMSampleBufferCreateCopyWithNewTiming(nil, self.lastSampleBuffer, count, pInfo, &tempSampleBuffer);
                                free(pInfo);
                                
                                if (tempSampleBuffer != NULL)
                                {
                                    BOOL success = [self.videoWriterinput appendSampleBuffer:tempSampleBuffer];
                                    CFRelease(self.lastSampleBuffer);
                                    self.lastSampleBuffer = tempSampleBuffer;
                                    tempSampleBuffer = NULL;
                                    completedOrFailed = !success;
                                }
                                else
                                {
                                    completedOrFailed = YES;
                                }
                            }
                            else  //write cur frame
                            {
                                if (self.curSampleBuffer != NULL)
                                {
                                    BOOL success = [self.videoWriterinput appendSampleBuffer:self.curSampleBuffer];
                                    CFRelease(self.lastSampleBuffer);
                                    self.lastSampleBuffer = self.curSampleBuffer;
                                    self.curSampleBuffer = NULL;
                                    completedOrFailed = !success;
                                }
                                else
                                {
                                    completedOrFailed = YES;
                                }
                            }
                        }
                        else
                        {
                            completedOrFailed = YES;
                        }
                        
                    }
                    else //first frame
                    {
                        self.lastSampleBuffer = [self.videoReaderOutput copyNextSampleBuffer];
                        if(self.lastSampleBuffer != NULL)
                        {
                            BOOL success = [self.videoWriterinput appendSampleBuffer:self.lastSampleBuffer];
                            completedOrFailed = !success;
                            CMItemCount count;
                            CMTime lastTimeStamp = CMSampleBufferGetOutputPresentationTimeStamp(self.lastSampleBuffer);
                            CMSampleBufferGetSampleTimingInfoArray(self.lastSampleBuffer, 0, nil, &count);
                            CMSampleTimingInfo* pInfo = (CMSampleTimingInfo*) malloc(sizeof(CMSampleTimingInfo) * count);
                            CMSampleBufferGetSampleTimingInfoArray(self.lastSampleBuffer, count, pInfo, &count);
                            for(CMItemCount i=0; i<count; i++)
                            {
                                pInfo[i].presentationTimeStamp = CMTimeMake(lastTimeStamp.value + self.frameTimeScale, (int)(self.frameTimeScale * self.frameRate));
                            }
                            
                            CMSampleBufferRef tempSampleBuffer;
                            CMSampleBufferCreateCopyWithNewTiming(nil, self.lastSampleBuffer, count, pInfo, &tempSampleBuffer);
                            free(pInfo);
                            CFRelease(self.lastSampleBuffer);
                            self.lastSampleBuffer = tempSampleBuffer;
                        }
                        else
                        {
                            completedOrFailed = YES;
                        }
                    }
                }
                if (completedOrFailed)
                {
                    BOOL oldFinished = self.videoFinished;
                    self.videoFinished = YES;
                    if (oldFinished == NO)
                    {
                        [self.videoWriterinput markAsFinished];
                    }
                    dispatch_group_leave(self.dispatchGroup);
                }
            }];
        }
        dispatch_group_notify(self.dispatchGroup, self.mainSerializationQueue, ^{
            
            BOOL finalSuccess = YES;
            NSError *finalError = nil;

            if (self.cancelled)
            {
                [self.assetReader cancelReading];
                [self.assetWriter cancelWriting];
            }
            else
            {
                if ([self.assetReader status] == AVAssetReaderStatusFailed)
                {
                    finalSuccess = NO;
                    finalError = [self.assetReader error];
                }
                if (finalSuccess)
                {
                    finalSuccess = [self.assetWriter finishWriting];
                    if (!finalSuccess)
                    {
                        finalError = [self.assetWriter error];
                    }
                }
            }
            [self readingAndWritingDidFinishSuccessfully:finalSuccess withError:finalError];
        });
    }
    return success;
}

- (void)readingAndWritingDidFinishSuccessfully:(BOOL)success withError:(NSError*)localError
{
    if (!success)
    {
        [self.assetReader cancelReading];
        [self.assetWriter cancelWriting];
        self.audioFinished = YES;
        self.videoFinished = YES;
        self.error = localError;
    }
}

- (void)cancel
{
    dispatch_async(self.mainSerializationQueue, ^{
        if (self.audioWriterInput)
        {
            dispatch_async(self.rwAudioSerializationQueue, ^{
                BOOL oldFinished = self.audioFinished;
                self.audioFinished = YES;
                if (oldFinished == NO)
                {
                    [self.audioWriterInput markAsFinished];
                }
                dispatch_group_leave(self.dispatchGroup);
            });
        }
        
        if (self.videoWriterinput)
        {
            dispatch_async(self.rwVideoSerializationQueue, ^{
                BOOL oldFinished = self.videoFinished;
                self.videoFinished = YES;
                if (oldFinished == NO)
                {
                    [self.videoWriterinput markAsFinished];
                }
                dispatch_group_leave(self.dispatchGroup);
            });
        }
        self.cancelled = YES;
    });
}

- (void)adjustAndCalcFramerate
{
    if(self.frameRate < 1 || self.frameRate > 120)
        self.frameRate = 60;
}
@end

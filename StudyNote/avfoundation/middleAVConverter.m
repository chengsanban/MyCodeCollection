//
//  middleAVConverter.m
//  Created by chengsanban on 2018/8/2.

#import "middleAVConverter.h"

@implementation MiddleAVConverter

- (id)initWithSrcPath:(NSString *)srcPath
              dstPath:(NSString *)dstPath
              dstType:(AVFileType)dstFileType
{
    if (self = [super init])
    {
        if(srcPath && dstPath && dstFileType)
        {
            self.srcVideoURL = [NSURL fileURLWithPath:srcPath];
            self.dstVideoURL = [NSURL fileURLWithPath:dstPath];
            self.dstFileType = dstFileType;
            self.asset = [AVAsset assetWithURL:self.srcVideoURL];
        }
    }
    return self;
}

- (void) initEncodeVideoSettings:(NSDictionary*)videoSettings
{
    self.videoEncodeSettings = videoSettings;
}

- (void) initEncodeAudioSettings:(NSDictionary*)audioSettings
{
    self.audioEncodeSettings = audioSettings;
}

- (void) startConvert
{
    if(self.srcVideoURL == nil || self.dstVideoURL == nil || self.dstFileType == nil || self.asset == nil)
        return;
    
    if(self.asset.playable == NO || self.asset.exportable == NO)
        return;
    
    // 创建主队列
    NSString *serializationQueueDescription = [NSString stringWithFormat:@"%@ serialization queue", self];
    self.mainSerializationQueue = dispatch_queue_create([serializationQueueDescription UTF8String], NULL);
    
    // 创建用于读写音频队列
    NSString *rwAudioSerializationQueueDescription = [NSString stringWithFormat:@"%@ rw audio serialization queue", self];
    self.rwAudioSerializationQueue = dispatch_queue_create([rwAudioSerializationQueueDescription UTF8String], NULL);
    
    // 创建用于读写视频队列
    NSString *rwVideoSerializationQueueDescription = [NSString stringWithFormat:@"%@ rw video serialization queue", self];
    self.rwVideoSerializationQueue = dispatch_queue_create([rwVideoSerializationQueueDescription UTF8String], NULL);
    
    // 加载资源
    self.cancelled = NO;
    NSString* tracksKey = @"tracks";
    // 异步加载所需要资源
    [self.asset loadValuesAsynchronouslyForKeys:[NSArray arrayWithObject:tracksKey] completionHandler:^{
        // 加载完毕调用主队列
        dispatch_async(self.mainSerializationQueue, ^{
            // 检查是否取消导出
            if (self.cancelled)
            {
                return;
            }
            BOOL success = YES;
            NSError *localError = nil;
            // 检查视频轨道是否加载
            success = ([self.asset statusOfValueForKey:tracksKey error:&localError] == AVKeyValueStatusLoaded);
            if (success)
            {
                // 确保写入文件不存在，存在就删除
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

- (BOOL) setupAssetReaderAndAssetWriter:(NSError**) outError
{
    // 创建和初始化 assetReader.
    self.assetReader = [[AVAssetReader alloc] initWithAsset:self.asset error:outError];
    BOOL success = (self.assetReader != nil);
    if (success)
    {
        // assetReader初始化成功，创建和初始化assetWriter
        self.assetWriter = [[AVAssetWriter alloc] initWithURL:self.dstVideoURL fileType:self.dstFileType error:outError];
        success = (self.assetWriter != nil);
    }
    
    if (success)
    {
        // assetReader和assetWriter成功，创建想要的视频轨道
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
        
        //音频处理
        if (assetAudioTrack)
        {
            // 音频解码设置
            NSDictionary *decodeAudioSettings = [NSDictionary dictionaryWithObjectsAndKeys:
                                                 [NSNumber numberWithUnsignedInt:kAudioFormatLinearPCM],AVFormatIDKey,
                                                 nil];
            self.audioReaderOutput = [AVAssetReaderTrackOutput assetReaderTrackOutputWithTrack:assetAudioTrack outputSettings:decodeAudioSettings];
            if([self.assetReader canAddOutput:self.audioReaderOutput])
            {
                [self.assetReader addOutput:self.audioReaderOutput];
            }
            
            // 音频编码设置
            self.audioWriterInput = [AVAssetWriterInput assetWriterInputWithMediaType:AVMediaTypeAudio outputSettings:self.audioEncodeSettings];
            if([self.assetWriter canAddInput:self.audioWriterInput])
            {
                [self.assetWriter addInput:self.audioWriterInput];
            }
        }
        
        //视频处理
        if (assetVideoTrack)
        {
            // 视频解码设置
            NSDictionary *decodeVideoSettings = [NSDictionary dictionaryWithObjectsAndKeys:
                                                 [NSNumber numberWithUnsignedInt:kCVPixelFormatType_422YpCbCr8],(id)kCVPixelBufferPixelFormatTypeKey,
                                                 [NSDictionary dictionary],(id)kCVPixelBufferIOSurfacePropertiesKey,
                                                 nil];
            self.videoReaderOutput = [AVAssetReaderTrackOutput assetReaderTrackOutputWithTrack:assetVideoTrack outputSettings:decodeVideoSettings];
            if([self.assetReader canAddOutput:self.videoReaderOutput])
            {
                [self.assetReader addOutput:self.videoReaderOutput];
            }
            
            // 视频编码设置
            self.videoWriterinput = [AVAssetWriterInput assetWriterInputWithMediaType:AVMediaTypeVideo outputSettings:self.videoEncodeSettings];
            if([self.assetWriter canAddInput:self.videoWriterinput])
            {
                [self.assetWriter addInput:self.videoWriterinput];
            }
        }
    }
    return success;
}

- (BOOL) startAssetReaderAndWriter:(NSError**) outError
{
    BOOL success = YES;
    
    // 启动assetReader
    success = [self.assetReader startReading];
    if (!success)
    {
        *outError = [self.assetReader error];
    }
    
    // 启动assetReader成功后，启动assetWriter
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
        // 准备好之后，进行编码和解码
        self.dispatchGroup = dispatch_group_create();
        [self.assetWriter startSessionAtSourceTime:kCMTimeZero];
        self.audioFinished = NO;
        self.videoFinished = NO;
        
        if (self.audioWriterInput)
        {
            // 编码音频前加入组
            dispatch_group_enter(self.dispatchGroup);
            // assetWriter准备好，并且被队列调用，下面代码会开始执行
            [self.audioWriterInput requestMediaDataWhenReadyOnQueue:self.rwAudioSerializationQueue usingBlock:^{
                // 异步执行，先检查是否完成
                if (self.audioFinished)
                {
                    return;
                }
                BOOL completedOrFailed = NO;
                // 没完成的话，确保数据准备好
                while ([self.audioWriterInput isReadyForMoreMediaData] && !completedOrFailed)
                {
                    //获取下一帧数据，并操作或直接送给assetWrite
                    CMSampleBufferRef sampleBuffer = [self.audioReaderOutput copyNextSampleBuffer];
                    if (sampleBuffer != NULL)
                    {
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
                    // 标记输入完成
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
        
        //同音频
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
                    CMSampleBufferRef sampleBuffer = [self.videoReaderOutput copyNextSampleBuffer];
                    
                    if (sampleBuffer != NULL)
                    {
                        BOOL success = [self.videoWriterinput appendSampleBuffer:sampleBuffer];
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
            // 是否取消
            if (self.cancelled)
            {
                [self.assetReader cancelReading];
                [self.assetWriter cancelWriting];
            }
            else
            {
                // 没取消，确保不是由于失败导致
                if ([self.assetReader status] == AVAssetReaderStatusFailed)
                {
                    finalSuccess = NO;
                    finalError = [self.assetReader error];
                }
                // 没有失败，结束并检查可能会存在的错误
                if (finalSuccess)
                {
                    finalSuccess = [self.assetWriter finishWriting];
                    if (!finalSuccess)
                    {
                        finalError = [self.assetWriter error];
                    }
                }
            }
            //完成之后，检查错误并处理，或者正常完成
            [self readingAndWritingDidFinishSuccessfully:finalSuccess withError:finalError];
        });
    }
    // 返回是否成功开启assetReader和assetWriter
    return success;
}

- (void) readingAndWritingDidFinishSuccessfully:(BOOL)success withError:(NSError*)localError
{
    if (!success)
    {
        //失败处理
        [self.assetReader cancelReading];
        [self.assetWriter cancelWriting];
        dispatch_async(dispatch_get_main_queue(), ^{
            // UI或者其他需要失败后做的
        });
    }
    else
    {
        //成功
        self.cancelled = NO;
        self.videoFinished = NO;
        self.audioFinished = NO;
        dispatch_async(dispatch_get_main_queue(), ^{
            // UI或者成功后要做的
        });
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


@end

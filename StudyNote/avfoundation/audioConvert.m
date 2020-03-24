  //asset生成必须为文件的url，而且是本地
    AVAsset *mediaAsset = _player.currentItem.asset;
    //AVMutableComposition 可以进行音视频的组合
    AVAssetExportSession *es = [[AVAssetExportSession alloc] initWithAsset:mediaAsset presetName:AVAssetExportPresetPassthrough];
    NSString *outPath = [NSTemporaryDirectory() stringByAppendingPathComponent:@"out.mov"];
    NSFileManager *fileManager = [NSFileManager defaultManager];
    [fileManager removeItemAtPath:outPath error:NULL];
    //格式的转换
    es.outputFileType = AVFileTypeQuickTimeMovie;
    es.outputURL = [[NSURL alloc] initFileURLWithPath:outPath];
    es.shouldOptimizeForNetworkUse=NO;
    CMTime start = CMTimeMakeWithSeconds(1.0, 600);
    CMTime duration = CMTimeMakeWithSeconds(3.0, 600);
    CMTimeRange range = CMTimeRangeMake(start, duration);
    //音频的时间范围
    es.timeRange = range;
    NSLog(@"exporting to %@",outPath);
    //异步输出完成后调用
    [es exportAsynchronouslyWithCompletionHandler:^{
        NSString *status = @"" ;
        if( es.status == AVAssetExportSessionStatusCompleted ) status = @"AVAssetExportSessionStatusCompleted";
        else if( es.status == AVAssetExportSessionStatusFailed ) status = @"AVAssetExportSessionStatusFailed";
        NSLog(@"done exporting to %@ status %d = %@ (%@)",outPath,es.status, status,[es error]);
    }];

    //第二种方式读取本地到另外一个地方
    NSString *documentPath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
    NSString *audioPath = [documentPath stringByAppendingPathComponent:[NSString stringWithFormat:@"audio"]];
    NSURL *fileUrl = [NSURL fileURLWithPath:[NSString stringWithFormat:@"%@/1141.mp3",audioPath]];
    AVURLAsset *asset = [[AVURLAsset alloc] initWithURL:fileUrl options:nil];
    NSError *error = nil;
    NSArray *array=[_player.currentItem.asset tracksWithMediaType:AVMediaTypeAudio];
    AVAssetTrack *track=[[_player.currentItem.asset tracksWithMediaType:AVMediaTypeAudio]firstObject];
    NSLog(@"%@",_player.currentItem.asset)
    assetReader=[[AVAssetReader alloc]initWithAsset:_player.currentItem.asset error:&error];
    NSDictionary *readerOutputSettings=@{(id)AVFormatIDKey:@(kAudioFormatLinearPCM)};
    //如果读取失败接下来的代码不会执行。
    AVAssetReaderTrackOutput *trackOutput=[[AVAssetReaderTrackOutput alloc]initWithTrack:track outputSettings:readerOutputSettings];
    [assetReader addOutput:trackOutput];
    //先判断read为YES
    BOOL read= [assetReader startReading];
    NSString *userPath = [audioPath stringByAppendingPathComponent:[NSString stringWithFormat:@"/writer1.wav"]];
    NSURL *outputURL=[NSURL fileURLWithPath:userPath];
    assetWriter=[[AVAssetWriter alloc]initWithURL:outputURL fileType:AVFileTypeWAVE error:nil];
    AudioChannelLayout channelLayout;
    memset(&channelLayout, 0, sizeof(AudioChannelLayout));
    channelLayout.mChannelLayoutTag = kAudioChannelLayoutTag_Stereo;
    //lin PCM需要设置PCM一些相关的属性
    NSDictionary *writerOutputSetting=@{(id)AVFormatIDKey:@(kAudioFormatLinearPCM),(id)AVSampleRateKey:[NSNumber numberWithFloat:44100.0],
                                        AVNumberOfChannelsKey:[NSNumber numberWithInt: 2],
                                        AVLinearPCMBitDepthKey:[NSNumber numberWithInt:16],
                                      AVLinearPCMIsNonInterleaved:[NSNumber numberWithBool:NO],
                                        AVLinearPCMIsFloatKey:[NSNumber numberWithBool:NO],
                                        AVLinearPCMIsBigEndianKey:[NSNumber numberWithBool:NO],AVChannelLayoutKey:[ NSData dataWithBytes:&channelLayout length: sizeof( AudioChannelLayout ) ]};
    AVAssetWriterInput *writerInput=[[AVAssetWriterInput alloc]initWithMediaType:AVMediaTypeAudio outputSettings:writerOutputSetting];
    [assetWriter addInput:writerInput];
    BOOL writer=[assetWriter startWriting];
    dispatch_queue_t dispatchQueue=dispatch_queue_create("com/tapharmonic.WriterQueue", NULL);
    [assetWriter startSessionAtSourceTime:kCMTimeZero];
    [writerInput requestMediaDataWhenReadyOnQueue:dispatchQueue usingBlock:^{
        bool complete=NO;
        while ([writerInput isReadyForMoreMediaData]&& !complete) {
            NSLog(@"%@",assetWriter.error);
            CMSampleBufferRef sampleBuffer=[trackOutput copyNextSampleBuffer];
            if(sampleBuffer){
                BOOL result=[writerInput appendSampleBuffer:sampleBuffer];
                CFRelease(sampleBuffer);
                complete=!result;
            }else{
                [writerInput markAsFinished];
                complete=YES;
            }
        }

        if(complete){
            [assetWriter finishWritingWithCompletionHandler:^{
                AVAssetWriterStatus status=assetWriter.status;
                NSLog(@"%@",assetWriter.error);
                if(status==AVAssetWriterStatusCompleted){
                    NSLog(@"AVAssetWriterStatusCompleted");
                }else{
                    NSLog(@"AVAssetWriterStatusFailed");
                }
            }];
        }
    }];
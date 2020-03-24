//
//  lowAVConverter.m
//  Created by chengsanban on 2018/8/2.

#import "lowAVConverter.h"

@implementation LowAVConverter

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
        }
    }
    return self;
}

- (void)startConvert
{
    if(self.srcVideoURL == nil || self.dstVideoURL == nil)
        return;
    
    AVURLAsset *asset = [AVURLAsset assetWithURL:self.srcVideoURL];
    
    AVMutableComposition *mainComposition = [AVMutableComposition composition];
    
    AVMutableCompositionTrack *videoCompositionTrack = nil;
    AVMutableCompositionTrack *audioCompositionTrack = nil;
    
    AVAssetTrack *videoTrack = [asset tracksWithMediaType:AVMediaTypeVideo].firstObject;
    AVAssetTrack *audioTrack = [asset tracksWithMediaType:AVMediaTypeAudio].firstObject;
    
    if (videoTrack) {
        if (!videoCompositionTrack) {
            videoCompositionTrack = [mainComposition addMutableTrackWithMediaType:AVMediaTypeVideo preferredTrackID:kCMPersistentTrackID_Invalid];
        }
        [videoCompositionTrack insertTimeRange:CMTimeRangeMake(kCMTimeZero, asset.duration) ofTrack:videoTrack atTime:kCMTimeZero error:nil];
    }
    if (audioTrack) {
        if (!audioCompositionTrack) {
            audioCompositionTrack = [mainComposition addMutableTrackWithMediaType:AVMediaTypeAudio preferredTrackID:kCMPersistentTrackID_Invalid];
        }
        [audioCompositionTrack insertTimeRange:CMTimeRangeMake(kCMTimeZero, asset.duration) ofTrack:audioTrack atTime:kCMTimeZero error:nil];
    }
    
    self.exporter = [[AVAssetExportSession alloc] initWithAsset:mainComposition presetName:AVAssetExportPreset1920x1080];
    self.exporter.outputURL = self.dstVideoURL;
    self.exporter.outputFileType = self.dstFileType;
    self.exporter.shouldOptimizeForNetworkUse = YES;
    
    [self.exporter exportAsynchronouslyWithCompletionHandler:^{
        //do smething after covert done
        [self convertDidFinish];
    }];
}

- (void) convertDidFinish
{
    switch (self.exporter.status) {
        case AVAssetExportSessionStatusFailed:
            NSLog(@"fail:->Reason:%@,UserInfo:%@",_exporter.error.localizedDescription,_exporter.error.userInfo.description);
            break;
        case AVAssetExportSessionStatusUnknown:
            NSLog(@"unknown");
            break;
        case AVAssetExportSessionStatusWaiting:
            NSLog(@"waiting");
            break;
        case AVAssetExportSessionStatusCancelled:
            NSLog(@"cancel");
            break;
        case AVAssetExportSessionStatusCompleted:
            NSLog(@"completed");
            break;
        case AVAssetExportSessionStatusExporting:
            NSLog(@"exporting");
            break;
        default:
            break;
    }
}

//使用定时器调用
- (float) progress
{
    return [self.exporter progress];
}

- (void) cancelConvert
{
    [self.exporter cancelExport];
}
@end

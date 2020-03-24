//
//  main.m
//
//  Created by User on 2018/8/2.
//  Copyright © 2018 User. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import "lowAVConverter.h"
#import "middleAVConverter.h"
#import "highestAVConverter.h"

int main(int argc, const char * argv[]) {
    @autoreleasepool {
 
        /*****************
         lowAVConverter测试
         *****************/
#if 1
        LowAVConverter* convert = [[LowAVConverter alloc] initWithSrcPath:@"/Users/user/Desktop/320x240.mov"
                                                                        dstPath:@"/Users/user/Desktop/xx.mp4"
                                                                        dstType:AVFileTypeMPEG4];
        [convert startConvert];
#endif
        
        /*****************
         middleAVConverter测试
         *****************/
#if 0
        NSDictionary *encodeAudioSettings = [NSDictionary dictionaryWithObjectsAndKeys:
                                             [NSNumber numberWithUnsignedInt:kAudioFormatMPEG4AAC], AVFormatIDKey,
                                             [NSNumber numberWithInteger:44100], AVSampleRateKey,
                                             [NSNumber numberWithUnsignedInt:2], AVNumberOfChannelsKey,
                                             nil];
        
        NSDictionary *encodeVideoSettings = [NSDictionary dictionaryWithObjectsAndKeys:
                                             AVVideoCodecTypeH264, AVVideoCodecKey,
                                             [NSNumber numberWithInt:320], AVVideoWidthKey,
                                             [NSNumber numberWithInt:240], AVVideoHeightKey,
                                             nil];
        MiddleAVConverter* convert = [[MiddleAVConverter alloc] initWithSrcPath:@"/Users/user/Desktop/320x240.mov"
                                                                        dstPath:@"/Users/user/Desktop/xx.mp4"
                                                                        dstType:AVFileTypeMPEG4];
        [convert initEncodeAudioSettings:encodeAudioSettings];
        [convert initEncodeVideoSettings:encodeVideoSettings];
        
        [convert startConvert];
#endif
        /*****************
         highestAVConverter测试
         *****************/
#if 0
        NSDictionary *encodeAudioSettings = [NSDictionary dictionaryWithObjectsAndKeys:
                                             [NSNumber numberWithUnsignedInt:kAudioFormatMPEG4AAC], AVFormatIDKey,
                                             [NSNumber numberWithInteger:48000], AVSampleRateKey,
                                             [NSNumber numberWithUnsignedInt:2], AVNumberOfChannelsKey,
                                             nil];
        
        NSDictionary *encodeVideoSettings = [NSDictionary dictionaryWithObjectsAndKeys:
                                             AVVideoCodecTypeH264, AVVideoCodecKey,
                                             [NSNumber numberWithInt:320], AVVideoWidthKey,
                                             [NSNumber numberWithInt:240], AVVideoHeightKey,
                                             nil];
        
        
        HighestAVConverter* convert = [[HighestAVConverter alloc] initWithSrcPath:@"/Users/user/Desktop/320x240.mov"
                                                           dstPath:@"/Users/user/Desktop/xx.mp4"
                                                           dstType:AVFileTypeMPEG4];
        
        [convert initEncodeAudioSettings:encodeAudioSettings];
        [convert initEncodeVideoSettings:encodeVideoSettings];
        
        [convert startConvertWithDuration:0 movieStartTime:0 Progress:^(float progress) {
                NSLog(@"--%f--\n",progress);
            }];
#endif
     
        //异步导出，主线程不能挂
        sleep(100000000);
    }
    return 0;
}

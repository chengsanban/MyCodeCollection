//
//  function.h
//  CoreAudio
//
//  Created by User on 2018/8/21.
//  Copyright © 2018 User. All rights reserved.
//

#ifndef function_h
#define function_h
#import <AudioToolbox/AudioToolbox.h>

@interface Function : NSObject
//******public**********
//get some info from audio file
+ (void)getAudioInfo;

//create a audio file
+ (void)createAudioFile;

//inspecting core audio format support
//kAudioFileWAVEType
//0: mFormatId: lpcm, mFormatFlags: 8, mBitsPerChannel: 8
//1: mFormatId: lpcm, mFormatFlags: 12, mBitsPerChannel: 16
//2: mFormatId: lpcm, mFormatFlags: 12, mBitsPerChannel: 24
//3: mFormatId: lpcm, mFormatFlags: 12, mBitsPerChannel: 32
//4: mFormatId: lpcm, mFormatFlags: 9, mBitsPerChannel: 32
//5: mFormatId: lpcm, mFormatFlags: 9, mBitsPerChannel: 64
+ (void)inspectAFSupport;

//record voice
+ (void)audioQueueBasicRecorder;

//playback audio
+ (void)audioQueueBasicPlayback;

//convert audio
+ (void)audioConvert;

//simple API convert audio
+ (void)simpleAudioConvert;

//audio unit player
+ (void)audioUnitPlayer;

//********private**********
//audioQueueBasicRecorder relate
+ (void)checkError:(OSStatus)status
         operation:(const char*)operation;
+ (OSStatus)myGetDefaultInputDeviceSampleRate:(Float64*)outSampleRate;
+ (void)myCopyEncoderCookieToFile:(AudioQueueRef)queue
                             fileID:(AudioFileID)theFile;
+ (int)myComputeRecordBufferSize:(const AudioStreamBasicDescription*)format
                      audioQueue:(AudioQueueRef)queue
                            time:(float)seconds;

//audioQueueBasicPlayback relate
+ (void)myCopyEncoderCookieToQueue:(AudioQueueRef)queue
                            fileID:(AudioFileID)theFile;
+ (void)calculateBytesForTime:(AudioFileID)inAudioFile
                         absd:(AudioStreamBasicDescription)inDesc
                         time:(Float64)inSeconds
                   bufferSize:(UInt32*)outBufferSize
                    packetNum:(UInt32*)outNumPackets;

//audioConvert relate
//nothing there

//simpleAudioConvert relate
//noting there

@end
#endif /* function_h */

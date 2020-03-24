//
//  function.m
//  CoreAudio
//
//  Created by User on 2018/8/21.
//  Copyright © 2018 User. All rights reserved.
//
#import "function.h"

@implementation Function

+ (void)getAudioInfo
{
    NSURL *audioURL = [NSURL fileURLWithPath:@"/Users/user/Desktop/xx.mp3"];
    AudioFileID audioFile;
    OSStatus theErr = noErr;
    theErr = AudioFileOpenURL((__bridge CFURLRef)audioURL,
                              kAudioFileReadPermission, 0,
                              &audioFile);
    assert (theErr == noErr);
    UInt32 dictionarySize = 0;
    theErr = AudioFileGetPropertyInfo (audioFile,kAudioFilePropertyInfoDictionary,&dictionarySize,0);
    assert (theErr == noErr);
    CFDictionaryRef dictionary;
    theErr = AudioFileGetProperty (audioFile,kAudioFilePropertyInfoDictionary,&dictionarySize,&dictionary);
    assert (theErr == noErr);
    NSLog (@"dictionary: %@", dictionary);
    CFRelease (dictionary);
    theErr = AudioFileClose (audioFile);
    assert (theErr == noErr);
}

#define SAMPLE_RATE 44100
#define DURATION 20
+ (void)createAudioFile
{
    double hz = 0.25;
    NSString *fileName = [NSString stringWithFormat:@"%0.3f-square.aif", hz];
    NSMutableString *filePath = [NSMutableString stringWithFormat:@"/Users/user/Desktop/"];
    [filePath appendString:fileName];
    NSURL *fileURL = [NSURL fileURLWithPath: filePath];
    // Prepare the format
    AudioStreamBasicDescription asbd;
    memset(&asbd, 0, sizeof(asbd));
    asbd.mSampleRate = SAMPLE_RATE;
    asbd.mFormatID = kAudioFormatLinearPCM;
    asbd.mFormatFlags = kAudioFormatFlagIsBigEndian | kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
    asbd.mBitsPerChannel = 16;
    asbd.mChannelsPerFrame = 1;
    asbd.mFramesPerPacket = 1;
    asbd.mBytesPerFrame = 2;
    asbd.mBytesPerPacket = 2;
    // Set up the file
    AudioFileID audioFile;
    OSStatus audioErr = noErr;
    audioErr = AudioFileCreateWithURL((__bridge CFURLRef)fileURL,kAudioFileAIFFType, &asbd, kAudioFileFlags_EraseFile, &audioFile);
    assert (audioErr == noErr);
    // Start writing samples
    long maxSampleCount = SAMPLE_RATE * DURATION;
    long sampleCount = 0;
    UInt32 bytesToWrite = 2;
    double wavelengthInSamples = SAMPLE_RATE / hz;
    while (sampleCount < maxSampleCount)
    {
        for (int i=0; i<wavelengthInSamples; i++)
        {
            // Square wave
            SInt16 sample;
            if (i < wavelengthInSamples/2)
            {
                sample = CFSwapInt16HostToBig (SHRT_MAX);    
            }
            else
            {
                sample = CFSwapInt16HostToBig (SHRT_MIN);
            }
            audioErr = AudioFileWriteBytes(audioFile, false, sampleCount*2, &bytesToWrite, &sample);
            assert (audioErr == noErr);
            sampleCount++;
        }
    }
    audioErr = AudioFileClose(audioFile);
    assert (audioErr == noErr);
    NSLog (@"wrote %ld samples", sampleCount);
}

+ (void)inspectAFSupport
{
    AudioFileTypeAndFormatID fileTypeAndFormat;
    fileTypeAndFormat.mFileType = kAudioFileWAVEType;
    fileTypeAndFormat.mFormatID = kAudioFormatLinearPCM;
    
    OSStatus audioErr = noErr;
    UInt32 infoSize = 0;
    
    audioErr = AudioFileGetGlobalInfoSize(kAudioFileGlobalInfo_AvailableStreamDescriptionsForFormat, sizeof(fileTypeAndFormat), &fileTypeAndFormat, &infoSize);
    assert(audioErr == noErr);
    
    AudioStreamBasicDescription* asbds = malloc(infoSize);
    audioErr = AudioFileGetGlobalInfo(kAudioFileGlobalInfo_AvailableStreamDescriptionsForFormat, sizeof(fileTypeAndFormat), &fileTypeAndFormat, &infoSize, asbds);
    assert(audioErr == noErr);
    
    int asbdCount = infoSize / sizeof(AudioStreamBasicDescription);
    for (int i=0; i<asbdCount; i++)
    {
        UInt32 format4cc = CFSwapInt32HostToBig(asbds[i].mFormatID);
        NSLog (@"%d: mFormatId: %4.4s, mFormatFlags: %d, mBitsPerChannel: %d",i, (char*)&format4cc,asbds[i].mFormatFlags,asbds[i].mBitsPerChannel);
    }
    free(asbds);
}

//////////////////////recoder//////////////////////////////
#pragma mark user data struct
typedef struct MyRecorder
{
    AudioFileID recordFile;
    SInt64 recordPacket;
    BOOL running;
}MyRecoder;


#pragma mark utility functions
+ (void)checkError:(OSStatus)status
         operation:(const char *)operation
{
    if (status == noErr)
        return;
    
    char errorString[20];
    // See if it appears to be a 4-char-code
    *(UInt32 *)(errorString + 1) = CFSwapInt32HostToBig(status);
    if (isprint(errorString[1]) && isprint(errorString[2]) && isprint(errorString[3]) && isprint(errorString[4]))
    {
        errorString[0] = errorString[5] = '\'';
        errorString[6] = '\0';
    }
    else
    {
        // No, format it as an integer
        sprintf(errorString, "%d", (int)status);
    }
    fprintf(stderr, "Error: %s (%s)\n", operation, errorString);
    exit(1);
}

+ (OSStatus)myGetDefaultInputDeviceSampleRate:(Float64*)outSampleRate
{
    OSStatus error;
    AudioDeviceID deviceID = 0;
    AudioObjectPropertyAddress propertyAddress;
    UInt32 propertySize;
    propertyAddress.mSelector = kAudioHardwarePropertyDefaultInputDevice;
    propertyAddress.mScope = kAudioObjectPropertyScopeGlobal;
    propertyAddress.mElement = 0;
    propertySize = sizeof(AudioDeviceID);
    error = AudioHardwareServiceGetPropertyData(kAudioObjectSystemObject,
                                                &propertyAddress,
                                                0,
                                                NULL,
                                                &propertySize,
                                                &deviceID);
    if(error)
    {
         return error;
    }
    
    propertyAddress.mSelector = kAudioDevicePropertyNominalSampleRate;
    propertyAddress.mScope = kAudioObjectPropertyScopeGlobal;
    propertyAddress.mElement = 0;
    propertySize = sizeof(Float64);
    error = AudioHardwareServiceGetPropertyData(deviceID,
                                                &propertyAddress,
                                                0,
                                                NULL,
                                                &propertySize,
                                                outSampleRate);
    return error;
}

+ (void)myCopyEncoderCookieToFile:(AudioQueueRef)queue
                           fileID:(AudioFileID)theFile
{
    OSStatus error;
    UInt32 propertySize;
    
    error = AudioQueueGetPropertySize(queue, kAudioConverterCompressionMagicCookie, &propertySize);
    if(error == noErr && propertySize > 0)
    {
        Byte* magicCookie = (Byte*)malloc(propertySize);
        [self checkError:AudioQueueGetProperty(queue,
                                               kAudioQueueProperty_MagicCookie,
                                               magicCookie,
                                               &propertySize)
               operation:"Couldn't get audio queue's magic cookie"];
        [self checkError:AudioFileSetProperty(theFile,
                                              kAudioFilePropertyMagicCookieData,
                                              propertySize,
                                              magicCookie)
               operation:"Couldn't set audio file's magic cookie"];
        free(magicCookie);
    }
}

+ (int)myComputeRecordBufferSize:(const AudioStreamBasicDescription *)format
                      audioQueue:(AudioQueueRef)queue
                            time:(float)seconds
{
    int packets,frames,bytes;
    frames = (int)ceil(seconds * format->mSampleRate);
    
    if(format->mBytesPerFrame > 0)
    {
        bytes = frames * format->mBytesPerFrame;
    }
    else
    {
        UInt32 maxPacketSize;
        if(format->mBytesPerPacket > 0)
        {
            // Constant packet size
            maxPacketSize = format->mBytesPerPacket;
        }
        else
        {
            // Get the largest single packet size possible
            UInt32 propertySize = sizeof(maxPacketSize);
            [self checkError:AudioQueueGetProperty(queue,
                                                   kAudioConverterPropertyMaximumOutputPacketSize,
                                                   &maxPacketSize,
                                                   &propertySize)
                   operation:"Couldn't get queue's maximum output packet size"];
        }
        if(format->mFramesPerPacket > 0)
        {
            packets = frames / format->mFramesPerPacket;
        }
        else
        {
            packets = frames;
        }
        
        if(packets == 0)
        {
            packets = 1;
        }
        bytes = packets * maxPacketSize;
    }
    return bytes;
}
// Insert Listing 4.2 here
// Insert Listings 4.20 and 4.21 here // Insert Listing 4.22 here
// Insert Listing 4.23 here
#pragma mark record callback function
// Replace with Listings 4.24-4.26
static void myAQInputCallbackForRecorder(void* inUserData,
                                         AudioQueueRef inQueue,
                                         AudioQueueBufferRef inBuffer,
                                         const AudioTimeStamp* inStartTime,
                                         UInt32 inNumPackets,
                                         const AudioStreamPacketDescription* inPacketDesc)
{
    MyRecoder* recorder = (MyRecoder*)inUserData;
    if(inNumPackets > 0)
    {
        [Function checkError:AudioFileWritePackets(recorder->recordFile,
                                               FALSE,
                                               inBuffer->mAudioDataByteSize,
                                               inPacketDesc,
                                               recorder->recordPacket,
                                               &inNumPackets,
                                               inBuffer->mAudioData)
               operation:"AudioFileWritePackets failed"];
        // Increment the packet index
        recorder->recordPacket += inNumPackets;
    }
    
    if(recorder->running == YES)
    {
        [Function checkError:AudioQueueEnqueueBuffer(inQueue,
                                                 inBuffer,
                                                 0,
                                                 NULL)
               operation:"AudioQueueEnqueueBuffer failed"];
    }
}

#define kNumberRecordBuffers 3
+ (void)audioQueueBasicRecorder
{
//    Set up format
//    Insert Listings 4.4-4.7 here
    MyRecoder recorder = {0};
    AudioStreamBasicDescription recordFormat;
    memset(&recordFormat, 0, sizeof(recordFormat));
    recordFormat.mFormatID = kAudioFormatMPEG4AAC;
    recordFormat.mChannelsPerFrame = 2;
    //get corrent device sample rate
    [self checkError:[self myGetDefaultInputDeviceSampleRate:&recordFormat.mSampleRate]
           operation:"Get input device sample rate error"];

    UInt32 propSize = sizeof(recordFormat);
    [self checkError:AudioFormatGetProperty(kAudioFormatProperty_FormatInfo,
                                            0,
                                            NULL,
                                            &propSize,
                                            &recordFormat)
           operation:"AudioFormatGetProperty failed"];
    //    Set up queue
    //    Insert Listings 4.8-4.9 here
    //    Set up file
    AudioQueueRef queue = {0};
    [self checkError:AudioQueueNewInput(&recordFormat,
                                        myAQInputCallbackForRecorder,
                                        &recorder,
                                        NULL,
                                        NULL,
                                        0,
                                        &queue)
           operation:"AudioQueueNewInput failed"];
    
    UInt32 size = sizeof(recordFormat);
    [self checkError:AudioQueueGetProperty(queue,
                                           kAudioConverterCurrentOutputStreamDescription,
                                           &recordFormat,
                                           &size)
           operation:"Couldn't get queue's format"];
//    Insert Listings 4.10-4.11 here
//     Other setup as needed
    NSURL* outputURL = [NSURL fileURLWithPath:@"/Users/user/Desktop/output.caf"];
    CFURLRef myFileURL = (__bridge CFURLRef)outputURL;
    [self checkError:AudioFileCreateWithURL(myFileURL,
                                            kAudioFileCAFType,
                                            &recordFormat,
                                            kAudioFileFlags_EraseFile,
                                            &recorder.recordFile)
           operation:"AudioFileCreateWithURL failed"];
    CFRelease(myFileURL);
    [self myCopyEncoderCookieToFile:queue fileID:recorder.recordFile];
//     Insert Listings 4.12-4.13 here、
    int bufferByteSize = [self myComputeRecordBufferSize:&recordFormat audioQueue:queue time:0.5];
    int bufferIndex;
    for(bufferIndex = 0; bufferIndex < kNumberRecordBuffers; ++bufferIndex)
    {
        AudioQueueBufferRef buffer;
        [self checkError:AudioQueueAllocateBuffer(queue,
                                                  bufferByteSize,
                                                  &buffer)
               operation:"AudioQueueAllocateBuffer failed"];
        [self checkError:AudioQueueEnqueueBuffer(queue,
                                                 buffer,
                                                 0,
                                                 NULL)
               operation:"AudioQueueEnqueueBuffer failed"];
    }
//     Start queue
    recorder.running = YES;
    [self checkError:AudioQueueStart(queue,
                                     NULL)
           operation:"AudioQueueStart failed"];
    
//     Insert Listings 4.14-4.15 here
    NSLog(@"Recording, press <return> to stop:\n");
    getchar();
    NSLog(@"Recording done\n");
    recorder.running = NO;
    [self checkError:AudioQueueStop(queue,
                                    TRUE)
           operation:"AudioQueueStop Failed"];
    
//     Stop queue
//     Insert Listings 4.16-4.18 here
    [self myCopyEncoderCookieToFile:queue fileID:recorder.recordFile];
    AudioQueueDispose(queue, TRUE);
    AudioFileClose(recorder.recordFile);

}
//////////////////////recoder//////////////////////////////

//////////////////////playback//////////////////////////////
#define kNumberPlaybackBuffers 3
#pragma mark user data struct
// Insert Listing 5.2 here
typedef struct MyPlayer
{
    AudioFileID playbackFile;
    SInt64 packetPosition;
    UInt32 numPacketsToRead;
    AudioStreamPacketDescription* packetDescs;
    Boolean isDone;
}MyPlayer;
#pragma mark utility functions
// Insert Listing 4.2 here
// Insert Listing 5.14 here
+ (void)myCopyEncoderCookieToQueue:(AudioQueueRef)queue
                            fileID:(AudioFileID)theFile
{
    UInt32 propertySize;
    OSStatus result = AudioFileGetPropertyInfo(theFile,
                                               kAudioFilePropertyMagicCookieData,
                                               &propertySize,
                                               NULL);
    if(result == noErr && propertySize > 0)
    {
        Byte* magicCookie = (UInt8*)malloc(sizeof(UInt8) * propertySize);
        [self checkError:AudioFileGetProperty(theFile,
                                              kAudioFilePropertyMagicCookieData,
                                              &propertySize,
                                              magicCookie)
               operation:"Get cookie from file failed"];
        [self checkError:AudioQueueSetProperty(queue,
                                               kAudioQueueProperty_MagicCookie,
                                               magicCookie,
                                               propertySize)
               operation:"Set cookie on queue failed"];
        free(magicCookie);
    }
}
// Insert Listing 5.15 here
+ (void)calculateBytesForTime:(AudioFileID)inAudioFile
                         absd:(AudioStreamBasicDescription)inDesc
                         time:(Float64)inSeconds
                   bufferSize:(UInt32 *)outBufferSize
                    packetNum:(UInt32 *)outNumPackets
{
    UInt32 maxPacketSize;
    UInt32 propSize = sizeof(maxPacketSize);
    [self checkError:AudioFileGetProperty(inAudioFile,
                                          kAudioFilePropertyPacketSizeUpperBound,
                                          &propSize,
                                          &maxPacketSize)
           operation:"Couldn't get file's max packet size"];
    
    static const int maxBufferSize = 0x10000;
    static const int minBufferSize = 0x4000;
    
    if(inDesc.mFramesPerPacket)
    {
        Float64 numPacketsForTime = inDesc.mSampleRate / inDesc.mFramesPerPacket * inSeconds;
        *outBufferSize = maxPacketSize * numPacketsForTime;
    }
    else
    {
        *outBufferSize = maxBufferSize > maxPacketSize ? maxBufferSize : maxPacketSize;
    }
    
    if(*outBufferSize > maxBufferSize &&
       *outBufferSize > maxPacketSize)
    {
        *outBufferSize = maxBufferSize;
    }
    else
    {
        if(*outBufferSize < minBufferSize)
        {
            *outBufferSize = minBufferSize;
        }
    }
    *outNumPackets = *outBufferSize / maxPacketSize;
}
#pragma mark playback callback function
// Replace with Listings 5.16-5.19
static void myAQOutputCallbackForPlayback(void *inUserData,
                                          AudioQueueRef inAQ,
                                          AudioQueueBufferRef inCompleteAQBuffer)
{
    MyPlayer *aqp = (MyPlayer*)inUserData;
    if(aqp->isDone)
        return;
    
    UInt32 numBytes;
    UInt32 nPackets = aqp->numPacketsToRead;
    [Function checkError:AudioFileReadPackets(aqp->playbackFile,
                                              false,
                                              &numBytes,
                                              aqp->packetDescs,
                                              aqp->packetPosition,
                                              &nPackets,
                                              inCompleteAQBuffer->mAudioData)
               operation:"AudioFileReadPackets failed"];
    if(nPackets > 0)
    {
        inCompleteAQBuffer->mAudioDataByteSize = numBytes;
        AudioQueueEnqueueBuffer(inAQ,
                                inCompleteAQBuffer,
                                (aqp->packetDescs ? nPackets : 0),
                                aqp->packetDescs);
        aqp->packetPosition += nPackets;
    }
    else
    {
        [Function checkError:AudioQueueStop(inAQ,
                                            false)
                   operation:"AudioQueueStop failed"];
        aqp->isDone = true;
    }
}

+ (void)audioQueueBasicPlayback
{
//    Open an audio file
//    Insert Listings 5.3-5.4 here
    MyPlayer player = {0};
    NSURL* fileURL = [NSURL fileURLWithPath:@"/Users/user/Desktop/xx.mp3"];
    CFURLRef myFileURL = (__bridge CFURLRef)fileURL;
    [self checkError:AudioFileOpenURL(myFileURL,
                                      kAudioFileReadPermission,
                                      0,
                                      &player.playbackFile)
           operation:"AudioFileOpenURL failed"];
    CFRelease(myFileURL);
//    Set up format
//    Insert Listing 5.5 here
    AudioStreamBasicDescription dataFormat;
    UInt32 propSize = sizeof(dataFormat);
    [self checkError:AudioFileGetProperty(player.playbackFile,
                                          kAudioFilePropertyDataFormat,
                                          &propSize,
                                          &dataFormat)
           operation:"Couldn`t get file`s data format"];
//     Set up queue
//     Insert Listings 5.6-5.10 here
    AudioQueueRef queue;
    [self checkError:AudioQueueNewOutput(&dataFormat,
                                         myAQOutputCallbackForPlayback,
                                         &player,
                                         NULL,
                                         NULL,
                                         0,
                                         &queue)
           operation:"AudioQueueNewOutput Failed"];
    UInt32 bufferByteSize;
    [self calculateBytesForTime:player.playbackFile
                           absd:dataFormat
                           time:0.5
                     bufferSize:&bufferByteSize
                      packetNum:&player.numPacketsToRead];
    bool isFormatVBR = (dataFormat.mBytesPerFrame == 0) || (dataFormat.mFramesPerPacket == 0);
    if(isFormatVBR)
    {
        player.packetDescs = (AudioStreamPacketDescription*)malloc(sizeof(AudioStreamPacketDescription)*player.numPacketsToRead);
    }
    else
    {
        player.packetDescs = NULL;
    }
    [self myCopyEncoderCookieToQueue:queue
                              fileID:player.playbackFile];
    AudioQueueBufferRef buffers[kNumberPlaybackBuffers];
    player.isDone = false;
    player.packetPosition = 0;
    int i;
    for(i = 0; i < kNumberPlaybackBuffers; ++i)
    {
        [self checkError:AudioQueueAllocateBuffer(queue,
                                                  bufferByteSize,
                                                  &buffers[i])
               operation:"AudioQueueAllocateBuffer failed"];
        myAQOutputCallbackForPlayback(&player, queue, buffers[i]);
        if(player.isDone)
        {
            break;
        }
    }
//     Start queue
//     Insert Listing 5.11-5.12 here
    [self checkError:AudioQueueStart(queue,
                                     NULL)
           operation:"AudioQueueStart failed"];
    NSLog(@"Playering...\n");
    do
    {
        CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0.25, false);
    }while(!player.isDone);
    CFRunLoopRunInMode(kCFRunLoopDefaultMode, 2, false);
//     Clean up queue when finished
//     Insert Listing 5.13 here
    player.isDone = true;
    [self checkError:AudioQueueStop(queue,
                                    TRUE)
           operation:"AudioQueueStop failed"];
    AudioQueueDispose(queue, TRUE);
    AudioFileClose(player.playbackFile);
}
//////////////////////playback//////////////////////////////

//////////////////////convert//////////////////////////////
#pragma mark user data struct
typedef struct MyAudioConverterSettings
{
    AudioStreamBasicDescription inputFormat;
    AudioStreamBasicDescription outputFormat;
    
    AudioFileID inputFile;
    AudioFileID outputFile;
    
    UInt64 inputFilePacketIndex;
    UInt64 inputFilePacketCount;
    UInt32 inputFilePacketMaxSize;
    AudioStreamPacketDescription* inputFilePacketDescriptions;
    
    void* sourceBuffer;
}MyAudioConverterSettings;
#pragma mark utility functions
void convert(MyAudioConverterSettings* mySettings)
{
    //Create the audioConvertrt object
    AudioConverterRef audioConverter;
    [Function checkError:AudioConverterNew(&mySettings->inputFormat,
                                           &mySettings->outputFormat,
                                           &audioConverter)
               operation:"AudioConvertNew failed"];
    UInt32 packetsPerBuffer = 0;
    UInt32 outputBufferSize = 32 * 1024; //32KB
    UInt32 sizePerPacket = mySettings->inputFormat.mBytesPerPacket;
    if(sizePerPacket == 0)
    {
        UInt32 size = sizeof(sizePerPacket);
        [Function checkError:AudioConverterGetProperty(audioConverter,
                                                       kAudioConverterPropertyMaximumOutputPacketSize,
                                                       &size,
                                                       &sizePerPacket)
                   operation:"Couldn't get kAudioConverterPropertyMaximumOutputPacketSize"];
        if(sizePerPacket > outputBufferSize)
            outputBufferSize = sizePerPacket;
        
        packetsPerBuffer = outputBufferSize / sizePerPacket;
        mySettings->inputFilePacketDescriptions = (AudioStreamPacketDescription*)malloc(sizeof(AudioStreamPacketDescription)*packetsPerBuffer);
    }
    else
    {
        packetsPerBuffer = outputBufferSize / sizePerPacket;
    }
    
    UInt8* outputBuffer = (UInt8*)malloc(sizeof(UInt8) * outputBufferSize);
    UInt32 outputFilePacketPosition = 0;
    while(1)
    {
        AudioBufferList convertedData;
        convertedData.mNumberBuffers = 1;
        convertedData.mBuffers[0].mNumberChannels = mySettings->inputFormat.mChannelsPerFrame;
        convertedData.mBuffers[0].mDataByteSize = outputBufferSize;
        convertedData.mBuffers[0].mData = outputBuffer;
        UInt32 ioOutputDataPackets = packetsPerBuffer;
        OSStatus error = AudioConverterFillComplexBuffer(audioConverter,
                                                         myAudioConverterCallback,
                                                         mySettings,
                                                         &ioOutputDataPackets,
                                                         &convertedData,
                                                         (mySettings->inputFilePacketDescriptions ? mySettings->inputFilePacketDescriptions : nil));
        if(error || !ioOutputDataPackets)
        {
            break;
        }
        
        [Function checkError:AudioFileWritePackets(mySettings->outputFile,
                                                   FALSE,
                                                   ioOutputDataPackets,
                                                   NULL,
                                                   outputFilePacketPosition / mySettings->outputFormat.mBytesPerPacket,
                                                   &ioOutputDataPackets,
                                                   convertedData.mBuffers[0].mData)
                   operation:"could not write packets to file"];
        outputFilePacketPosition += (ioOutputDataPackets * mySettings->outputFormat.mBytesPerPacket);
    }
    AudioConverterDispose(audioConverter);
}

#pragma mark converter callback function
OSStatus myAudioConverterCallback(AudioConverterRef inAudioConverter,
                                  UInt32 *ioDataPacketCount,
                                  AudioBufferList *ioData,
                                  AudioStreamPacketDescription **outDataPacketDescription,
                                  void *inUserData)
{
    MyAudioConverterSettings* audioConverterSettings = (MyAudioConverterSettings*)inUserData;
    
    ioData->mBuffers[0].mData = NULL;
    ioData->mBuffers[0].mDataByteSize = 0;
    
    if(audioConverterSettings->inputFilePacketIndex + *ioDataPacketCount > audioConverterSettings->inputFilePacketCount)
    {
        //maybe ioDataPacketCount ? Int64
        *ioDataPacketCount = (UInt32)(audioConverterSettings->inputFilePacketCount - audioConverterSettings->inputFilePacketIndex);
    }
    
    if(*ioDataPacketCount == 0)
        return noErr;
    
    if(audioConverterSettings->sourceBuffer != NULL)
    {
        free(audioConverterSettings->sourceBuffer);
        audioConverterSettings->sourceBuffer = NULL;
    }
    
    audioConverterSettings->sourceBuffer = (void*)calloc(1, *ioDataPacketCount * audioConverterSettings->inputFilePacketMaxSize);
    
    UInt32 outByteCount = 0;
    OSStatus result = AudioFileReadPackets(audioConverterSettings->inputFile,
                                           true,
                                           &outByteCount,
                                           audioConverterSettings->inputFilePacketDescriptions,
                                           audioConverterSettings->inputFilePacketIndex,
                                           ioDataPacketCount,
                                           audioConverterSettings->sourceBuffer);
    
    if(result == eofErr && *ioDataPacketCount)
    {
        result = noErr;
    }
    else if (result != noErr)
    {
        return result;
    }
    
    audioConverterSettings->inputFilePacketIndex += *ioDataPacketCount;
    ioData->mBuffers[0].mData = audioConverterSettings->sourceBuffer;
    ioData->mBuffers[0].mDataByteSize = outByteCount;
    if(outDataPacketDescription)
    {
        *outDataPacketDescription = audioConverterSettings->inputFilePacketDescriptions;
    }
    return result;
}

+ (void)audioConvert
{
    MyAudioConverterSettings audioConverterSettings = {0};
    NSURL* fileURL = [NSURL fileURLWithPath:@"/Users/user/Desktop/xx.mp3"];
    CFURLRef inputFileURL = (__bridge CFURLRef)fileURL;
    [self checkError:AudioFileOpenURL(inputFileURL,
                                      kAudioFileReadPermission,
                                      0,
                                      &audioConverterSettings.inputFile)
           operation:"AudioFileOpenURL failed"];
    
    CFRelease(inputFileURL);
    //set input file info
    UInt32 propSize = sizeof(audioConverterSettings.inputFormat);
    [self checkError:AudioFileGetProperty(audioConverterSettings.inputFile,
                                          kAudioFilePropertyDataFormat,
                                          &propSize,
                                          &audioConverterSettings.inputFormat)
           operation:"Couldn't get file's data format"];
    
    propSize = sizeof(audioConverterSettings.inputFilePacketCount);
    [self checkError:AudioFileGetProperty(audioConverterSettings.inputFile,
                                          kAudioFilePropertyAudioDataPacketCount,
                                          &propSize,
                                          &audioConverterSettings.inputFilePacketCount)
           operation:"couldn't get file's packet count"];
    
    propSize = sizeof(audioConverterSettings.inputFilePacketMaxSize);
    [self checkError:AudioFileGetProperty(audioConverterSettings.inputFile,
                                          kAudioFilePropertyMaximumPacketSize,
                                          &propSize,
                                          &audioConverterSettings.inputFilePacketMaxSize)
           operation:"couldn't get file's max packet size"];
    
    //set output file info
    audioConverterSettings.outputFormat.mSampleRate = 44100.0;
    audioConverterSettings.outputFormat.mFormatID = kAudioFormatLinearPCM;
    audioConverterSettings.outputFormat.mFormatFlags = kAudioFormatFlagIsBigEndian | kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
    audioConverterSettings.outputFormat.mBytesPerPacket = 4;
    audioConverterSettings.outputFormat.mFramesPerPacket = 1;
    audioConverterSettings.outputFormat.mBytesPerFrame = 4;
    audioConverterSettings.outputFormat.mChannelsPerFrame = 2;
    audioConverterSettings.outputFormat.mBitsPerChannel = 16;
    
    NSURL* outputURL = [NSURL fileURLWithPath:@"/Users/user/Desktop/converted.aif"];
    CFURLRef outputFileURL = (__bridge CFURLRef)outputURL;
    [self checkError:AudioFileCreateWithURL(outputFileURL,
                                            kAudioFileAIFFType,
                                            &audioConverterSettings.outputFormat,
                                            kAudioFileFlags_EraseFile,
                                            &audioConverterSettings.outputFile)
           operation:"AudioFileCreateWithURL failed"];
    CFRelease(outputFileURL);
    
    NSLog(@"Converting...\n");
    convert(&audioConverterSettings);
    
    AudioFileClose(audioConverterSettings.inputFile);
    AudioFileClose(audioConverterSettings.outputFile);
}
//////////////////////convert//////////////////////////////
//////////////////////simple API convert//////////////////////////////
typedef struct MySimpleAudioConverterSettings
{
    AudioStreamBasicDescription outputFormat;
    ExtAudioFileRef inputFile;
    AudioFileID outputFile;
}MySimpleAudioConverterSettings;

void simpleConvert(MySimpleAudioConverterSettings *mySettings)
{
    UInt32 outputBufferSize = 32 * 1024; //32KB
    UInt32 sizePerPacket = mySettings->outputFormat.mBytesPerPacket;
    UInt32 packetsPerBuffer = outputBufferSize / sizePerPacket;
    UInt8* outputBuffer = (UInt8*)malloc(sizeof(UInt8) * outputBufferSize);
    UInt32 outputFilePacketPosition = 0; //In bytes
    
    while(1)
    {
        AudioBufferList convertedData;
        convertedData.mNumberBuffers = 1;
        convertedData.mBuffers[0].mNumberChannels = mySettings->outputFormat.mChannelsPerFrame;
        convertedData.mBuffers[0].mDataByteSize = outputBufferSize;
        convertedData.mBuffers[0].mData = outputBuffer;
        
        UInt32 frameCount = packetsPerBuffer;
        [Function checkError:ExtAudioFileRead(mySettings->inputFile,
                                              &frameCount,
                                              &convertedData)
                   operation:"can not read from input file"];
        if(frameCount == 0)
        {
            NSLog(@"Done reading from file\n");
            return;
        }
        
        [Function checkError:AudioFileWritePackets(mySettings->outputFile,
                                                   FALSE,
                                                   frameCount,
                                                   NULL,
                                                   outputFilePacketPosition / mySettings->outputFormat.mBytesPerPacket,
                                                   &frameCount,
                                                   convertedData.mBuffers[0].mData)
                   operation:"can not write packet to file"];
        outputFilePacketPosition += (frameCount * mySettings->outputFormat.mBytesPerPacket);
    }
}

+ (void)simpleAudioConvert
{
    MySimpleAudioConverterSettings simpleAudioConverterSettings = {0};
    //open file
    NSURL* inputURL = [NSURL fileURLWithPath:@"/Users/user/Desktop/xx.mp3"];
    CFURLRef inputFileURL = (__bridge CFURLRef)inputURL;
    [self checkError:ExtAudioFileOpenURL(inputFileURL,
                                         &simpleAudioConverterSettings.inputFile)
           operation:"ExtAudioFileOpenURL failed"];
    CFRelease(inputFileURL);
    //set output file info
    simpleAudioConverterSettings.outputFormat.mSampleRate = 44100.0;
    simpleAudioConverterSettings.outputFormat.mFormatID = kAudioFormatLinearPCM;
    simpleAudioConverterSettings.outputFormat.mFormatFlags = kAudioFormatFlagIsBigEndian | kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
    simpleAudioConverterSettings.outputFormat.mBytesPerPacket = 4;
    simpleAudioConverterSettings.outputFormat.mFramesPerPacket = 1;
    simpleAudioConverterSettings.outputFormat.mBytesPerFrame = 4;
    simpleAudioConverterSettings.outputFormat.mChannelsPerFrame = 2;
    simpleAudioConverterSettings.outputFormat.mBitsPerChannel = 16;
    
    NSURL* outputURL = [NSURL fileURLWithPath:@"/Users/user/Desktop/converted.aif"];
    CFURLRef outputFileURL = (__bridge CFURLRef)outputURL;
    [self checkError:AudioFileCreateWithURL(outputFileURL,
                                            kAudioFileAIFFType,
                                            &simpleAudioConverterSettings.outputFormat,
                                            kAudioFileFlags_EraseFile,
                                            &simpleAudioConverterSettings.outputFile)
           operation:"AudioFileCreateWithURL failed"];
    CFRelease(outputFileURL);
    
    [self checkError:ExtAudioFileSetProperty(simpleAudioConverterSettings.inputFile,
                                             kExtAudioFileProperty_ClientDataFormat,
                                             sizeof(AudioStreamBasicDescription),
                                             &simpleAudioConverterSettings.outputFormat)
           operation:"Couldn't set client data format on input ext file"];
    
    NSLog(@"Converting...\n");
    simpleConvert(&simpleAudioConverterSettings);
    
    ExtAudioFileDispose(simpleAudioConverterSettings.inputFile);
    AudioFileClose(simpleAudioConverterSettings.outputFile);
}
//////////////////////simple API convert//////////////////////////////

//////////////////////audio unit player//////////////////////////////
typedef struct MyAUGraphPlayer
{
    AudioStreamBasicDescription inputFormat;
    AudioFileID inputFile;
    AUGraph graph;
    AudioUnit fileAU;
}MyAUGraphPlayer;

+ (void)audioUnitPlayer
{
    MyAUGraphPlayer player = {0};
    NSURL* inputURL = [NSURL fileURLWithPath:@"/Users/user/Desktop/xx.mp3"];
    CFURLRef inputFileURL = (__bridge CFURLRef)inputURL;
    [self checkError:AudioFileOpenURL(inputFileURL,
                                      kAudioFileReadPermission,
                                      0,
                                      &player.inputFile)
           operation:"AudioFileOpenURL failed"];
    
    UInt32 propSize = sizeof(player.inputFormat);
    [self checkError:AudioFileGetProperty(player.inputFile,
                                          kAudioFilePropertyDataFormat,
                                          &propSize,
                                          &player.inputFormat)
           operation:"can not get file data format"];
    CFRelease(inputFileURL);
    //Build a basic fileplayer->speakers graph
    CreateMyAUGraph(&player);
    
    //Configure the file player
    Float64 fileDuration = PrepareFileAU(&player);

}
//////////////////////audio unit player//////////////////////////////
@end

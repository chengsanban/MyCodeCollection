//
//  highestAVConverter.h
//  Created by chengsanban on 2018/8/2.

/*************************
 高级api，只能设置导出类型，导出音频，视频的编码等参数，视频帧率，视频时长 ,视频起始位置
 取消导出，获取导出进度
 *************************/

#ifndef highestAVConverter_h
#define highestAVConverter_h

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>

typedef void (^ProgressCallBack)(float progress);

@interface HighestAVConverter : NSObject

@property (strong) AVAssetReaderAudioMixOutput* audioReaderOutput;
@property (strong) AVAssetReaderVideoCompositionOutput* videoReaderOutput;
@property (strong) AVAssetWriterInput* audioWriterInput;
@property (strong) AVAssetWriterInput* videoWriterinput;
@property (copy) NSDictionary* audioEncodeSettings;
@property (copy) NSDictionary* videoEncodeSettings;
@property (strong) AVAssetReader* assetReader;
@property (strong) AVAssetWriter* assetWriter;
@property (assign) BOOL cancelled;
@property (assign) BOOL audioFinished;
@property (assign) BOOL videoFinished;
@property (strong) AVAsset* asset;
@property (strong) NSURL* srcVideoURL;
@property (strong) NSURL* dstVideoURL;
@property (strong) AVFileType dstFileType;
@property (assign) dispatch_queue_t mainSerializationQueue;
@property (assign) dispatch_queue_t rwAudioSerializationQueue;
@property (assign) dispatch_queue_t rwVideoSerializationQueue;
@property (assign) dispatch_group_t dispatchGroup;
@property (assign) double frameRate;
@property (assign) int frameTimeScale;
@property (strong) ProgressCallBack callback;
@property (strong) NSError *error;
@property (assign) CMSampleBufferRef lastSampleBuffer;
@property (assign) CMSampleBufferRef curSampleBuffer;
@property (assign) CMTime duration;
@property (assign) CMTime movieStartTime;

//外部用
- (id)initWithSrcPath:(NSString *)srcVideoPath
              dstPath:(NSString *)dstVideoPath
              dstType:(AVFileType)dstFileType;
- (void)initEncodeVideoSettings:(NSDictionary*)videoSettings;
- (void)initEncodeAudioSettings:(NSDictionary*)audioSettings;
- (void)startConvertWithDuration:(int)duration 
                  movieStartTime:(int)movieStartTime 
                        Progress:(ProgressCallBack)progress;
- (void)cancel;

//内部用
- (BOOL)setupAssetReaderAndAssetWriter:(NSError**) outError;
- (BOOL)startAssetReaderAndWriter:(NSError**) outError;
- (void)readingAndWritingDidFinishSuccessfully:(BOOL)success withError:(NSError*)localError;

@end
#endif /* highestAVConverter_h */

//
//  middleAVConverter.h
//  Created by chengsanban on 2018/8/2.

/*************************
 中级api，只能设置导出类型，导出音频，视频的编码等参数
 取消导出，获取导出进度
 *************************/

#ifndef middleAVConverter_h
#define middleAVConverter_h

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>

@interface MiddleAVConverter : NSObject

@property (strong) NSURL* srcVideoURL;
@property (strong) NSURL* dstVideoURL;
@property (strong) AVFileType dstFileType;
@property BOOL cancelled;
@property BOOL audioFinished;
@property BOOL videoFinished;
@property (strong) AVAsset* asset;
@property (strong) AVAssetReaderOutput* audioReaderOutput;
@property (strong) AVAssetReaderOutput* videoReaderOutput;
@property (strong) AVAssetWriterInput* audioWriterInput;
@property (strong) AVAssetWriterInput* videoWriterinput;
@property (copy)NSDictionary* audioEncodeSettings;
@property (copy)NSDictionary* videoEncodeSettings;
@property (strong) AVAssetReader* assetReader;
@property (strong) AVAssetWriter* assetWriter;
@property (assign) dispatch_queue_t mainSerializationQueue;
@property (assign) dispatch_queue_t rwAudioSerializationQueue;
@property (assign) dispatch_queue_t rwVideoSerializationQueue;
@property (assign) dispatch_group_t dispatchGroup;

//外部调用
- (id)initWithSrcPath:(NSString*)srcPath
              dstPath:(NSString*)dstPath
              dstType:(AVFileType)dstFileType;

- (void) initEncodeVideoSettings:(NSDictionary*)videoSettings;

- (void) initEncodeAudioSettings:(NSDictionary*)audioSettings;

- (void) startConvert;

- (void) cancel;

//内部调用

- (BOOL) setupAssetReaderAndAssetWriter:(NSError**) outError;

- (BOOL) startAssetReaderAndWriter:(NSError**) outError;

- (void) readingAndWritingDidFinishSuccessfully:(BOOL)success
                                      withError:(NSError*)localError;

@end

#endif /* middleAVConverter_h */

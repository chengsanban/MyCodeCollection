//
//  lowAVConverter.h
//  Created by chengsanban on 2018/8/2.

/*************************
 简单api，只能设置导出类型
 取消导出，获取导出进度
 *************************/

#ifndef lowAVConverter_h
#define lowAVConverter_h
#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>

@interface LowAVConverter : NSObject

@property (strong) NSURL* srcVideoURL;
@property (strong) NSURL* dstVideoURL;
@property (strong) AVFileType dstFileType;
@property (strong) AVAssetExportSession* exporter;

- (id)initWithSrcPath:(NSString*)srcPath
              dstPath:(NSString*)dstPath
              dstType:(AVFileType)dstFileType;

- (void)startConvert;

- (void) convertDidFinish;

- (float) progress;

- (void) cancelConvert;
@end
#endif /* lowAVConverter_h */

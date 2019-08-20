//
//  main.m
//  OCTEST
//
//  Created by 周晨 on 2018/4/23.
//  Copyright © 2018 周晨. All rights reserved.
//

#import <Foundation/Foundation.h>
#import<string.h>
typedef enum
{
    kGreen,
    kRed,
    kBlue,
}ShapeColor;

typedef  struct
{
    int x,y,width,high;
}ShapeBound;

@interface Circle:NSObject
{
    ShapeColor color;
    ShapeBound bounder;
};

-(void) SetFillColor:(ShapeColor)fillColor;

-(void) SetShapeBound:(ShapeBound)bound;

-(void)Draw;
@end  //Circle

@implementation Circle
-(void) SetFillColor:(ShapeColor)inputColor
{
    color = inputColor;
    NSLog(@"color");
}

-(void) SetShapeBound:(ShapeBound)inputBound
{
    bounder = inputBound;
    NSLog(@"bounder");
}

-(void) Draw
{
    NSLog(@"x:%d/y:%d/width:%d/high:%d\n",bounder.x,bounder.y,bounder.width,bounder.high);
    NSLog(@"color:%d",color);
}
@end  //Circle

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        id myCircle = [Circle new];
        ShapeBound bound = {0,0,10,20};
        [myCircle SetFillColor:kRed];
        [myCircle SetShapeBound:bound];
        [myCircle Draw];
        
/*file output
        if(argc == 1)
        {
            NSLog(@"you need input a file");
            return 1;
        }
        FILE* file = fopen(argv[1],"r");
        char word[100];
        while(fgets(word,100,file))
        {
            word[strlen(word)-1] = '\0';
            NSLog(@"%s  %ld",word,strlen(word));
        }
        fclose(file);
 */
    }
    return 0;
}

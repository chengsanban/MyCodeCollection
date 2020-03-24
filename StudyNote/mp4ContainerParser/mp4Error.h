#ifndef MP4_ERROR_H
#define MP4_ERROR_H

#pragma warning(disable:4996)

#include"crossPlatform.h"

#define MP4ERROR Int32

#define MP4_OK 0
#define NULL_PTR -1
#define OUT_OF_FILE_BOUNDS -2
#define PARAM_INVALID -3
#define TYPE_STRING_NULL -4
#define TYPE_STRING_NUM_ERROR -5
#define NO_BOX_IN_FILE -6
#define UUID_NULL -7
#define UUID_NUM_ERROR -8
#define FILE_CAN_NOT_OPEN -9
#define MP4_BOX_TREE_GEN_FAIL -10
#define ROOT_NULL -11

#endif //MP4_ERROR_H
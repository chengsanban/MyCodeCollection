#ifndef	BIT_STREAM_H
#define BIT_STREAM_H

#include<iostream>
#include<fstream>
#include"mp4Error.h"
#include"crossPlatform.h"
using namespace std;

enum POSITION
{
	CUR,
	BEG,
	END,
};

/////////////////BitStream
class BitStream
{
public:
	BitStream(wchar_t* filePath, ios::openmode mode);
	~BitStream();
	MP4ERROR OpenStream();
	bool IsBigEndianMachine();
public:
	fstream m_FileStream;
	UInt64 m_isBigEndian;
};

/////////////////InBitStream
class InBitStream : public BitStream
{
public:
	InBitStream(wchar_t* filePath);
	~InBitStream();
	MP4ERROR OpenStream();
	//is consider byte order?
	MP4ERROR GetNByte(void* dst, Int32 count, bool byteOrder = false);
	//consider byte order
	MP4ERROR Get2ByteOrder(void* dst);
	MP4ERROR Get4ByteOrder(void* dst);
	MP4ERROR Get8ByteOrder(void* dst);
	MP4ERROR Get16ByteOrder(void* dst);
	// no consider byte order
	MP4ERROR GetByte(void* dst);
	MP4ERROR Get2Byte(void* dst);
	MP4ERROR Get3Byte(void* dst);
	MP4ERROR Get4Byte(void* dst);
	MP4ERROR Get8Byte(void* dst);
	MP4ERROR Get16Byte(void* dst);
	MP4ERROR Seek(UInt64 position);
	MP4ERROR Seek(UInt64 off, POSITION pos);
	MP4ERROR Peek(UInt64 count);
	Int64 GetFileSize();
	Int64 Tell();
private:
	UInt64 m_FileSize;
};

/////////////////OutBitStream
class OutBitStream : public BitStream
{
public:
	OutBitStream(wchar_t* filePath);
	OutBitStream();
	~OutBitStream();
	void WriteString(const char* string);
	void WriteChar(char ch);
	void FlushStream();
	void WriteUInt32Char2Stream(UInt32 UInt32Char);
	void WriteUIntNum2string(UInt64 UInt64Num);
	void WriteIntNum2string(Int64 Int64Num);
private:
};

#endif //BIT_STREAM_H
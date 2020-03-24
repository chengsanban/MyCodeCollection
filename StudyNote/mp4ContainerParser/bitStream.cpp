#include<assert.h>
#include"bitStream.h"

/////////////////BitStream
BitStream::BitStream(wchar_t* filePath, ios::openmode mode)
{
	m_isBigEndian = IsBigEndianMachine();
	m_FileStream.open(filePath, mode);
}

BitStream::~BitStream()
{
	if (m_FileStream.is_open())
		m_FileStream.close();
}

MP4ERROR BitStream::OpenStream()
{
	if (m_FileStream.is_open())
		return MP4_OK;
	else
		return FILE_CAN_NOT_OPEN;
	
}

bool BitStream::IsBigEndianMachine()
{
	union CheckEndian
	{
		UInt32 U32_number;
		UInt8 U8_number;
	};

	CheckEndian chenkEndian;
	chenkEndian.U32_number = 1;

	if (chenkEndian.U8_number == 1)
		return false;
	else
		return true;
}

/////////////////InBitStream
InBitStream::InBitStream(wchar_t* filePath)
:BitStream(filePath,ios::in | ios::binary)
{
}

InBitStream::~InBitStream()
{
}

MP4ERROR InBitStream::OpenStream()
{
	if (!m_FileStream.is_open())
		return FILE_CAN_NOT_OPEN;

	m_FileStream.seekg(0, ios_base::end);
	m_FileSize = m_FileStream.tellg();
	m_FileStream.seekg(0, ios_base::beg);
	return MP4_OK;
}

MP4ERROR InBitStream::Seek(UInt64 position)
{
	if (position > m_FileSize)
		return OUT_OF_FILE_BOUNDS;

	m_FileStream.seekg(position);
	return MP4_OK;
}

MP4ERROR InBitStream::Seek(UInt64 off, POSITION pos)
{
	if (off > m_FileSize)
		return OUT_OF_FILE_BOUNDS;

	if (pos == CUR)
		m_FileStream.seekg(off, ios_base::cur);
	else if (pos == BEG)
		m_FileStream.seekg(off, ios_base::beg);
	else if (pos == END)
		m_FileStream.seekg(off, ios_base::end);

	return MP4_OK;
}

Int64 InBitStream::Tell()
{
	return m_FileStream.tellg();
}

MP4ERROR InBitStream::Peek(UInt64 count)
{
	if (count < 0)
		return PARAM_INVALID;

	Int64 oldStreamPos = Tell();

	MP4ERROR result = MP4_OK;

	UInt8* temp = new UInt8[count];
	result = GetNByte(temp, count);
	delete[] temp;
	Seek(oldStreamPos);

	return result;
}

MP4ERROR InBitStream::Get2ByteOrder(void* dst)
{
	return GetNByte(dst, 2, true);
}

MP4ERROR InBitStream::Get4ByteOrder(void* dst)
{
	return GetNByte(dst, 4, true);
}

MP4ERROR InBitStream::Get8ByteOrder(void* dst)
{
	return GetNByte(dst, 8 , true);
}

MP4ERROR InBitStream::Get16ByteOrder(void* dst)
{
	return GetNByte(dst, 16, true);
}

MP4ERROR InBitStream::GetByte(void* dst)
{
	return GetNByte(dst, 1);
}

MP4ERROR InBitStream::Get2Byte(void* dst)
{
	return GetNByte(dst, 2);
}

MP4ERROR InBitStream::Get3Byte(void* dst)
{
	return GetNByte(dst, 3);
}

MP4ERROR InBitStream::Get4Byte(void* dst)
{
	return GetNByte(dst, 4);
}

MP4ERROR InBitStream::Get8Byte(void* dst)
{
	return GetNByte(dst, 8);
}

MP4ERROR InBitStream::Get16Byte(void* dst)
{
	return GetNByte(dst, 16);
}

MP4ERROR InBitStream::GetNByte(void* dst, Int32 count , bool byteOrder)
{
	if (NULL == dst)
		return NULL_PTR;

	if ((m_FileSize - Tell()) < count)
		return OUT_OF_FILE_BOUNDS;

	//if net byte order(big) or do not consider byte order
	if (m_isBigEndian || byteOrder == false)
	{
		m_FileStream.read((char*)dst, count);
	}
	else
	{
		UInt8* tempBuffer = new UInt8[count];

		m_FileStream.read((char*)tempBuffer, count);
		UInt64 re = m_FileStream.gcount();

		for (Int32 i = 0; i < count; ++i)
		{
			((UInt8*)dst)[count - i - 1] = tempBuffer[i];
		}
		delete[] tempBuffer;
	}


	return MP4_OK;
}

Int64 InBitStream::GetFileSize()
{
	return m_FileSize;
}

//////////////////OutBitStream
OutBitStream::OutBitStream()
:BitStream(L"info.txt", ios::out | ios::trunc)
{
}

OutBitStream::OutBitStream(wchar_t* filePath)
:BitStream(filePath, ios::out | ios::trunc)
{
}

OutBitStream::~OutBitStream()
{
}

void OutBitStream::WriteString(const char* string)
{
	m_FileStream.write(string, strlen(string));
}

void OutBitStream::WriteChar(char ch)
{
	m_FileStream.put(ch);
}

void OutBitStream::WriteUInt32Char2Stream(UInt32 UInt32Char)
{
	char* tempChar = reinterpret_cast<char*>(&UInt32Char);

	if (m_isBigEndian)
	{
		for (int i = 0; i < 4; ++i)
		{
			WriteChar(tempChar[i]);
		}
	}
	else
	{
		for (int i = 3; i >= 0; --i)
		{
			WriteChar(tempChar[i]);
		}
	}
}

void OutBitStream::WriteUIntNum2string(UInt64 UInt64Num)
{
	if (UInt64Num == 0)
	{
		WriteChar('0');
		return;
	}

	UInt8 sigleNum = 0;
	UInt64 tempUInt64Num = UInt64Num;
	string numString;
	while (tempUInt64Num)
	{
		sigleNum = tempUInt64Num % 10;
		tempUInt64Num = tempUInt64Num / 10;
		numString.insert(0, 1, (char)(sigleNum + '0'));
	}
	WriteString(numString.c_str());
}

void OutBitStream::WriteIntNum2string(Int64 Int64Num)
{
	UInt64 tempUInt64Num;
	if (Int64Num < 0)
	{
		WriteChar('-');
		tempUInt64Num = (-Int64Num);
	}
	else
		tempUInt64Num = Int64Num;

	WriteUIntNum2string(tempUInt64Num);
}

void OutBitStream::FlushStream()
{
	m_FileStream.flush();
}
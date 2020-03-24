#include<iostream>
using namespace std;
#define FRIST_POS_MINUS_ONE -1
#define NO_MATCH -1
/**************************K M P***************************************/
int KmpSearch(char* srcStr, char* patternStr ,int* next)
{
	int srcStrPos = 0;
	int patternStrPos = 0;
	int srcStrLen = strlen(srcStr);
	int patternStrLen = strlen(patternStr);
	while (srcStrPos < srcStrLen && patternStrPos < patternStrLen)
	{
		//重新从被匹配串头开始匹配 或者 上一对匹配成功继续匹配下一对字符
		if (patternStrPos == FRIST_POS_MINUS_ONE || srcStr[srcStrPos] == patternStr[patternStrPos])
		{
			++srcStrPos;
			++patternStrPos;
		}
		else
		{
			patternStrPos = next[patternStrPos];
		}			
	}

	if (patternStrPos == patternStrLen)
		return srcStrPos - patternStrPos;//返回数组中匹配位置
	else
		return NO_MATCH;//没找到字符串
}

void GetNext(char* patternStr, int* next)
{
	int patternStrLen = strlen(patternStr);
	next[0] = FRIST_POS_MINUS_ONE;
	//前缀位置
	int patternStrPrefixPos = FRIST_POS_MINUS_ONE;
	//后缀位置
	int patternStrSuffixPos = 0;
	while (patternStrSuffixPos < patternStrLen - 1)
	{
		//printf("pre:%d\tsuf:%d\n", patternStrPrefixPos, patternStrSuffixPos);
		//patternStr[patternStrPrefixPos]表示前缀，patternStr[patternStrSuffixPos]表示后缀
		if (patternStrPrefixPos == FRIST_POS_MINUS_ONE || patternStr[patternStrSuffixPos] == patternStr[patternStrPrefixPos])
		{
			++patternStrPrefixPos;
			++patternStrSuffixPos;
			next[patternStrSuffixPos] = patternStrPrefixPos;
			//printf("------pos:%d\tvalue:%d\n", patternStrSuffixPos, patternStrPrefixPos);
		}
		else
		{
			patternStrPrefixPos = next[patternStrPrefixPos];
		}
	}
}

void GetNextPro(char* patternStr, int* next)
{
	int patternStrLen = strlen(patternStr);
	next[0] = FRIST_POS_MINUS_ONE;
	//前缀位置
	int patternStrPrefixPos = -1;
	//后缀位置
	int patternStrSuffixPos = 0;
	while (patternStrSuffixPos < patternStrLen - 1)
	{
		//patternStr[patternStrPrefixPos]表示前缀，patternStr[patternStrSuffixPos]表示后缀
		if (patternStrPrefixPos == -1 || patternStr[patternStrSuffixPos] == patternStr[patternStrPrefixPos])
		{
			++patternStrPrefixPos;
			++patternStrSuffixPos;
			if (patternStr[patternStrPrefixPos] != patternStr[patternStrSuffixPos])
				next[patternStrSuffixPos] = patternStrPrefixPos;
			else
				next[patternStrSuffixPos] = next[patternStrPrefixPos];
		}
		else
		{
			patternStrPrefixPos = next[patternStrPrefixPos];
		}
	}
}

void TestKMP(char* srcStr, char* patternStr)
{
	int* next = new int[strlen(patternStr)];
	GetNext(patternStr, next);
	//GetNextPro(patternStr, next);
	int pos = KmpSearch(srcStr, patternStr, next);
	delete[] next;
	printf("pos:%d\n", pos);
}
/**************************K M P***************************************/

/**************************B M***************************************/
void GetRight(char* patternStr, int* right)
{
	for (int i = 0; i < 256; ++i)
	{
		right[i] = -1;
	}

	for (int patternStrPos = 0; patternStrPos < strlen(patternStr); ++patternStrPos)
	{
		right[patternStr[patternStrPos]] = patternStrPos;
	}
}

int BMSearch(char* srcStr, char* patternStr, int* right)
{
	int srcStrLen = strlen(srcStr);
	int patternStrLen = strlen(patternStr);
	int skip;
	for (int srcStrPos = 0; srcStrPos <= srcStrLen - patternStrLen; srcStrPos += skip)
	{
		skip = 0;
		for (int patternStrPos = patternStrLen - 1; patternStrPos >= 0; --patternStrPos)
		{
			if (patternStr[patternStrPos] != srcStr[srcStrPos + patternStrPos])
			{
				skip = patternStrPos - right[srcStr[srcStrPos + patternStrPos]];
				if (skip < 1)
				{
					skip = 1;
				}
				break;
			}
		}
		if (skip == 0)
			return srcStrPos;
	}
	return NO_MATCH;
}

void TestBM(char* srcStr, char* patternStr)
{
	int* right = new int[256];
	GetRight(patternStr, right);
	int pos = BMSearch(srcStr, patternStr, right);
	delete[] right;
	printf("pos:%d\n", pos);
}
/**************************B M***************************************/

/**************************S U N D A Y***************************************/
int GetIndex(char* patternStr, char c)
{
	for (int patternStrPos = strlen(patternStr) - 1; patternStrPos >= 0; patternStrPos--) {
		if (patternStr[patternStrPos] == c)
			return patternStrPos;
	}
	return -1;
}

int SundaySearch(char* srcStr, char* patternStr)
{
	int srcStrLen = strlen(srcStr);
	int patternStrLen = strlen(patternStr);
	int srcStrPos, patternStrPos;
	int skip = -1;
	for (srcStrPos = 0; srcStrPos <= srcStrLen - patternStrLen; srcStrPos += skip)
	{
		for (patternStrPos = 0; patternStrPos < patternStrLen; ++patternStrPos)
		{
			if (srcStr[srcStrPos + patternStrPos] != patternStr[patternStrPos])
			{
				if (srcStrPos == srcStrLen - patternStrLen)
					break;
				skip = patternStrLen - GetIndex(patternStr, srcStr[srcStrPos + patternStrLen]);
				break;
			}
		}
		if (patternStrPos == patternStrLen)
			return srcStrPos;
	}
	return NO_MATCH;
}

void TestSunday(char* srcStr, char* patternStr)
{
	int pos = SundaySearch(srcStr, patternStr);
	printf("pos:%d\n", pos);
}
/**************************S U N D A Y***************************************/

int main()
{
	char* patternStr = "acjdafjksjdglkjflsjdgew9gw";
	char* srcStr = "xabdsadsxabxacjdafjksjdglkjflsjdgew9gwe-g0ewjge";
	TestKMP(srcStr, patternStr);
	TestBM(srcStr, patternStr);
	TestSunday(srcStr, patternStr);
	return 0;
}
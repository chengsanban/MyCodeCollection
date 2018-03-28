/*
题目描述
计算字符串最后一个单词的长度，单词以空格隔开。 
输入描述:
一行字符串，非空，长度小于5000。

输出描述:
整数N，最后一个单词的长度。

示例1
输入
hello world
输出
5
*/

#include<iostream>
#include<string>
using namespace std;
int main()
{
	string s;
	while (getline(cin, s))
	{
		int index = s.size() - 1;
		while (s[index] == ' ')
			index--;

		int count = 0;
		for (; index >= 0; --index)
		{
			if (s[index] != ' ')
				++count;
			else
				break;
		}
		cout << count << endl;
	}
	return 0;
}
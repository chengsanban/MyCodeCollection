/*
题目描述
写出一个程序，接受一个十六进制的数值字符串，输出该数值的十进制字符串。（多组同时输入 ）

输入描述:
输入一个十六进制的数值字符串。

输出描述:
输出该数值的十进制字符串。

示例1
输入
0xA
输出
10
*/

#include<iostream>
#include<string>
#include<string.h>
using namespace std;

int main()
{
    string inputStr;
    while(cin>>inputStr)
    {
        int index = inputStr.length() - 1;
        int base = 1;
        int res = 0;
        while(inputStr[index] != 'x')
        {
            if(isdigit(inputStr[index]))
                res += base*(inputStr[index]-'0');
            else
                res += base*(inputStr[index]-'A'+10);
            base*=16;
            --index;
        }
        cout<<res<<endl;
    }
    return 0;
}
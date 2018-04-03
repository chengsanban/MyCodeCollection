/*
输入一个int型整数，按照从右向左的阅读顺序，返回一个不含重复数字的新的整数。

输入描述:
输入一个int型整数

输出描述:
按照从右向左的阅读顺序，返回一个不含重复数字的新的整数

示例1
输入
9876673
输出
37689
*/

#include<iostream>
using namespace std;
int main()
{
    int n;
    int a[10]={0};
    int num=0;
    cin>>n ;
    while(n)
    {
        if(a[n%10]==0)
        {
            a[n%10]++;//这一步是更新，遇到下次相同的数会跳过
            num=num*10+n%10;
        }
        n/=10;
    }
     
    cout<<num<<endl;
     
    return 0;
}
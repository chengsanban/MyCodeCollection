#include<iostream>
#include"heap.h"
using namespace std;

void Test1()
{
	int a[10] = {12,14,1,6,9,78,32,77,6,10};
	Heap<int, Big<int>> hp1(a, 10);
	hp1.Push(102);
	hp1.Print();
	hp1.Sort();
	hp1.Print();

	Heap<int> hp2(a, 10);
	hp2.Push(1);
	hp2.Print();
	hp2.Sort();
	hp2.Print();
}

int main(void)
{
	Test1();
	return 0;
}
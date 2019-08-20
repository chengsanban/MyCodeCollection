#include<iostream>
using namespace std;
int findFirstOne(int value);
bool testBit(int value, int pos);
int findNums(int date[], int length, int &num1, int &num2){
	if (length<2){ return -1; }
	int ansXor = 0;
	for (int i = 0; i<length; i++){
		ansXor ^= date[i];               //异或
	}
	int pos = findFirstOne(ansXor);
	num1 = num2 = 0;
	for (int i = 0; i<length; i++){
		if (testBit(date[i], pos))
			num1 ^= date[i];
		else
			num2 ^= date[i];
	}
	return 0;
}
int findFirstOne(int value){     //取二进制中首个为1的位置
	int pos = 1;
	while ((value & 1) != 1){
		value = value >> 1;
		pos++;
	}
	return pos;
}
bool testBit(int value, int pos){  //测试某位置是否为1
	return ((value >> pos) & 1);
}
int main(void){
	int date[10] = { 1, 2, 3, 4, 5, 6, 4, 3, 2, 1 };
	int ans1, ans2;
	if (findNums(date, 10, ans1, ans2) == 0)
		cout << ans1 << " " << ans2 << endl;
	else
		cout << "error" << endl;
	return 0;
}
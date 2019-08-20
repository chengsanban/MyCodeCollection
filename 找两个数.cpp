//一个整型数组里除了两个数字之外，其他的数字都出现了两次。请写程序找出这两个只出现一次的数字。 


//思路
//1、亦或可以使偶数个数抵消，就会剩下刚才指定的两数亦或值
//2、两数不同亦或的某位肯定是1，也就是两数位不同的地方，当然这样的位很多，找出一位标记
//3、按照是这一位把数组分组标记亦或就会剩下分别的数
class Solution {
public:
    void FindNumsAppearOnce(vector<int> data,int* num1,int *num2) {
        int len=data.size();
        if(len< 2) return ;
        int res=data[0]^data[1];
        for(int i=2;i<len;++i)
        {
           res^=data[i];   
        }
        
        int flag=1;
         while((res & flag) == 0) flag <<= 1;
        *num1 = res;
        *num2 = res;
        
        for(int i = 0; i < len; ++ i )
        {
            if((flag & data[i]) == 0) *num2 ^= data[i];
            else *num1 ^= data[i];
        }

        
    }
};
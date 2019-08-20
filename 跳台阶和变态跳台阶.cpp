//一只青蛙一次可以跳上1级台阶，也可以跳上2级。求该青蛙跳上一个n级的台阶总共有多少种跳法。 
class Solution { 

public: 

    int jumpFloor(int number) { 

        int a[number+1]; 

        int i; 

        a[1]=1; 

        a[2]=2; 

        for(i=3;i<=number;i++) 

        { 

            a[i]=a[i-1]+a[i-2]; 

        } 

        return a[number]; 

    } 

}; 
//一只青蛙一次可以跳上1级台阶，也可以跳上2级……它也可以跳上n级。求该青蛙跳上一个n级的台阶总共有多少种跳法。 
class Solution {
public:
    int jumpFloorII(int number) {
return  1<<--number;
    }
};
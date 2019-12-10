//给定一个double类型的浮点数base和int类型的整数exponent。求base的exponent次方。
class Solution {
public:
    double Power(double base, int exponent) {
    int p = abs(exponent);
      double r = 1.0;
        while(p){
            if(p & 1) r *= base;
            base *= base;
            p >>= 1;
        }
        return exponent < 0 ? 1/ r : r;

    }
};
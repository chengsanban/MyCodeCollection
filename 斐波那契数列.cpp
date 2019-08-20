
class Solution {
public:
   int Fibonacci(int n) {
        if(n == 0)
            return 0;
        if(n == 1)
            return 1;
        int numfn1 = 0, numfn2 = 1;
        int currentnum;
        for(int i=2; i<=n; ++i) {
            currentnum = numfn1+numfn2;
            numfn1 = numfn2;
            numfn2 = currentnum;
        }
        return currentnum;
    }

};
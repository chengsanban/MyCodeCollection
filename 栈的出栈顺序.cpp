//输入两个整数序列，第一个序列表示栈的压入顺序，请判断第二个序列是否为该栈的弹出顺序。
//假设压入栈的所有数字均不相等。例如序列1,2,3,4,5是某栈的压入顺序，序列4，5,3,2,1是该压栈序列对应的一个
//弹出序列，但4,3,5,1,2就不可能是该压栈序列的弹出序列。 
class Solution {
public:
    bool IsPopOrder(vector<int> pushV,vector<int> popV) {
        stack<int> st;
        int len1 = pushV.size();
        int len2 = popV.size();
        if(len1 != len2 || 0 == len1)
            return false;
        int j = 0;
        for(int i = 0;i < len1;++i) {
            st.push(pushV[i]);
            for(;j < len1;++j) {
                if(!st.empty() && popV[j] == st.top()) 
                    st.pop();
                else
                    break;
            }
        }
        return st.empty();
    }
};

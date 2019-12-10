//输入一个整数数组，判断该数组是不是某二叉搜索树的后序遍历的结果。如果是则输出Yes,否则输出No。假设输入的数组的任意两个数字都互不相同。

//因为后续遍历搜索二叉树，根一定在最后，所以数组前面部分一定小于根，后面一定大于根
//遍历数组当前面小于跟，后面全部大于根，继续划分区间递归遍历
class Solution {
public:
    bool JudgeBTS(vector<int> &sequence,int first,int end)
        {
        if(end<=first)
            return true;
        int i=first;
        for(;i<end;++i)   //判断是否前面一部分小于跟
            if(sequence[i]>sequence[end])
            break;
        for(int j=i+1;j<end;++j)//本来剩下的部分一定大于根，要有小于一定不是
            if(sequence[j]<sequence[end])
            return false;
       return JudgeBTS(sequence,first,i-1)&&JudgeBTS(sequence,i,end-1);
        //递归
    }
    bool VerifySquenceOfBST(vector<int> sequence) {
		if(sequence.empty())		//空的话返回不是
            return false;
        return JudgeBTS(sequence,0,sequence.size()-1);
    }
};
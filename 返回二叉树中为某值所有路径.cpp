//输入一颗二叉树和一个整数，打印出二叉树中结点值的和为输入整数的所有路径。
//路径定义为从树的根结点开始往下一直到叶结点所经过的结点形成一条路径。


//解释：通过递归把给定整数依次递减，直到碰到一个跟剩余数相同的节点值，且此节点是叶子
class Solution {
public:
    void GetPath(TreeNode* root,int expectNumber,vector<int> &path,vector<vector<int>> &Ret)
    {
        if(root==NULL||root->val>expectNumber)//空或者节点数已经大于预留数，pass
            return;
        path.push_back(root->val);
		//当前节点值和预留数相同且为叶子节点，加入这条路径，并且弹出当前值，返回上一层
        if(root->val==expectNumber&&root->right==NULL&&root->left==NULL)
        {
            Ret.push_back(path);
            path.pop_back();
        }
		//继续递归
        else
        {
        	GetPath(root->left,expectNumber-root->val,path,Ret);   
       	    GetPath(root->right,expectNumber-root->val,path,Ret);
            path.pop_back();
        }
    }
    
    vector<vector<int> > FindPath(TreeNode* root,int expectNumber) 
    {
		vector<vector<int>> Ret;
        vector<int> path;
        if(root==NULL)
            return Ret;
        GetPath(root,expectNumber,path,Ret);
        return Ret;
    }
};
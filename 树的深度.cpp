//求二叉树深度

//思路
//把没颗子树的左右子树最大值+1返回，递归问提
class Solution {
public:
    int TreeDepth(TreeNode* pRoot)
    {
     if(pRoot==NULL)
         return 0;
     int right=TreeDepth(pRoot->right);
     int left=TreeDepth(pRoot->left);
        
     return left>right?left+1:right+1;
    }
};
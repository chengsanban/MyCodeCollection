class Solution {
public:
    int run(TreeNode *root) {
        return _run(root);
    }
    
    int _run(TreeNode *root)
    {
    	if(root==NULL) 
            return 0;
        int left=_run(root->left);
        int right=_run(root->right);
        if(left&&right)
        	return left>right?right+1:left+1;
        else
            return left>right?left+1:right+1;
        
    }
};

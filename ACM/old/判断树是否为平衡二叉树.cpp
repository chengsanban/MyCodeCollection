class Solution {
public:
	bool IsBalanced_Solution(TreeNode* pRoot) {
		if (IsBalanced(pRoot) == -1)
			return false;
		return true;
	}

	int IsBalanced(TreeNode* root)
	{
		if (root == NULL)
			return 0;

	int left=IsBalanced(root->left);
	if (left == -1)
		return -1;
	int right=IsBalanced(root->right);
	if (right == -1)
		return -1;
        
        	if (abs(left - right) > 1)
		return -1;

	return left > right ? left + 1 : right + 1;
	}
};
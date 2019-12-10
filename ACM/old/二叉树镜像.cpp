class Solution {
public:
    void Mirror(TreeNode *pRoot) {
		_Mirror(pRoot);
	}

	void _Mirror(TreeNode *root)
	{
		if (root == NULL)
			return;
		swap(root->right,root->left);
		_Mirror(root->left);
		_Mirror(root->right);
	}
};
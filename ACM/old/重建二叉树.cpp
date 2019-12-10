//输入某二叉树的前序遍历和中序遍历的结果，请重建出该二叉树。假设输入的前序遍历和中序遍历的结果中都不含重复的//数字。例如输入前序遍历序列{1,2,4,7,3,5,6,8}和中序遍历序列{4,7,2,1,5,3,8,6}，则重建二叉树并返回。
 class Solution {
public:
    struct TreeNode* reConstructBinaryTree(vector<int> pre, vector<int> in) {    
         
        struct TreeNode *result = createBinaryTreeHelper(pre, in, 
                                                  0, pre.size() - 1, 0, in.size() - 1);
        return result;
    }
     
    struct TreeNode* createBinaryTreeHelper(vector<int> pre, vector<int> in, 
                                            int preLeft, int preRight, int inLeft, int inRight) {
        if(preLeft > preRight) return NULL;
         
        int rootValue = pre[preLeft];
        struct TreeNode *rootNode = new TreeNode(rootValue);
         
        int index;   // 根结点在中序序列中的位置
        for(int i = inLeft; i <= inRight; ++i) {
            if(in[i] == rootValue) {
                index = i;
                break;
            }
        }
        int lenLeft = index - inLeft;
        int lenRight = inRight - index;
         
        rootNode->left = createBinaryTreeHelper(pre, in, preLeft + 1, preLeft + lenLeft, 
                                                         inLeft, index - 1);
        rootNode->right = createBinaryTreeHelper(pre, in, preRight - lenRight + 1, preRight, 
                                                         index + 1, inRight);
         
        return rootNode;
    }
};
 
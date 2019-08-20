class Solution {
public:
    vector<int> PrintFromTopToBottom(TreeNode *rt) {
 queue<TreeNode*> q;
        q.push(rt);
        vector<int> v;
        while(!q.empty()){
            rt = q.front(); q.pop();
            if(!rt) continue;
            v.push_back(rt -> val);
            q.push(rt -> left);
            q.push(rt -> right);
        }
        return v;

    }
};
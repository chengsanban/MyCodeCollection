//ÄæÖÃÒ»¸öÁ´±í
class Solution {
public:
    vector<int> printListFromTailToHead(struct ListNode* head) {
               vector<int> v;
        while(head != NULL)
        {
            v.insert(v.begin(),head->val);
            head = head->next;
        }
        return v; 
    }
};
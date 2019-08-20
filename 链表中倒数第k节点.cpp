class Solution {
public:
    ListNode* FindKthToTail(ListNode* pListHead, unsigned int k) {
     ListNode* p=pListHead;
        unsigned int length=0;
        //求链表长度
        while(p){
            length++;
            p=p->next;
        }
        if(k>length)
            return NULL;
        unsigned int temp=1;
        p=pListHead;
        //求第n-k+1个结点
        while(p){
            if(temp==length-k+1)
                return p;
            else{
                temp++;
                p=p->next;
            }
        }
        return NULL;

    }
};
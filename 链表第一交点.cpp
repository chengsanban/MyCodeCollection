//求两个点链表的第一个交点

//解答
//1、扫描两个链表长度
//2、让长的链表先走链表长度差值步
//3、两个链表一起走遇到的第一个点就是所求
class Solution {
public:
    ListNode* FindFirstCommonNode( ListNode *pHead1, ListNode *pHead2) {
        if(pHead1==NULL||pHead2==NULL)
            return NULL;
        int len1=0;
        int len2=0;
        ListNode* new1=pHead1;
        ListNode* new2=pHead2;
        while(new1)
         {
         new1=new1->next;
         ++len1;
        }
        while(new2)
        {
         new2=new2->next;
         ++len2;   
        }
        int ans=abs(len1-len2);
        if(len1>len2)
        {
          	while(ans)
        	{
            	pHead1=pHead1->next;
                --ans;
        	}   
        }
        else
        {
             while(ans)
        	{
            	pHead2=pHead2->next;
                --ans;
        	}   
        }

        while(pHead1!=pHead2)
         {
            pHead1=pHead1->next;
            pHead2=pHead2->next;
        }
        
        return pHead1;
    }
};
//输入一个复杂链表（每个节点中有节点值，以及两个指针，一个指向下一个节点，另一个特殊指针指向任意一个节点）。 复制它

//解决：先在每个节点之间间隔插入节点，并赋值前节点值
//然后把前一个节点的复杂指针复制给下一个
//分离原链和本链
//具体可以参考博客http://www.cnblogs.com/daniagger/archive/2012/06/19/2555321.html
class Solution {
public:
    RandomListNode* Clone(RandomListNode* pHead)
    {
        if(pHead==NULL) return NULL;
 
        RandomListNode *newHead = new RandomListNode(pHead->label);
        RandomListNode *pHead1=NULL, *pHead2=NULL;
 
        // 上链，使新旧链表成之字形链接
        for(pHead1=pHead,pHead2=newHead;pHead1;){
            RandomListNode* tmp = pHead1->next;
            pHead1->next = pHead2;
            pHead2->next = tmp;
 
            // next
            pHead1 = tmp;
            if(tmp) pHead2 = new RandomListNode(tmp->label);
            else pHead2 = NULL;
        }
 
        // 更新新链表的random指针
        for(pHead1=pHead,pHead2=newHead;pHead1;){
            if(pHead1->random) pHead2->random = pHead1->random->next;
            else pHead2->random = NULL;
 
            pHead1 = pHead2->next;
            if(pHead1) pHead2 = pHead1->next;
            else pHead2 = NULL;
        }
 
        // 脱链，更新各链表的next指针
        for(pHead1=pHead,pHead2=newHead;pHead1;){
            pHead1->next = pHead2->next;
            if(pHead1->next) pHead2->next = pHead1->next->next;
            else pHead2->next = NULL;
 
            pHead1 = pHead1->next;
            pHead2 = pHead2->next;
        }
 
        return newHead;
    }
};

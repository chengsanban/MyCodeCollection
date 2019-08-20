//定义栈的数据结构，请在该类型中实现一个能够得到栈最小元素的min函数
//建立两个栈一个存数据，一个存最小值
#include<cassert>
class Solution {
public:
    stack<int> m_data, m_min;
    //push时候当push数据小于当前存最小数据的堆顶才存入
//这样当pop数据时，根据栈的原理只要没到栈顶的最小值，最小值一定栈顶元素	
    void push(int value) 
    {
        m_data.push(value);
        if(m_min.size() == 0 || m_min.top() >= value)
            m_min.push(value);
    }
    void pop() 
    {
        assert(m_data.size() >0 && m_min.size() > 0);
        if(m_data.top()==m_min.top())
             m_min.pop();
        m_data.pop();
    }
    int top() {
        assert(m_data.size() > 0 && m_min.size() > 0);
        return m_data.top();
    }
    int min() {
        assert(m_data.size() > 0 && m_min.size() > 0);
        return m_min.top();
    }
};

//题目描述
//输入一个链表，输出该链表中倒数第k个结点

//方法1：从前遍历，遇见偶数，删除尾插
//空间小，时间比较费
class Solution {
public:
    void reOrderArray(vector<int> &array) {
        vector<int>::iterator even = array.begin();
	int size = array.size();
	while (size)
	{
		if (*even % 2 == 0)
		{
			int tmp = *even;
			even = array.erase(even);
			array.push_back(tmp);
		}
		else
			even++;
		size--;
	}
    }
};
//方法2：偶数记录下来，放在后面
//时间O（n），空间O(n)
void Sloution(vector<int> &array)
{
	vector<int> v;
	vector<int>::iterator evenPtr = array.begin();
	vector<int>::iterator ptr = evenPtr;
	while (ptr!=array.end())
	{
		if (*ptr % 2 == 0)
			v.push_back(*ptr);
		else
		{
			*evenPtr = *ptr;
			evenPtr++;
		}
		ptr++;
	}
	vector<int>::iterator vPtr = v.begin();
	while (evenPtr != array.end())
	{
		*evenPtr = *vPtr;
		evenPtr++;
		vPtr++;
	}
}
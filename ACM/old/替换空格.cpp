//请实现一个函数，将一个字符串中的空格替换成“%20”。例如，当字符串为We Are Happy.则经过替换之后的字符串为We%20Are%20Happy。
class Solution {
public:
	void replaceSpace(char *str,int length) {
	char* cur = str;
		char* tail = str + length;
		while (cur!=tail)
		{
			if (*cur == ' ')
			{
				while (tail != cur)
				{
					*(tail + 2) = *tail;
					tail--;
				}
				*cur++ = '%';
				*cur++ = '2';
				*cur = '0';
				tail = str + length + 2;
			}
			cur++;
		}
	}
};
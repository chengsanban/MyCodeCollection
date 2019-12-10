//题目：求一串字符串中第一个出现一次的字符


//解答一
//一次遍历解决问题
//用两个大小为26的hash表（或者一个大小为256的hash表也可以），字符为hash表的键值
//值为对应的字符出现次数
//一个vector<pair<char,int> >来存放该char第一次出现的位置
//遍历每个字符时，判断hash表中字符是否为0，为零则该字符第一次出现，将该字符和
//其位置push进vector中。若不为0，者该键值对应的值加一
//遍历完之后，看遍历vector，以pair<char,int>中的char为键值在hash中查看其值是否为1，若
//是1，则返回pair<char,int> 中的int即为第一个只出现一次字符的出现位置


//解答二
//还是使用哈希表，第一遍统计个数，第二遍从前遍历，找到第一个字符数量为1的几位所得
 
class Solution {
public:
    int FirstNotRepeatingChar(string str) {
        if(str.empty()) return -1;
        int countsA[26] = {0};
        int countsB[26] = {0};
        vector<pair<char,int> > comeoutOrder;
        pair<char,int> pairIt;
        for(int i = 0;i<str.size();i++){
            if(str[i] >= 'A' && str[i] <= 'Z'){
                if(countsA[str[i]-'A'] == 0){
                    countsA[str[i]-'A']++;
                    pairIt.first = str[i];
                    pairIt.second = i;
                    comeoutOrder.push_back(pairIt);
                }
                else countsA[str[i]-'A']++;
            }
            else if(str[i] >= 'a' && str[i] <= 'z'){
                if(countsB[str[i]-'a'] == 0){
                    countsB[str[i]-'a']++;
                    pairIt.first = str[i];
                    pairIt.second = i;
                    comeoutOrder.push_back(pairIt);
                }
                else countsB[str[i]-'a']++;
            }
            else return -1;
        }
        for(int i = 0;i<comeoutOrder.size();i++){
            if(comeoutOrder[i].first >= 'A' && comeoutOrder[i].first <= 'Z')
                {
                if(countsA[comeoutOrder[i].first-'A'] == 1) return comeoutOrder[i].second;
            }
            else {
                if(countsB[comeoutOrder[i].first-'a'] == 1) return comeoutOrder[i].second;
            }
        }
         
        return -1;
    }
};

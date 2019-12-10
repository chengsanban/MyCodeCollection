//在一个二维数组中，每一行都按照从左到右递增的顺序排序，每一列都按照从上到下递增的顺序排序。请完成一个函数，输入这样的一个二维数组和一个整数，判断数组中是否含有该整数。 

class Solution {
public:
    bool Find(vector<vector<int> > array,int target) {
              int sizeX = array.size();
        int sizeY = array[0].size();
        int i = sizeX-1;
        int j = 0;
        while (i >= 0 && j < sizeY) {
            if (target == array[i][j])
             return true;
            else if (target < array[i][j])
             i--;              
            else
                 j++;
        }
        return false;
    }
};
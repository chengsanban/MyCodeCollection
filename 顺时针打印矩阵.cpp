class Solution {
public:
    vector<int> printMatrix(vector<vector<int> > matrix) {
	int rowUp = 0;
	int rowDown = matrix.size() - 1;
	int colUp = 0;
	int colDown = matrix[0].size() - 1;
	vector<int> v;
	while (rowUp <= rowDown || colUp <= colDown)
	{
			for (int i = colUp; i <= colDown; i++)
			{
				v.push_back(matrix[rowUp][i]);
			}
			rowUp++;
	
			for (int i = rowUp; i <= rowDown; i++)
			{
				v.push_back(matrix[i][colDown]);
			}
			colDown--;

			for (int i = colDown; i >= colUp; i--)
			{
				v.push_back(matrix[rowDown][i]);
			}
			rowDown--;
	
			for (int i = rowDown; i >= rowUp; i--)
			{
				v.push_back(matrix[i][colUp]);
			}
			colUp++;

	}

	return v;
}
};
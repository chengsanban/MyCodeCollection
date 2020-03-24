#pragma once
#include<vector>


//small heap
template<typename T>
struct Small
{
	bool operator()(const T& left, const T& right)
	{
		return left < right;  //operator <
	}
};

//big heap
template<typename T>
struct Big
{
	bool operator()(const T& left, const T& right)
	{
		return left > right; //operator >
	}
};


template<typename T,typename Compare = Small<T>>
class Heap
{
public:
	Heap()
	{}

	Heap(const T* arrayHead, size_t arraySize)
	{
		//init _arrays
		for (size_t i = 0; i < arraySize; ++i)
		{
			_arrays.push_back(arrayHead[i]);
		}

		//create heap
		//The last leaf node of the father node si n/2 in Complete binary tree
		for (int i = (_arrays.size() - 2) / 2; i >= 0; --i)
		{
			AdjustDown(i,_arrays.size());
		}
	}

	~Heap()
	{}

	void Push(const T& x)
	{
		_arrays.push_back(x);
		AdjustUp(_arrays.size()-1);
	}

	void Pop()
	{
		assert(_arrays.size() > 0);
		swap(_arrays[0], _arrays[_arrays.size() - 1]);
		_arrays.pop_back();

		AdjustDown(0);
	}

	T& Top()
	{
		assert(_arrays.size() > 0);
		return _arrays[0];
	}

	bool Empty()
	{
		return _arrays.empty();
	}

	int Size()
	{
		return _arrays.size();
	}

	void Print()
	{
		for (size_t i = 0; i < _arrays.size(); ++i)
		{
			cout << _arrays[i] << " ";
		}
		cout << endl;
	}

	void Sort()
	{
		for (size_t i = 0; i < _arrays.size(); ++i)
		{
			swap(_arrays[0],_arrays[_arrays.size()-i-1]);
			AdjustDown(0,_arrays.size() - i - 1);
		}
	}
private:
	//
	void AdjustDown(int root,int ajustSize)
	{
		int child = root * 2 + 1;

		Compare com;
		while (child < ajustSize)
		{
			//The last root node `s child node maybe just one
			if (child + 1 < ajustSize && com(_arrays[child + 1], _arrays[child]))
			{
				++child;
			}

			if (com(_arrays[child],_arrays[root]))
			{
				swap(_arrays[child],_arrays[root]);
				root = child;
				child = 2 * root + 1;
			}
			else
			{
				break;
			}
		}
	}

	void AdjustUp(int child)
	{
		int parent = (child - 1) / 2;

		while (child > 0)
		{
			if (Compare()(_arrays[child], _arrays[parent]))
			{
				swap(_arrays[parent], _arrays[child]);
				child = parent;
				parent = (child - 1) / 2;
			}
			else
			{
				break;
			}
		}
	}
private:
	std::vector<T> _arrays;
};


//PriorityQueue
template<class T>
class PriorityQueue
{
public:
	void Push(const T& x)
	{
		_hp.Push(x);
	}

	void Pop()
	{
		_hp.Pop();
	}
private:
	Heap<T> _hp;
};
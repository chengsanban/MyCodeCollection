#include <algorithm>


//BubbleSortPro add Flag for order data group
void BubbleSortBase(int* arrayHead,int arraySize)
{
	for (int i = 0; i < arraySize; ++i)
	{
		for (int j = 0; j < arraySize - i-1; ++j)
		{
			if (arrayHead[j]>arrayHead[j + 1])
			{
				std::swap(arrayHead[j], arrayHead[j + 1]);
			}
		}
	}
}


void SelectSort(int* arrayHead, int arraySize)
{
	for (int i = 0; i < arraySize; ++i)
	{
		for (int j = i+1; j < arraySize; ++j)
		{
			if (arrayHead[i]<arrayHead[j])
			{
				std::swap(arrayHead[i],arrayHead[j]);
			}
		}
	}
}

void InsertSort(int* arrayHead, int arraySize)
{
	for (int i = 1; i < arraySize; ++i)
	{
		//InsertSort optimization
		//if (arrayHead[i] < arrayHead[i - 1])
		//{
			int temp = arrayHead[i];
			int j;
			for (j = i - 1; j >= 0 && arrayHead[j]>temp; j--)
			{
				arrayHead[j + 1] = arrayHead[j];
			}
			arrayHead[j + 1] = temp;
		//}
	}
}

void ShellSort(int* arrayHead, int arraySize)
{
	int increment = arraySize;//º‰∏Ù‘ˆ¡ø
	do
	{
		increment = increment / 3 + 1;
		for (int i = 1; i<arraySize; i += increment)
		{
			int temp = arrayHead[i];
			int j;
			for (j = i - increment; j >= 0 && arrayHead[j]>temp; j -= increment)
			{
				arrayHead[j + increment] = arrayHead[j];
			}
			arrayHead[j + increment] = temp;
		}

	} while (increment > 1);
}


//-------------------------merge------------------------------------------
#define MERGESORT_ARRAY_SIZE 10

void _Merge(int* sourceArr, int* tempArr, int startIndex, int midIndex, int endIndex)
{
	int frontArrayIndex = startIndex, backArrayIndex = midIndex+1, tempIndex = startIndex;
	while (frontArrayIndex < midIndex+1 && backArrayIndex < endIndex + 1)
	{
		if (sourceArr[frontArrayIndex] > sourceArr[backArrayIndex])
			tempArr[tempIndex++] = sourceArr[frontArrayIndex++];
		else
			tempArr[tempIndex++] = sourceArr[backArrayIndex++];
	}

	while (frontArrayIndex < midIndex + 1)
		tempArr[tempIndex++] = sourceArr[frontArrayIndex++];

	while (backArrayIndex < endIndex + 1)
		tempArr[tempIndex++] = sourceArr[backArrayIndex++];

	for (frontArrayIndex = startIndex; frontArrayIndex <= endIndex; frontArrayIndex++)
		sourceArr[frontArrayIndex] = tempArr[frontArrayIndex];
}

void _MSort(int* sourceArr, int* tempArr, int startIndex, int endIndex)
{
	int midIndex;
	if (startIndex < endIndex)
	{
		midIndex = (startIndex + endIndex) / 2;
		_MSort(sourceArr, tempArr, startIndex, midIndex);
		_MSort(sourceArr, tempArr, midIndex + 1, endIndex);
		_Merge(sourceArr, tempArr, startIndex, midIndex, endIndex);
	}

}



void MergeSort(int * arrayHead, int arraySize)
{
	int tempArray[MERGESORT_ARRAY_SIZE] = {0};
	_MSort(arrayHead, tempArray, 0,arraySize-1);
}

//-------------------------merge------------------------------------------

//-------------------------QuickSort------------------------------------------
void _QSort(int* arrayHead, int left, int right)
{
	if (left > right)
		return;

	int leftIndex = left;
	int rightIndex = right;
	int key = arrayHead[left];

	while (leftIndex < rightIndex)
	{
		while (leftIndex < rightIndex && key <= arrayHead[rightIndex])
		{
			rightIndex--;
		}

		arrayHead[leftIndex] = arrayHead[rightIndex];

		while (leftIndex < rightIndex && key >= arrayHead[leftIndex])
		{
			leftIndex++;
		}

		arrayHead[rightIndex] = arrayHead[leftIndex];
	}
	arrayHead[leftIndex] = key;
	_QSort(arrayHead,left,leftIndex-1);
	_QSort(arrayHead, leftIndex + 1, right);
}


void QuickSort(int* arrayHead, int arraySize)
{
	_QSort(arrayHead,0,arraySize-1);
}
//-------------------------QuickSort------------------------------------------

#define COUNTING_NUM 100
void CountingSort(int* arrayHead, int arraySize)
{
	int countArray[COUNTING_NUM] = { 0 };
	for (int i = 0; i < arraySize; ++i)
	{
		countArray[arrayHead[i]]++;
	}

	for (int i = 0,j=0; i < COUNTING_NUM; ++i)
	{
		while (countArray[i] > 0)
		{
			countArray[i]--;
			arrayHead[j++] = i;
		}

		if (j > arraySize)
			break;
	}
}

void RadixSort(int* arrayHead, int arraySize)
{
	int* tempArray = new int[arraySize];
	int* countArray = new int[10];
	int radix = 1;
	//for sample,use MAX_RADIX instead of calculate maxbit
	const int MAX_RADIX = 1000;

	while (radix <= MAX_RADIX)
	{
		//clear count
		for (int i = 0; i < 10; ++i)
		{
			countArray[i] = 0;
		}

		//counting
		for (int i = 0; i < 10; ++i)
		{
			countArray[arrayHead[i] / radix %10] ++;
		}

		for (int i = 1; i < 10; ++i)
		{
			countArray[i] += countArray[i - 1];
		}

		for (int i = arraySize - 1; i >= 0; i--)
		{
			int tempRadix = arrayHead[i] / radix % 10;
			tempArray[countArray[tempRadix] - 1] = arrayHead[i];
			countArray[tempRadix]--;
		}

		for (int i = 0; i < arraySize; ++i)
		{
			arrayHead[i] = tempArray[i];
		}
		radix *= 10;
	}
	delete[] tempArray;
	delete[] countArray;
}
#include <iostream>
#include <fstream>
#include "Item.h"

using namespace std;

int main() {
	Item items[10];

	ifstream infile("JBB.txt");

	int carryWeight = 0;
	infile >> carryWeight;

	int itemCount = 0;
	while (!infile.eof()) {
		infile >> items[itemCount];
		itemCount++;
	}
	infile.close();


	int itemtotal = 0;

	for (int i = 0; i < itemCount; i++)
	{
		itemtotal += items[i].Quantity;
	}

	int** optTable = new int* [itemtotal + 1];

	for (int i = 0; i < itemtotal + 1; i++)
	{
		optTable[i] = new int[carryWeight + 1];
	}

	int* itemindextable = new int [itemtotal + 1];
	int iterator = 1;
	for (int i = 0; i < itemCount; i++)
	{
		int counter = iterator;
		for (int j = iterator; j < (iterator + items[i].Quantity); j++)
		{
			itemindextable[j] = i;
			counter++;
		}
		iterator = counter;
	}
	
	for (int i = 0; i < carryWeight + 1; i++)
	{
		optTable[0][i] = 0;
	}

	for (int i = 1; i <= itemtotal; i++)
	{
		for (int j = 0; j <= carryWeight; j++)
		{
			if (j - items[itemindextable[i]].Weight >= 0)
			{
				// we have to compare
				int PreviousBest = optTable[i - 1][j];
				int NewItemIncluded = optTable[i - 1][j - items[itemindextable[i]].Weight] + items[itemindextable[i]].Value;

				if (PreviousBest > NewItemIncluded)
				{
					optTable[i][j] = PreviousBest;
				}
				else
				{
					optTable[i][j] = NewItemIncluded;
				}
			}
			else {
				// we can't include the new item yet
				optTable[i][j] = optTable[i - 1][j];
			}
		}
	}

	int currentRow = itemtotal;
	int currentCol = carryWeight;

	while (currentRow > 0 && currentCol > 0)
	{
		if (optTable[currentRow][currentCol] == optTable[currentRow-1][currentCol])
		{
			currentRow--;
		}
		else
		{
			currentCol -= items[itemindextable[currentRow]].Weight;
			items[itemindextable[currentRow]].Taken++;
			currentRow--;
		}
	}

	int TotalValue = 0;

	for (int i = 0; i < itemCount; i++)
	{
		if (items[i].Taken > 0)
		{
			cout << items[i].Taken << " " << items[i].Name <<" ";
			TotalValue += items[i].Value * items[i].Taken;
		}		
	}
	cout << endl;
	cout << "Total Value: " << TotalValue << endl;

	for (int i = 0; i < itemtotal + 1; i++)
	{
		delete[] optTable[i];
	}

	delete[] optTable;
	delete[] itemindextable;
	

}
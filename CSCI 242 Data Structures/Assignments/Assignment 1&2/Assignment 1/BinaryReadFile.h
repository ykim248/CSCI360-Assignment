#pragma once
// BinaryFileRead.cpp : Defines the entry point for the console application.
//

#include <iostream>

using namespace std;

int BinaryRead()
{
	int* Array;
	char fileName[50];
	int size, readVal;

	cout << "Enter a filename to sort => ";
	cin >> fileName;

	FILE* inFile;

	fopen_s(&inFile, fileName, "rb");
	fread(&size, sizeof(size), 1, inFile);
	Array = new int[size];

	for (int i = 0; i < size; i++) {
		fread(&readVal, sizeof(readVal), 1, inFile);
		Array[i] = readVal;
	}

	fclose(inFile);
	return 0;
}


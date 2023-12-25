#include <iostream>
#include <math.h>
#include "Timing.h"

using namespace std;

void Combine(int* Source, int* Dest, int left, int mid, int right)
{
    int list1 = left;
    int list2 = mid + 1;
    int DestList = left;

    while (list1 <= mid && list2 <= right)
    {
        if (Source[list1] < Source[list2])
        {
            Dest[DestList] = Source[list1];
            list1++;
        }
        else
        {
            Dest[DestList] = Source[list2];
            list2++;
        }
        DestList++;
    }
    if (list1 > mid)
    {
        while (list2 <= right)
        {
            Dest[DestList] = Source[list2];
            DestList++;
            list2++;
        }
    }
    else
    {
        while (list1 <= mid)
        {
            Dest[DestList] = Source[list1];
            DestList++;
            list1++;
        }
    }
}


void MergeSort(int *Source, int * Dest, int left, int right)
{    
    if (left < right)
    {
        int mid = (left + right) / 2;
        MergeSort(Dest, Source, left, mid);

        MergeSort(Dest, Source, (mid + 1), right);
       
        Combine(Source, Dest, left, mid, right);
    }    
}

int Partition(int* array, int Low, int High)
{
    int mid = (Low + High) / 2;
    int Pivot = array[mid];
    while (Low != High)
    {
        while (Low != High)
        {
            if (array[High] < Pivot)
            {
                array[Low] = array[High];
                Low++;
                break;
            }
            else
            {
                High--;
            }
        }
        while (Low != High)
        {
            if (array[Low] > Pivot)
            {
                array[High] = array[Low];
                High--;
                break;
            }
            else
            {
                Low++;
            }
        }
        array[Low] = Pivot;
        return Low;
    }
}

void QuickSort(int* array, int Low, int High)
{
    int mid = (Low + High) / 2;
    int Pivot = array[mid];
    int Pi = Partition(array, Low, High);
    if (Low < (Pi-1))
    {
        QuickSort(array, Low, (Pi - 1));
    }
    if (Pi + 2 < High)
    {
        QuickSort(array, (Pi - 2), High);
    }
}







int main()
{
    cout << "Please select a file: " << endl;

    cout << "1) 10numbers" << endl;
    cout << "2) 100numbers" << endl;
    cout << "3) 1000000 Almost Sorted" << endl;
    cout << "4) 1000000 Random" << endl;
    cout << "5) 10000000 Random" << endl;

    int Choice;
    cout << "Selection -> ";
    cin >> Choice;

    char FileName[50];

    switch (Choice)
    {
    case 1:
        strcpy_s(FileName, "10numbers.bin");
        break;
    case 2:
        strcpy_s(FileName, "100numbers.bin");
        break;
    case 3:
        strcpy_s(FileName, "1000000almostsorted.bin");
        break;
    case 4:
        strcpy_s(FileName, "1000000numbers.bin");
        break;
    case 5:
        strcpy_s(FileName, "10000000numbers.bin");
        break;
    }

    int* Array;
    int size, readVal;

    FILE* inFile;

    fopen_s(&inFile, FileName, "rb");
    fread(&size, sizeof(size), 1, inFile);
    Array = new int[size];

    for (int i = 0; i < size; i++) {
        fread(&readVal, sizeof(readVal), 1, inFile);
        Array[i] = readVal;
    }

    fclose(inFile);

    /* for (int i = 1; i < size; i++)
     {
         cout << Array[i] << endl;
     }*/


    cout << "Please select a sort to use: " << endl;

    cout << "1) Bubble Sort" << endl;
    cout << "2) Selection Sort" << endl;
    cout << "3) Insertion Sort" << endl;
    cout << "4) Shell Sort" << endl;
    cout << "5) Merge Sort" << endl;
    cout << "6) Quick Sort" << endl;

    int Selection;
    cout << "Selection -> ";
    cin >> Selection;

    bool ok = true;

    int LowerBound, HigherBound;

    switch (Selection)
    {
    case 1:
        cout << "Start: ";
        Timing();

        //counts down
          //for i-size to 0
        for (int i = (size - 1); i > 0; i--)
        {
            //for j=0 to i
            for (int j = 0; j < i; j++)
            {
                int temp;
                if (Array[j] > Array[j + 1])
                {
                    temp = Array[j];
                    Array[j] = Array[j + 1];
                    Array[j + 1] = temp;
                }
            }
        }


        cout << " End: ";
        Timing();


        cout << "Enter Lower Bound Number: ";
        cin >> LowerBound;
        cout << "Enter Higher Bound Number: ";
        cin >> HigherBound;

        for (int i = LowerBound; i <= HigherBound; i++)
        {
            cout << Array[i] << " ";
        }

        cout << endl;


        break;


    case 2:
        cout << "Start: ";
        Timing();

        for (int i = (size - 1); i > 0; i--)
        {
            int index = 0;
            for (int j = 1; j <= i; j++)
            {
                if (Array[index] < Array[j])
                {
                    index = j;
                }

            }
            swap(Array[index], Array[i]);
        }

        cout << " End: ";
        Timing();


        cout << "Enter Lower Bound Number: ";
        cin >> LowerBound;
        cout << "Enter Higher Bound Number: ";
        cin >> HigherBound;

        for (int i = LowerBound; i <= HigherBound; i++)
        {
            cout << Array[i] << " ";
        }
        cout << endl;

        break;

    case 3:
        cout << "Start: ";
        Timing();


        for (int i = 2; i < size; i++)
        {
            Array[0] = Array[i];
            int head = i - 1;
            while (Array[0] < Array[head])
            {
                Array[head + 1] = Array[head];
                head--;
            }
            Array[head + 1] = Array[0];

        }
        cout << endl;

        cout << "End: ";
        Timing();



        cout << "Enter Lower Bound Number: ";
        cin >> LowerBound;
        cout << "Enter Higher Bound Number: ";
        cin >> HigherBound;

        for (int i = LowerBound; i <= HigherBound; i++)
        {
            cout << Array[i] << " ";
        }

        cout << endl;

        break;

    case 4:
        cout << "Starting: ";
        Timing();

        int NumShells;
        int head;
        int RightEdge;

        NumShells = sqrt(size);

        


        while (NumShells >= 1)
        {
            for (head = (NumShells + 1); head < size; head++)
            {
                Array[0] = Array[head];
                RightEdge = head - NumShells;
                while (RightEdge > 0 && Array[0] < Array[RightEdge])
                {
                    Array[RightEdge + NumShells] = Array[RightEdge];
                    RightEdge = RightEdge - NumShells;
                }
                Array[RightEdge + NumShells] = Array[0];
            }
            if (NumShells == 1) break;
            NumShells = NumShells / 3;
            if (NumShells == 0)
            {
                NumShells = 1;
            }
        }

        cout << " End: ";
        Timing();


        cout << "Enter Lower Bound Number: ";
        cin >> LowerBound;
        cout << "Enter Higher Bound Number: ";
        cin >> HigherBound;

        for (int i = LowerBound; i <= HigherBound; i++)
        {
            cout << Array[i] << " ";
        }

        cout << endl;

        break;

    case 5:
        cout << "Starting: ";
        Timing();

        int start = 0;
        int* TempArr = new int[size];
        for (int i = 0; i < size; i++)
        {
            TempArr[i] = Array[i];
        }

        MergeSort(TempArr, Array, start, size);

        delete[] TempArr;


        cout << " End: ";
        Timing();

        cout << "Enter Lower Bound Number: ";
        cin >> LowerBound;
        cout << "Enter Higher Bound Number: ";
        cin >> HigherBound;

        for (int i = LowerBound; i <= HigherBound; i++)
        {
            cout << Array[i] << " ";
        }

        cout << endl;

        break;

    case 6:
        cout << "Starting: ";
        Timing();

        int Low = 0;

        QuickSort(Array, Low, size);

        cout << "Starting: ";
        Timing();
    }
}



#include <iostream>
#include <math.h>
#include "Timing.h"

using namespace std;

struct Node
{
    int value;
    Node* next = NULL;
};

struct Bins
{
    Node* Front = NULL;
    Node* Back = NULL;
};



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
    Array = new int[size + 1];

    for (int i = 1; i <= size; i++) {
        fread(&readVal, sizeof(readVal), 1, inFile);
        Array[i] = readVal;
    }

    fclose(inFile);

    cout << "Please select a sort to use: " << endl;

    cout << "1) Heap Sort" << endl;
    cout << "2) Bin/Radix Sort" << endl;

    int Selection;
    cout << "Selection -> ";
    cin >> Selection;

    bool ok = true;
    int LowerBound, HigherBound;
    int eol = size;
    int temp;
    int Cidx;
    int Pidx;

    switch (Selection)
    {
    case 1:

        cout << "Starting: ";
        Timing();

        for (int i = 2; i < eol; i++)
        {
            Cidx = i;

            while (Cidx > 1)
            {
                Pidx = Cidx / 2;

                if (Array[Cidx] > Array[Pidx])
                {
                    Array[0] = Array[Cidx];
                    Array[Cidx] = Array[Pidx];
                    Array[Pidx] = Array[0];
                }
                Cidx = Pidx;
            }
        }

        while (eol > 2)
        {
            Array[0] = Array[1];
            Array[1] = Array[eol];
            Array[eol] = Array[0];

            eol--;
            Pidx = 1;

            int LC;
            int RC;
            int BC = 0;

            while (Pidx <= eol)
            {


                LC = Pidx * 2;
                RC = (Pidx * 2) + 1;

                if (LC < eol && RC <= eol) //what if the right child is equal to the end of the list?
                {
                    if (Array[LC] > Array[RC])
                    {
                        BC = LC;
                    }
                    else
                    {
                        BC = RC;
                    }
                }
                else if (LC <= eol)
                {
                    if (Array[Pidx] < Array[eol])
                    {
                        Array[0] = Array[Pidx];
                        Array[Pidx] = Array[eol];
                        Array[Pidx] = Array[0];
                    }
                }
                //else if the left child is part of the list?


                if (Array[BC] > Array[Pidx])
                {
                    Array[0] = Array[BC];
                    Array[BC] = Array[Pidx];
                    Array[Pidx] = Array[0];
                }// if best child is not greater than. quit.
                else
                {
                    break;
                }

                Pidx = BC;
            }
        }


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



    case 2:

        cout << "Starting: ";
        Timing();

        int destidx = 1;
        Node* NodeArray = new Node[size+1];
        Bins bins[10];
        int digit = 0;

        int largest = -1;
        int Width = 0;

        for (int i = 1; i < size; i++)
        {
            NodeArray[i].value = Array[i];
            if (largest < NodeArray[i].value)
            {
                largest = NodeArray[i].value;
            }
        }

        while (largest > 0)
        {
            largest /= 10;
            Width++;
        }

        for (int i = 1; i <= Width; i++)
        {
            for (int CurNum = 1; CurNum < size + 1; CurNum++)
            {
                int power = pow(10, digit);
                int binval = (Array[CurNum] / power) % 10;
                NodeArray[CurNum].value = Array[CurNum];

                if (bins[binval].Front != NULL)
                {
                    bins[binval].Back->next = &NodeArray[CurNum];
                }
                else
                {
                    bins[binval].Front = &NodeArray[CurNum];
                }

                bins[binval].Back = &NodeArray[CurNum];
            }

            int destidx = 1;
            for (int Binidx = 0; Binidx <= 9; Binidx++)
            {
                while (bins[Binidx].Front != NULL)
                {
                    Array[destidx] = bins[Binidx].Front->value;
                    Node* Save = bins[Binidx].Front;
                    bins[Binidx].Front = bins[Binidx].Front->next;
                    Save->next = NULL;
                    destidx++;
                }
            }
            digit++;
        }

        delete[] NodeArray;


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
    }



}



#pragma once

#include <iostream>
#include "CursorControl.h"
using namespace std;

struct Node
{
	int val;
	Node* left = NULL;
	Node* right = NULL;
	int bf = 0;
};

class AVLtree
{
private:
	Node* Root = NULL;
	
	bool done = false;
	void recAdd(Node*& Travis, int num)
	{
		
		if (!Travis)
		{
			Travis = new Node;
			Travis->val = num;
		}
		else if (Travis->val < num)
		{
			recAdd(Travis->right, num);
			if (!done)
			{
				Travis->bf++;
				if (Travis->bf == 0)
				{
					done = true;
				}
				else if (Travis->bf == 2)
				{
					if (Travis->right->bf == 1)
					{
						RotateLeft(Travis);
						done = true;
					}
					else if (Travis->right->bf == -1)
					{
						DoubleRotateLeft(Travis);
						done = true;
					}
				}
			}
		}
		else if (Travis->val > num)
		{
			recAdd(Travis->left, num);
			if (!done)
			{
				Travis->bf--;
				if (Travis->bf == 0)
				{
					done = true;
				}
				else if (Travis->bf == -2)
				{
					if (Travis->left->bf == -1)
					{
						RotateRight(Travis);
						done = true;
					}
					else if (Travis->left->bf == 1)
					{
						DoubleRotateRight(Travis);
						done = true;
					}
				}
			}
		}
	}

	void RotateLeft(Node*& Travis)
	{
		Node* Temp = Travis->right->left;
		Travis->right->left = Travis;
		Travis = Travis->right;
		Travis->left->right = Temp;

		Travis->bf = 0;
		Travis->left->bf = 0;
	}

	void RotateRight(Node*& Travis)
	{
		Node* Temp = Travis->left->right;
		Travis->left->right = Travis;
		Travis = Travis->left;
		Travis->right->left = Temp;

		Travis->bf = 0;
		Travis->right->bf = 0;
	}

	void DoubleRotateLeft(Node*& Travis)
	{
		Node* Temp = Travis;
		Travis = Temp->right->left;
		Temp->right->left = Travis->right;
		Travis->right = Temp -> right;
		Temp->right = Travis->left;
		Travis->left = Temp;

		Travis->bf = 0;
		Travis->left->bf = 0;
		Travis->right->bf = 0;
	}

	void DoubleRotateRight(Node*& Travis)
	{
		Node* Temp = Travis;
		Travis = Temp->left->right;
		Temp->left->right = Travis->left;
		Travis->left = Temp->left;
		Temp->left = Travis->right;
		Travis->right = Temp;

		Travis->bf = 0;
		Travis->left->bf = 0;
		Travis->right->bf = 0;
	}

	void recClear(Node*& travel)
	{
		if (travel->left)
		{
			recClear(travel->left);
		}

		if (travel->right)
		{
			recClear(travel->right);
		}

		delete travel;
		travel = NULL;
	}

	void recinorder(Node*& temp)
	{
		if (temp->left)
		{
			recinorder(temp->left);
		}

		cout << temp->val << " ";


		if (temp->right)
		{
			recinorder(temp->right);
		}
	}

	void RecShowTree(Node* Travis, int LeftEdge, int Mid, int RightEdge, int Y)
	{
		if (Travis)
		{
			
			SetCursor(Mid, Y);
			cout << Travis->val;

			SetCursor(Mid, Y + 1);
			cout << Travis->bf;

			cout << endl << endl;

			//think partition
			int NewLeftMid = (Mid + LeftEdge) / 2;
			int NewRightMid = (Mid + RightEdge) / 2;

			RecShowTree(Travis->left, LeftEdge, NewLeftMid, Mid, Y + 2);
			RecShowTree(Travis->right, Mid, NewRightMid, RightEdge, Y + 2);

			SetCursor(0, 20);
		}
	}

public:
	void Add(int num)
	{
		done = false;
		recAdd(Root, num);
	}

	void Clear()
	{
		recClear(Root);
		Root = NULL;
		cout << "Tree has been Cleared" << endl;
	}

	void InOrder()
	{
		if (Root)
		{
			recinorder(Root);
			cout << endl;
		}
		else
		{
			cout << "Tree is empty" << endl;
		}
	}

	void ShowTree()
	{
		RecShowTree(Root, 0, 39, 79, 0);
	}
};
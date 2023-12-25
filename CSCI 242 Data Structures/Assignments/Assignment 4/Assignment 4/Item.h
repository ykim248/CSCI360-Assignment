#pragma once
#include <string>

using namespace std;

struct Item {
	string Name;
	int Quantity;
	int Weight;
	int Value;	
	int Taken = 0;
};

ifstream &operator>> (ifstream &left, Item &right) {
	left >> right.Name;
	left >> right.Quantity;
	left >> right.Weight;
	left >> right.Value;
	return left;
}
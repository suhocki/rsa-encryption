#pragma once
#include <string>
#include <math.h>
#include <sstream>
#include <iostream>

using namespace std;

class numTransfer {
private:
	int inputDec;
	int outputDec;
	string inputHex;
	string outputHex;

public:
	numTransfer();
	static string toHex(int in);
	static int toDec(string in);
};
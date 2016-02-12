#include "numTransfer.h"

numTransfer::numTransfer()
{
}

string numTransfer::toHex(int in)
{
	numTransfer obj;
	obj.inputDec = in;

	string ch = "fedcba9876543210";
	string s;
	stringstream ss;
	while (obj.inputDec) {
		ss << ch[15 - (obj.inputDec % 16)];
		ss >> s;
		obj.outputHex.insert(0, s);
		obj.inputDec = obj.inputDec / 16;
		ss.clear();
	}

	return obj.outputHex;
}

int numTransfer::toDec(string in)
{
	numTransfer obj;
	obj.inputHex = in;

	string ch = "0123456789abcdef";
	obj.outputDec = 0;
	int power = obj.inputHex.size();
	for (int i = 0; i < obj.inputHex.size(); i++) {
		for (int j = 0; j < 16; j++) {
			if (obj.inputHex[i] == ch[j]) {
				obj.outputDec += (j) * pow(16, power-1);
				power--;
			}
		}
	}

	return obj.outputDec;
}



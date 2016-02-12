#pragma once
#include <string>
#include "key.h"
#include <fstream>
#include <iostream>
#include "numTransfer.h"
#include <sstream>
#include <iterator>

using namespace std;

class rsa
{
private:
	Key key;
	int static encrypt(int text, int* pbkey);
	int static decrypt(int text, int* prkey);

public:
	int* getPublicKey();
	int* getPrivateKey();
	void static saveKeys(string outpb, string outpr);
	void static encryptTxtFile(string in, string out, string pbkey);
	void static decryptTxtFile(string in, string out, string prkey);
};
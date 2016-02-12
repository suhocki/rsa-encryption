#pragma once
#include <vector>
#include <time.h>
#include <iostream>

class Key {
	int p;
	int q;
	int n;
	int e;
	int d;
	int* primeArray;
	int* getPrimeArray(int size);
	int getD();

public:
	Key();
	int* publicKey;
	int* privateKey;
};
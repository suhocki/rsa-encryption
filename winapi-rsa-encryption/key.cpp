#include "key.h"


using namespace std;

Key::Key()
{
	srand((unsigned int)time(NULL));
	primeArray = getPrimeArray(1500);
	p = primeArray[38 + rand() % 20];
	q = primeArray[18 + rand() % 20];
	n = p*q;
	e = primeArray[8 + rand() % 10];
	d = getD();
	publicKey = new int[1];
	privateKey = new int[1];

	cout << e << " " << n << " " << d << endl;

	publicKey[0] = e;
	publicKey[1] = n;
	privateKey[0] = d;
	privateKey[1] = n;
}

int* Key::getPrimeArray(int size)
{
	int i, j;
	int N;
	bool *mas;

	N = size + 1;
	mas = new bool[N];

	for (i = 1; i <= size; i++) mas[i] = true;

	for (i = 2; ((i*i) <= size); i++)
		if (mas[i])
			for (j = (i*i); j <= size; j += i)
				if (mas[j]) mas[j] = false;

	vector<int> vec;

	for (i = 2; i <= size; i++)
		if (mas[i]) {
			vec.push_back(i);
		}

	int * buff = new int[vec.size()];
	for (i = 0; i < vec.size(); i++) {
		buff[i] = vec[i];
	}

	return buff;
}

int Key::getD()
{
	d = 1;
	while ((((e*d) % ((p - 1)*(q - 1))) != 1) &&
		(((p - 1)*(q - 1)) % e) &&
		(d != (p - 1)*(q - 1))) {
		d++;
	};

	return d;
}
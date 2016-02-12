#include "rsa.h"

using namespace std;

int rsa::encrypt(int text, int * pbkey)
{
	int result = 1;
	int buff1 = pbkey[0];
	int buff2 = pbkey[1];

	while (pbkey[0]) {
		if (pbkey[0] & 1)
			result = (result * text) % pbkey[1];
		text = (text * text) % pbkey[1];
		pbkey[0] >>= 1;
	}

	pbkey[0] = buff1;
	pbkey[1] = buff2;

	return result;
}

int rsa::decrypt(int text, int * prkey)
{
	int result = 1;
	int buff1 = prkey[0];
	int buff2 = prkey[1];

	while (prkey[0]) {
		if (prkey[0] & 1)
			result = (result * text) % prkey[1];
		text = (text * text) % prkey[1];
		prkey[0] >>= 1;
	}

	prkey[0] = buff1;
	prkey[1] = buff2;

	return result;
}

int* rsa::getPublicKey()
{
	return key.publicKey;
}

int* rsa::getPrivateKey()
{
	return key.privateKey;
}

void rsa::saveKeys(string outpb, string outpr)
{
	Key cryptor;
	
	ofstream foutpb(outpb);
	foutpb << cryptor.publicKey[0] << " " << cryptor.publicKey[1] << " ";
	ofstream foutpr(outpr);
	foutpr << cryptor.privateKey[0] << " " << cryptor.privateKey[1] << " ";
	foutpb.close();
	foutpr.close();
}

void rsa::encryptTxtFile(string in, string out, string pbkey)
{
	ifstream fin(in);
	ofstream fout(out);
	int ascii[255];
	char ch;


	ifstream fkey(pbkey);

	string word;
	int i = 0;
	int* key = new int[1];
	while (getline(fkey, word, ' ')) {
		key[i++] = stoi(word);
	}
	cout << key[0] << " " << key[1] << endl;



	i = 0;
	while (fin.get(ch)) {
		ascii[i++] = ch;
	}

	int cryptotext;
	cryptotext = rsa::encrypt(ascii[0], key);

	string hex = numTransfer::toHex(cryptotext);
	while (hex.size() < 4) {
		hex.insert(0, "0");
	}

	fout << hex;

	int buff;
	for (int j = 1; j < i; j++) {
		ascii[j] = (ascii[j] + ascii[j - 1]) % key[1];
		cryptotext = rsa::encrypt(ascii[j], key);
		hex = numTransfer::toHex(cryptotext);
		while (hex.size() < 4) {
			hex.insert(0, "0");
		}
		fout << hex;
	}

	fout.close();
	fin.close();
}

void rsa::decryptTxtFile(string in, string out, string prkey)
{
	ifstream fin(in);
	ifstream fkey(prkey);
	ofstream fout(out);
	char ch;
	
	string word;
	int i = 0;
	int* key = new int[1];
	while (getline(fkey, word, ' ')) {
		key[i++] = stoi(word);
	}
	cout << key[0] << " " << key[1] << endl;
	string line;
	vector<string> hex;
	
	i = 0;
	int blockSize = 4;
	hex.push_back("");

	while (fin.get(ch)) {
		if ((hex[i].size() == 0) && (ch == '0')) {
			blockSize--;
		}
		else {
			if (hex[i].size() == blockSize) {
				i++;
				blockSize = 4;
				hex.push_back("");
			}
			hex[i].push_back(ch);
		}
	}

	int dec;
	int decPrev;

	for (int i = 0; i < hex.size(); i++) {
		if (i != 0) {
			decPrev = dec;
		}
		dec = numTransfer::toDec(hex[i]);
		if (i == 0) {
			fout << (char)decrypt(dec, key);
		}
		else {
			fout << (char)(decrypt(dec, key) - decrypt(decPrev, key));
		}
	}

	fout.close();
	fin.close();
}

#include "rsa.h"
#include "numTransfer.h"
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {
	std::cout << "RSA Encryption/Decryption v1.0\n";
	std::cout << "Copyright (c) 2015 Maksim Suhotski\n\n";

	if (argc == 5)
	{
		if (argv[1][0] == 'e')
		{
			std::cout << "Encrypting data...\n";
			rsa::encryptTxtFile(argv[2], argv[3], argv[4]);
			cout << "Saving crypted data...\n";
			cout << "Success!\n";
			return 0;
		}
		if (argv[1][0] == 'd')
		{
			std::cout << "Dencrypting data...\n";
			rsa::decryptTxtFile(argv[2], argv[3], argv[4]);
			cout << "Saving decrypted data...\n";
			cout << "Success!\n";
			return 0;
		}
	}
	else 
		if (argc == 4)
		{
			if (argv[1][0] == 'g')
			{
				cout << "Generating keys...\n";
				rsa::saveKeys(argv[2], argv[3]);
				cout << "Saving keys...\n";
				cout << "Success!\n";
				return 0;
			}
		}
	else
		if (argc == 2) {
			if (argv[1][0] == 'h' || argc == 2)
			{
				cout << "Usage:\n";
				cout << " e <in> <out> <public_key_path> - Encrypt data from the file <in> to <out>.\n";
				cout << " g <pb_key_path> <pr_key_path> - Generate keys.\n";
				cout << " d <in> <out> <private_key> - Encrypt data from the file <in> to <out>.\n";
				return 0;
			}
		}

	std::cout << "Type 'h' for help\n";
	return 0;
}	
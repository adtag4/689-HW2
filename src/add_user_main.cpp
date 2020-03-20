#include "CryptoEval.h"

#include <string>
#include <iostream>
#include <fstream>

void printUsage()
{
	std::cout << "Usage: ./adduser <username> <passwd>";
}

int main(int argc, char **argv)
{
	if(argc != 3)
	{
		printUsage();
		return 0;
	}

	CryptoEval cipher;

	std::string username(argv[1]);
	std::string password(argv[2]);

	std::string salt = cipher.genSalt();

	User toAdd(username, salt, cipher.hash(salt, password));
	std::cout << "Passwd: " << password << std::endl;
	std::cout << "Salt: " << salt << std::endl;
	std::cout << "Hash: " << toAdd.hash_ << std::endl;

	auto existing = cipher.readPasswd();
	for(auto x : existing)
	{
		if(toAdd == x)
		{
			std::cerr << "That user already exists." << std::endl;
			return 0;
		}
	}
	
	existing.push_back(toAdd);

	std::ofstream output("passwd.txt");
	for(auto x : existing)
	{
		output << x << std::endl;
	}
	output.close();
	return 0;
}

// CryptoEval.cpp

#include "CryptoEval.h"
#include "User.h"

#include <string>
#include <random>
#include <iostream>
#include <sstream>
#include <ctime>
#include <fstream>

#include <sys/types.h>
#include <unistd.h>
#include <argon2.h>
#include <string.h>

std::string CryptoEval::genSalt() 
{
	srand(time(0));

	// salt len is 16
	uint8_t *salt = (uint8_t *) calloc(16, 1); 
	for(auto i = 0; i < 16; i++)
	{
		salt[i] = (uint8_t) rand()%256;
	}
	
	return ui8pts(salt, 16);
}

std::string CryptoEval::hash(std::string salt, std::string data) 
{
	uint8_t* saltV = stui8p(salt);
	uint8_t* dataV = rawString(data);
	uint8_t* hash = (uint8_t *) calloc(32, 1);
	uint32_t lenData = data.length();
	
	// performance parameters taken from github example code
	argon2i_hash_raw(2, (1<<16), 1, dataV, lenData, saltV, 16, hash, 32);

	return ui8pts(hash, 32);
}

uint8_t *CryptoEval::rawString(std::string s)
{
	uint8_t *ret = (uint8_t *) calloc(s.length(), 1);
	
	for(auto i = 0; i < s.length(); i++)
	{
		ret[i] = (uint8_t) s.c_str()[i];
	}
	return ret;
}

uint8_t *CryptoEval::stui8p(std::string s)
{
	uint8_t *ret = (uint8_t *) calloc(strlen(s.c_str())/2, 1);

	int tmp = 0;
	char c1, c2;
	for(auto i = 0; i < strlen(s.c_str()); i+=2)
	{
		tmp = 0;
		
		c1 = s.c_str()[i];
		c2 = s.c_str()[i+1];
		
		if(c1 >= 0x30 && c1 <= 0x39)
		{
			tmp += (c1 - 0x30);
			tmp *= 16;
		}
		else if(c1 >= 0x41 && c1 <= 0x46)
		{
			tmp += (c1 - 0x41) + 0xA;
			tmp *= 16;
		}
			
		if(c2 >= 0x30 && c2 <= 0x39)
		{
			tmp += (c2 - 0x30);
		}
		else if(c2 >= 0x41 && c2 <= 0x46)
		{
			tmp += (c2 - 0x41) + 0xA;
		}
		
		ret[i/2] = (uint8_t) (tmp % 256);
	}

	return ret;
}

std::string CryptoEval::ui8pts(uint8_t *s, int len)
{
	std::stringstream ss;

	int tmp = 0;
	char c1, c2;
	for(auto i = 0; i < len; i++)
	{
		tmp = s[i] % 16; // find least significant hex char
		if(tmp < 10)
		{
			c2 = (char) tmp + 0x30;
		}
		else
		{
			c2 = (char) tmp + 0x41 - 0xA;
		}

		tmp = s[i] / 16; // find most significant hex char
		if(tmp < 10)
		{
			c1 = (char) tmp + 0x30;
		}
		else
		{
			c1 = (char) tmp + 0x41 - 0xA;
		}
		
		ss << c1 << c2;
	}
	return ss.str();
}

std::vector<User> CryptoEval::readPasswd() 
{
	std::ifstream passwdFile("passwd.txt");
	User u;
	std::vector<User> ret;
	while(passwdFile >> u)
	{
		ret.push_back(u);
	}
	return ret;
}


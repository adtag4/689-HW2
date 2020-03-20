// CryptoEval.h

# pragma once

#include <string>
#include <vector>

#include <unistd.h>
#include <sys/types.h>

#include "User.h"

class CryptoEval
{
	public:
		CryptoEval() {};
		~CryptoEval() {};

		std::string genSalt();
		std::string hash(std::string salt, std::string data);
		
		std::vector<User> readPasswd();

		uint8_t		*rawString(std::string s);
		uint8_t		*stui8p(std::string s);
		std::string	ui8pts(uint8_t *s, int len);
};

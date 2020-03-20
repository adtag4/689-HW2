/* User.h
 * Author: Albert Taglieri
 * Purpose: Interface/doc for a user class
 */

#pragma once

// relevant includes
#include <string>

class User
{
	public:
		User() {};
		User(std::string username);
		User(std::string username, std::string hash);
		User(std::string username, std::string salt, std::string hash);
		~User() {};

		bool operator == (User& otherUser); // for comparing username
		bool operator != (User& otherUser); // for both username and hash (stronger)

		std::string	username_;
		std::string	salt_;
		std::string	hash_;
};

std::ostream& operator << (std::ostream& out, User& u);
std::istream& operator >> (std::istream& in,  User& u);


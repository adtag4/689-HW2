// User.cpp

#include "User.h"

User::User(std::string username)
	: username_(username)
{

}

User::User(std::string username, std::string hash)
	: username_(username),
	  salt_(""),
	  hash_(hash)
{

}

User::User(std::string username, std::string salt, std::string hash)
	: username_(username),
	  salt_(salt),
	  hash_(hash)
{
	
}

bool User::operator == (User& otherUser)
{
	return (username_ == otherUser.username_);
}

bool User::operator != (User& otherUser)
{
	if(hash_ == otherUser.hash_)
	{
		return false;
	}
	return true;
}

std::ostream& operator << (std::ostream& out, User& u)
{
	std::string separator(":");
	out << u.username_;
	out << separator;
	out << u.salt_;
	out << separator;
	out << u.hash_;
	out << separator;
	return out;
}

std::istream& operator >> (std::istream& in, User& u)
{
	std::string s;
	std::getline(in, s, ':');
	u.username_ = s;

	std::getline(in, s, ':');
	u.salt_ = s;

	std::getline(in, s, ':');
	u.hash_ = s;

	return in;
}

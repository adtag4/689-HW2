// IP.cpp

#include "IP.h"
#include <iostream>
#include <sstream>

IP::IP(std::string ipAddr)
{
//	std::cout << "Building IP: " << ipAddr << std::endl;
	std::string s;
	std::stringstream ss;
	ss.str(ipAddr);
	std::getline(ss, s, '.');
	b3 = std::stoi(s);
	std::getline(ss, s, '.');
	b2 = std::stoi(s);
	std::getline(ss, s, '.');
	b1 = std::stoi(s);
	std::getline(ss, s, '.');
	b0 = std::stoi(s);
}

std::string IP::toString()
{
	std::stringstream ss;
	ss << static_cast<int>(b3) << '.' << static_cast<int>(b2) << '.'; 
	ss << static_cast<int>(b1) << '.' << static_cast<int>(b0);
	return ss.str();
}

bool IP::operator == (IP& other)
{
	if(b0 != other.b0) {return false;}
	if(b1 != other.b1) {return false;}
	if(b2 != other.b2) {return false;}
	if(b3 != other.b3) {return false;}
	return true;
}

std::ostream& operator << (std::ostream& out, IP& data)
{
	out << data.toString();
	return out;
}

std::istream& operator >> (std::istream& in, IP& data)
{
	std::string s;
	in >> s;
	IP test(s);
	data = test;
	return in;
}

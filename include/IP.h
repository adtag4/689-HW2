/* IP.h
 * Author: Albert Taglieri
 * Purpose: Interface/doc for an IP address class
 */

#pragma once

// relevant includes
#include <string>
#include <iostream>

// I assume IPv4 

class IP
{
	public:
		IP() {};
		IP(std::string ipAddr);
		~IP() {};

		// comparison operator
		bool operator == (IP& other);
		std::string toString();
		
		char	b0; // actual byte values, 0-3 (little endian)
		char	b1;
		char	b2;
		char	b3;
	
		
};

std::ostream& operator << (std::ostream& out, IP& data);
std::istream& operator >> (std::istream& in,  IP& data);

/* Client.h
 * Author: Albert Taglieri
 * Purpose: Interface/doc for clients
 */

#pragma once

// relevant includes
#include <Session.h>
#include <IP.h>

class Client
{
	public:
		Client();
		Client(std::string server, int port);
		~Client() {};
		
		// main() performs the execution of the client
		void	main();
		
	protected:
	private:
		IP	serverAddr_;
		int	serverPort_;
		Session	activeConnection_;
};

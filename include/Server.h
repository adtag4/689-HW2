/* Server.h
 * Author: Albert Taglieri
 * Purpose: Provide interface/doc for Server class
 */

#pragma once

// Relevant includes
#include <vector>

#include "User.h"
#include "Session.h"
#include "Logger.h"
#include "IP.h"
#include "CryptoEval.h"

class Server
{
	public:
		Server();
		Server(int port);
		~Server() {};

		void	main();
		void	readPasswd();
		void	readWhitelist();
		void	updatePasswd();
	protected:
	private:
		std::vector<User>	userList_;
		std::vector<IP>		whitelist_;
		int			listenSocket_;
		int			port_;
		Logger			log_;
		bool			running_;
};

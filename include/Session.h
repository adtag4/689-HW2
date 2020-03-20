/* Session.h
 * Author: Albert Taglieri
 * Purpose: Interface/doc for a session (ie active, ongoing connection)
 */

#pragma once

// relevant includes
#include <IP.h>
#include <string>
#include <iostream>
#include <sstream>


#include "CryptoEval.h"
#include "Logger.h"
#include "User.h"
#include "IP.h"

// enum class to label which end of a session I am locally
enum class SessionMode {SERVER_MODE, CLIENT_MODE};

/* Session description
 * CLIENT				SERVER
 * 		[connection made]
 * S_CONNECTING				S_CONNECTING (waits and verifies username)
 * 	
 * (block until rfu)			(block until username)
 * (send username)			(send passwd prompt)	
 * 					
 * S_AUTH				S_AUTH (waits and verifies password)
 * 					
 * (block until rfp)			(block until password)
 * (send password)			(send menu/prompt)
 *
 * S_ACTION				S_ACTION (sends menu and waits for option)
 * 
 * (block until prompt)			(block until choice)
 * (send choice/reply)			(send reply/prompt) ==> back to S_ACTION
 * ==> back to S_ACTION
 * 					S_PW ()
 *		
 *					(block until pw)
 *					(send reply/promlt) ==> back to S_ACTION
 */

enum class State {S_CONNECTING, S_AUTH, S_ACTION, S_CLOSED, S_PASSCHANGE};

class Session
{
	public:
		Session() {}; // Deleted b/c a session never exists without a socket (DONT USE)
		Session(int socketFD, SessionMode mode); // for clients - ideally separate to subclasses
		Session(int socketFD, SessionMode mode, std::vector<IP>& whitelist, std::vector<User> *passwd, Logger *log); // this ctor ought be used for server mode
		~Session() {};
		
		SessionMode	getMode();
		
		//void		send(int len, char *data); // Easier to work with than c++ abstractions
		//char		*recv(int len);
		//char		*recvUntil(char term);

		// C++ string equivalents b/c people like those
		//std::string	recv(int len);
		//std::string	recvUntil(char term);

		// performs session functionality with state machine
		void main(); 
	protected:
		// different mode main threads
		void 		serverMain();
		void		clientMain();

		// server functionality
		void		serverConnecting();
		void		serverAuth();
		void		serverAction();

		// client functionality
		void		clientConnecting();
		void		clientAuth();
		void		clientAction();
		
		void		sessionExit(); // if S_CLOSED

		bool		isOnWhitelist(IP& ip);
		bool		isLegitUser(User& u);
		bool		isLegitPass(User& u);

		int		parseResponse(std::string s);

		void		setEchoIn(bool enable);

		// sever actions
		void		greet();
		void		resp1();
		void		resp2();
		void		resp3();
		void		resp4();
		void		resp5();
		void		respErr();
		void		passChange();

		void		sendMenu();
		void		sendPrompt();
		void		sendBuffer();

		std::string	receiveBuffer();

	private:
		IP		remoteAddr_;
		SessionMode	sessionMode_;
		int		socketFD_;
		State		sessionState_;
		bool		active_;
		User		*user_; // active user on session
		CryptoEval	cipher_;
		std::stringstream ss_; // sending buffer
		std::stringstream rs_; // receiving buffer
		std::vector<IP>		whitelist_;
		std::vector<User>	*passwd_;
	
		Logger		*Log;
};	

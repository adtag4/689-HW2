// Session.cpp

#include "Session.h"
#include "Logger.h"
#include "User.h"
#include "IP.h"
#include "CryptoEval.h"

#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <termios.h>

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <exception>

#define BUF_SZ 2048

Session::Session(int socketFD, SessionMode mode)
	: sessionMode_(mode),
	  socketFD_(socketFD),
	  sessionState_(State::S_CONNECTING)
{
	// safety
	if(mode == SessionMode::SERVER_MODE)
	{
		std::cerr << "You made a server with the client session constructor" << std::endl;
		exit(-1);
	}

	active_ = true;	
}

Session::Session(int socketFD, SessionMode mode, std::vector<IP>& whitelist, std::vector<User> *passwd, Logger *log)
	: sessionMode_(mode),
	  socketFD_(socketFD),
	  sessionState_(State::S_CONNECTING),
	  whitelist_(whitelist),
	  passwd_(passwd),
	  Log(log)
{
	// get remote details
	struct sockaddr_in saddr;
	socklen_t slen = sizeof(saddr);
	getpeername(socketFD_, (struct sockaddr *)&saddr, &slen);

	std::string ipAddr(inet_ntoa(saddr.sin_addr)); // convert IP to std:string
	remoteAddr_ = IP(ipAddr);
	
	if(!isOnWhitelist(remoteAddr_))
	{
		close(socketFD_);
		sessionState_ = State::S_CLOSED;
	}
	active_ = true;

	// send request for username
	ss_ << "Please enter your username: ";
	sendBuffer();
}

SessionMode Session::getMode()
{
	return sessionMode_;
}

void Session::main()
{
	if(sessionMode_ == SessionMode::SERVER_MODE)
	{
		serverMain();
		
		// can only get here once closed, but have to free memory in cleaning up thread
		this->~Session();
	}
	else
	{
		clientMain();
	}
}

void Session::serverMain()
{
	while(active_)
	{
		switch(sessionState_)
		{
			case State::S_CONNECTING:
				serverConnecting();
				break;
			case State::S_AUTH:
				serverAuth();
				break;
			case State::S_ACTION:
				serverAction();
				break;
			default:
				sessionExit();
		}
	}
}

void Session::clientMain()
{
	while(active_)
	{
		switch(sessionState_)
		{
			case State::S_CONNECTING:
				clientConnecting();
				break;
			case State::S_AUTH:
				clientAuth();
				break;
			case State::S_ACTION:
				clientAction();
				break;
			default:
				sessionExit();
		}
	}
}

void Session::serverConnecting()
{
	// Receive username
	std::string s = receiveBuffer();
	User u(s);
	
	if(isLegitUser(u)) // if true, sets user_ from userlist to contain salt/hash
	{
		sessionState_ = State::S_AUTH;
		std::stringstream ss;
		ss << "Please enter your password: ";
		send(socketFD_, ss.str().c_str(), strlen(ss.str().c_str()), 0);
	}
	else
	{
		close(socketFD_);
		sessionState_ = State::S_CLOSED;
		std::string logEntry = "Invalid user " + s + " rejected from " + remoteAddr_.toString();
		Log->log(logEntry);
	}
}

std::string Session::receiveBuffer()
{
	char buffer[BUF_SZ];
	int  bytes = recv(socketFD_, buffer, BUF_SZ - 1, 0);
	if(bytes <= 0)
	{
		std::string logEntry = "Error on socket from " + remoteAddr_.toString();
		Log->log(logEntry);
		sessionState_ = State::S_CLOSED;
		close(socketFD_);
		return "";
	}
	buffer[bytes] = '\0';

	//DBG
	//std::cout << "RECEIVED: " << std::string(buffer);

	return std::string(buffer);
}

void Session::serverAuth()
{
	// receive password
	std::string buffer = receiveBuffer();
	
	// check username
	std::string unhashed(buffer);
	std::string salt = user_->salt_;
	std::string hash = cipher_.hash(salt, unhashed);
	User u(user_->username_, hash);

	if(isLegitPass(u))
	{
		sessionState_ = State::S_ACTION;
		std::string logEntry = "Session authenticated for user " + user_->username_ + " from " + remoteAddr_.toString();
		Log->log(logEntry);
	}
	else
	{
		close(socketFD_);
		sessionState_ = State::S_CLOSED;
		std::string logEntry = "Invalid credentials for " + user_->username_ + " rejected from " + remoteAddr_.toString();
		Log->log(logEntry);
	}

	sendMenu();
	ss_ << std::endl;
	sendPrompt();
	sendBuffer();
}

void Session::sendBuffer()
{
	//DBG
	//std::cout << "SENDING: " << ss_.str().c_str() << std::endl;

	int bytes = send(socketFD_, ss_.str().c_str(), ss_.str().length(), 0);
	if(bytes <= 0)
	{
		std::string logEntry = "Sending error to " + remoteAddr_.toString();
		Log->log(logEntry);
		sessionState_ = State::S_CLOSED;
		close(socketFD_);
	}

	std::stringstream ss;
	std::stringstream().swap(ss_); // refresh send buffer
}

void Session::sendPrompt()
{
	ss_ << ">>";
}

void Session::sendMenu()
{
	ss_ << "Please enter one of the following options" << std::endl;
	ss_ << "[hello]:  Get yerself greeted." << std::endl;
	ss_ << "[1]:      Learn the answer to life the universe and everything." << std::endl;
	ss_ << "[2]:      Learn how many volumes are in John Owen's complete works." << std::endl;
	ss_ << "[3]:      Fast, Neat, Average." << std::endl;
	ss_ << "[4]:      Learn what time it is." << std::endl;
	ss_ << "[5]:      Learn what happens if you know neither yourself, nor your enemy." << std::endl;
	ss_ << "[passwd]: Allows you to change your password." << std::endl;
	ss_ << "[exit]:   Logout and disconnect." << std::endl;
	ss_ << "[menu]:   Display this screen.";
}

void Session::serverAction()
{
	std::string logEntry; // declare to avoid compiler blabbing
	// receive response
	std::string buffer = receiveBuffer();

	// Parse and execute relevant command gotten from user
	std::string command(buffer);
	int cmd = parseResponse(command);
	switch(cmd)
	{
		case 0:
			greet();
			sessionState_ = State::S_ACTION;
			break;
		case 1:
			resp1();
			sessionState_ = State::S_ACTION;
			break;
		case 2:
			resp2();
			sessionState_ = State::S_ACTION;
			break;
		case 3:
			resp3();
			sessionState_ = State::S_ACTION;
			break;
		case 4:
			resp4();
			sessionState_ = State::S_ACTION;
			break;
		case 5:
			resp5();
			sessionState_ = State::S_ACTION;
			break;
		case 6:
			passChange(); // takes care of all exchanges (is blocking performing an S_ACTION)
			sessionState_ = State::S_ACTION;
			break;
		case 7:
			sessionState_ = State::S_CLOSED;
			logEntry = "User " + user_->username_ + " on " + remoteAddr_.toString() + " logged out.";
			Log->log(logEntry);
			break;
		case 8:
			sendMenu();
			sessionState_ = State::S_ACTION;
			break;
		default: // for invalid input, send an error and prompt again
			respErr();
			sessionState_ = State::S_ACTION;
	}
	ss_ << std::endl;
	sendPrompt();
	sendBuffer();
}

void Session::clientConnecting()
{
	// block for username request
	std::string buffer = receiveBuffer();

	// display to user
	std::cout << buffer << std::flush;
	
	// get username input from user 
	std::string username;
	std::getline(std::cin, username); //should terminate on endline
	
	ss_ << username;

	// send to server
	sendBuffer();
	sessionState_ = State::S_AUTH;
	
}

// hide input (based on stackoverflow.com/questions/1413445/reading-a-password-from-stdcin)
void Session::setEchoIn(bool enable)
{
	struct termios tty;
	tcgetattr(STDIN_FILENO, &tty);
	if(!enable)
	{
		tty.c_lflag &= ~ECHO;
	}
	else
	{
		tty.c_lflag |= ECHO;
	}

	(void) tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}



void Session::clientAuth()
{
	// block for password request
	std::string buffer = receiveBuffer();

	// display to user
	std::cout << buffer << std::flush;
	
	// hide input
	setEchoIn(false);

	// get username input from user 
	std::string password;
	std::getline(std::cin, password); //should terminate on endline
	
	// reveal input
	setEchoIn(true);

	// send to server
	ss_ << password;
	sendBuffer();
	
	sessionState_ = State::S_ACTION;

}

void Session::clientAction()
{
	// block for prompt
	std::string buffer = receiveBuffer();

	// display to user
	std::cout << buffer << std::flush;
	
	// get username input from user 
	std::string choice;
	std::getline(std::cin, choice); //should terminate on endline
	
	ss_ << choice;
	sendBuffer();
	
	if(choice == "passwd")
	{
		sessionState_ = State::S_AUTH; // hide password on input again, then proceed back to active
	}
	else if(choice == "exit")
	{
		sessionState_ = State::S_CLOSED;
		close(socketFD_);
	}
	else
	{
		sessionState_ = State::S_ACTION;
	}
}

void Session::sessionExit()
{
	std::cerr << "Session is closed." << std::endl;
	active_ = false;
}

bool Session::isOnWhitelist(IP& ip)
{
	bool good = false;
	for(auto x : whitelist_)
	{
		if(x == ip) {good = true; break;};
	}
	return good;
}

bool Session::isLegitUser(User& u)
{
	bool good = false;
	for(auto& x : *passwd_)
	{
		if(x == u) // just checks username and nothing else
		{
			user_ = &x;
			good = true;
			break; // usernames should be unique, but catch any possible errors
		}
	}
	return good;
}

bool Session::isLegitPass(User& u)
{
	if(u != *user_) // checks for username and hash equality
	{
		return false;
	}
	return true;
}

int Session::parseResponse(std::string s)
{
	if(s == "hello")
	{
		return 0;
	}
	else if(s == "menu")
	{
		return 8;
	}
	else if(s == "passwd")
	{
		return 6;
	}
	else if(s == "exit")
	{
		return 7;
	}
	try
	{
		int x = stoi(s);
		return x;
	}
	catch(std::exception e)
	{
		return 9;
	}
}

void Session::resp1()
{
	ss_ << "It's 42! (Exclamation, not factorial)";
}

void Session::resp2()
{
	ss_ << "There are 23: 16 of assorted work, and 7 of exposition on the Epistle to the Hebrews.";
}

void Session::resp3()
{
	ss_ << "Friendly, Good, Good.";
}
void Session::resp4()
{
	auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	ss_ << "Server Time is: " << ctime(&t);
}

void Session::resp5()
{
	ss_ << "You will succumb in every battle.";
}

void Session::greet()
{
	ss_ << "Greetings, " << user_->username_ << ".";
}
void Session::respErr()
{
	ss_ << "Invalid response.";
}

void Session::passChange()
{
	// send request for password
	ss_ << "Enter your new password: ";
	sendBuffer();

	// block until recvd, then eval and store
	std::string passwd = receiveBuffer();

	// hash new passwd with salt
	auto hash = cipher_.hash(user_->salt_, passwd);

	// store in user 
	user_->hash_ = hash; // will be updated in file by main thread, but main thread 
			      // now sees the update for all future sessions in real time too.  
	ss_ << std::endl;
	sendPrompt();
	sendBuffer();
	sessionState_ = State::S_ACTION;
}

// Server.cpp

#include "Server.h"
#include "Logger.h"

#include <thread>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>

Server::Server() 
{
	// read the passwd file to populate user list
	readPasswd(); // passwd.txt

	// read the whitelist file to populate the whitelist
	readWhitelist(); // whitelist.txt
	
	// setup log happens implicitly
	port_ = 10000;
}

Server::Server(int port)
{
	port_ = port;

	readPasswd(); // passwd.txt

	// read the whitelist file to populate the whitelist
	readWhitelist(); // whitelist.txt
	
}

void Server::main() 
{
	running_ = true;
	
	// dynamically update the file every minute
	std::thread(&Server::updatePasswd, this).detach();
	
	// make listener
	listenSocket_ = socket(AF_INET, SOCK_STREAM, 0);
	if(listenSocket_ == -1)
	{
		std::cerr << "Failed to make socket" << std::endl;
		log_.log("Call to socket() failed: terminating server.");
		log_.shutdown();
		running_ = false;
		return;
	}

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port_);
	inet_pton(AF_INET, "0.0.0.0", &addr.sin_addr);

	int err;
	err = bind(listenSocket_, (struct sockaddr *) &addr, sizeof(addr));
	if(err == -1)
	{
		std::cerr << "Failed to bind" << std::endl;
		std::stringstream ss;
		ss << "Call to bind() failed on port " << port_ << ": terminating server.";
		log_.log(ss.str());
		log_.shutdown();
		running_ = false;
		return;
	}

	err = listen(listenSocket_, SOMAXCONN);
	if(err == -1)
	{
		std::cerr << "Failed to listen" << std::endl;
		log_.log("Call to listen() failed: terminating server.");
		log_.shutdown();
		running_ = false;
		return;
	}

	// accept and handle connections
	while(running_)
	{
		struct sockaddr_in clientAddr;
		socklen_t clientSize = sizeof(clientAddr);
		int clientSocket = accept(listenSocket_, (sockaddr *) &clientAddr, &clientSize);
		if(clientSocket == -1)
		{
			log_.log("Failed to accept a connection.");
			continue; // skip back to start of while loop
		}
		
		Session *x = new Session(clientSocket, SessionMode::SERVER_MODE, whitelist_, &userList_, &log_);
		std::thread(&Session::main, x).detach();

		// RAII cleans up ptr, but Session object stays in memory running on own thread until closed
	}
}

void Server::readPasswd() 
{
	std::ifstream passwdFile("passwd.txt");
	User u;
	while(passwdFile >> u)
	{
		userList_.push_back(u);
	}
}

void Server::readWhitelist() 
{
	std::ifstream whitelistFile("whitelist.txt");
	IP ip;
	std::string s;
	std::stringstream ss;
	while(std::getline(whitelistFile, s))
	{
		ss.str(s);
		ss >> ip;
		whitelist_.push_back(ip);
	}
}

void Server::updatePasswd() 
{
	while(running_)
	{
		std::this_thread::sleep_for(std::chrono::minutes(1));
	
		// write userList_ to passwd.txt
		std::ofstream passwdFile;
		passwdFile.open("passwd.txt");
		for(auto x : userList_)
		{
			passwdFile << x << std::endl;
		}
		passwdFile.close();
	}
}




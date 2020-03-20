#include "Client.h"
#include "Session.h"

#include <string>
#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <dirent.h>

Client::Client() : Client("127.0.0.1", 10000)
{}

Client::Client(std::string server, int port)
	: serverAddr_(server),
	  serverPort_(port)
{}

void Client::main()
{
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	
	struct addrinfo		*ai_head;
	struct sockaddr_in	*addr_in;
	struct sockaddr_in	serv_addr;

	std::string server = serverAddr_.toString();
	
	int err = getaddrinfo(server.c_str(), NULL, NULL, &ai_head);
	if(err)
	{
		std::cerr << "Could not resolve server" << std::endl;
		return;
	}

	addr_in = (struct sockaddr_in *) ai_head->ai_addr;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = addr_in->sin_addr.s_addr;
	serv_addr.sin_port = htons(serverPort_);

	err = connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	if(err < 0)
	{
		std::cerr << "Failed connecting" << std::endl;
		return;
	}

	Session s(sock, SessionMode::CLIENT_MODE);
	s.main();
}

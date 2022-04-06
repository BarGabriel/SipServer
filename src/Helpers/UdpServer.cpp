#include "UdpServer.hpp"
#include <sstream>
#include <vector>
#include <thread>

UdpServer::UdpServer(std::string ip, int port, OnNewMessageEvent event) : _ip(std::move(ip)), _port(port), _onNewMessageEvent(event), _keepRunning(false)
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		std::cerr << "Failed. Error Code: " << WSAGetLastError() << std::endl;
		exit(EXIT_FAILURE);
	}

	if ((_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		std::cerr << "socket creation failed" << std::endl;
		exit(EXIT_FAILURE);
	}

	memset(&_servaddr, 0, sizeof(_servaddr));
	_servaddr.sin_family = AF_INET;
	_servaddr.sin_addr.s_addr = inet_addr(_ip.c_str());
	_servaddr.sin_port = htons(port);

	if (bind(_sockfd, reinterpret_cast<const struct sockaddr*>(&_servaddr), sizeof(_servaddr)) < 0)
	{
		std::cerr << "bind failed" << std::endl;
		exit(EXIT_FAILURE);
	}
}

UdpServer::~UdpServer()
{
	close();
}

void UdpServer::startReceive()
{
	_keepRunning = true;
	_receiverThread = std::thread([=]
		{
			char buffer[BUFFER_SIZE];
			sockaddr_in senderEndPoint;
			memset(&senderEndPoint, 0, sizeof(senderEndPoint));
			int len = sizeof(senderEndPoint);

			while (_keepRunning)
			{
				recvfrom(_sockfd, buffer, BUFFER_SIZE, 0, reinterpret_cast<struct sockaddr*>(&senderEndPoint), &len);
				if (_keepRunning) 
				{
					_onNewMessageEvent(std::move(buffer), senderEndPoint);
				}				
			}
		});
}

int UdpServer::send(sockaddr_in address, std::string buffer)
{
	return sendto(_sockfd, buffer.c_str(), strlen(buffer.c_str()),
		0, reinterpret_cast<const struct sockaddr*>(&address), sizeof(address));
}

void UdpServer::close()
{
	_keepRunning = false;
	shutdown(_sockfd, 2);
	closesocket(_sockfd);
	_receiverThread.join();
}
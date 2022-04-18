#ifndef UDP_SERVER_HPP
#define UDP_SERVER_HPP

#ifdef  __linux__
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#elif defined _WIN32 || defined _WIN64
#include <WinSock2.h>
#endif

#include <iostream>
#include <atomic>
#include <functional>
#include <thread>

class UdpServer
{
public:
	using OnNewMessageEvent = std::function<void(std::string, sockaddr_in)>;
	static constexpr int BUFFER_SIZE = 2048;

	UdpServer(std::string ip, int port, OnNewMessageEvent event);
	~UdpServer();

	void startReceive();
	int send(struct sockaddr_in address, std::string buffer);

private:
	void closeServer();

	std::string _ip;
	int _port;
	int _sockfd;
	sockaddr_in _servaddr;
	OnNewMessageEvent _onNewMessageEvent;
	std::atomic<bool> _keepRunning;
	std::thread _receiverThread;
};

#endif
#ifndef UDP_SERVER_HPP
#define UDP_SERVER_HPP

#include <WinSock2.h>
#include <iostream>
#include <functional>

class UdpServer
{
public:
	using OnNewMessageEvent = std::function<void(std::string, sockaddr_in)>;
	static constexpr int BUFFER_SIZE = 2048;

	UdpServer(std::string ip ,int port, OnNewMessageEvent event);

	void startReceive();
	int send(struct sockaddr_in address, std::string buffer);
	void close();

private:
	std::string _ip;
	int _port;
	int _sockfd;
	sockaddr_in _servaddr;
	OnNewMessageEvent _onNewMessageEvent;
	std::atomic<bool> _keepRunning;
};

#endif
#ifndef SIP_SERVER_HPP
#define SIP_SERVER_HPP

#include "UdpServer.hpp"
#include "RequestsHandler.hpp"
#include "Session.hpp"
#include "SipMessageFactory.hpp"

class SipServer
{
public:
	SipServer(std::string ip, int port = 5060);

private:
	void onNewMessage(std::string data, sockaddr_in src);
	void onHandled(const sockaddr_in& dest, std::shared_ptr<SipMessage> message);

	UdpServer _socket;
	RequestsHandler _handler;
	SipMessageFactory _messagesFactory;
};
#endif
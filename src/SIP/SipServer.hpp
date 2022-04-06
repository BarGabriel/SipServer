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
	void onNewClient(std::shared_ptr<SipClient> newClient);
	void onUnregister(std::shared_ptr<SipClient> client);
	void onHandled(std::string destNumber, std::shared_ptr<SipMessage> message);

	UdpServer _socket;
	RequestsHandler _handler;
	SipMessageFactory _messagesFactory;
	std::unordered_map<std::string, std::shared_ptr<SipClient>> _clients;
	std::unordered_map<std::string, std::shared_ptr<Session>> _sessions;
};
#endif
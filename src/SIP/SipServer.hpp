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
	void close();

private:

	void onNewMessage(std::string data, sockaddr_in src);
	void onNewClient(SipClient newClient);
	void onUnregister(SipClient client);
	void onHandled(std::string destNumber, std::shared_ptr<SipMessage> message);

	UdpServer _socket;
	RequestsHandler _handler;
	SipMessageFactory _messagesFactory;
	std::unordered_map<std::string, SipClient> _clients;
	std::unordered_map<std::string, Session> _sessions;
};
#endif
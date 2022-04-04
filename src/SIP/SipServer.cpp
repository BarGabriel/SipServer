#include "SipServer.hpp"
#include "SipMessageTypes.h"

SipServer::SipServer(std::string ip, int port) :
	_socket(ip, port, std::bind(&SipServer::onNewMessage, this, std::placeholders::_1, std::placeholders::_2)),
	_handler(ip, port, _sessions, 
		std::bind(&SipServer::onNewClient, this, std::placeholders::_1), 
		std::bind(&SipServer::onUnregister, this, std::placeholders::_1), 
		std::bind(&SipServer::onHandled, this, std::placeholders::_1, std::placeholders::_2))
{
	_socket.startReceive();
}

void SipServer::onNewMessage(std::string data, sockaddr_in src)
{
	auto message = _messagesFactory.createMessage(std::move(data), std::move(src));
	if (message.has_value()) 
	{
		_handler.handle(std::move(message.value()));
	}
}

void SipServer::onNewClient(SipClient newClient)
{
	if (_clients.find(newClient.getNumber()) == _clients.end()) {
		std::cout << "New Client: " << newClient.getNumber() << std::endl;
		_clients.emplace(newClient.getNumber(), newClient);		
	}		
}

void SipServer::onUnregister(SipClient client)
{
	std::cout << "unregister client: " << client.getNumber() << std::endl;
	_clients.erase(client.getNumber());
}

void SipServer::onHandled(std::string destNumber, std::shared_ptr<SipMessage> message)
{
	try
	{
		_socket.send(_clients.at(std::move(destNumber)).getAddress(), message->toString());
	}
	catch (const std::out_of_range&)
	{
		SipMessage messagae(*message);
		messagae.setHeader(SipMessageTypes::NOT_FOUND);
		_socket.send(message->getSource(),std::move(messagae.toString()));
	}
}
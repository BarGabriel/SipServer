#include "RequestsHandler.hpp"
#include <sstream>
#include "SipMessageTypes.h"
#include "SipSdpMessage.hpp"
#include "IDGen.hpp"

RequestsHandler::RequestsHandler(std::string serverIp, int serverPort,
	OnHandledEvent onHandledEvent) :
	_serverIp(std::move(serverIp)), _serverPort(serverPort),
	_onHandled(onHandledEvent)
{
	initHandlers();
}

void RequestsHandler::initHandlers()
{
	_handlers.emplace(SipMessageTypes::REGISTER, std::bind(&RequestsHandler::OnRegister, this, std::placeholders::_1));
	_handlers.emplace(SipMessageTypes::SUBSCRIBE, std::bind(&RequestsHandler::OnSubscribe, this, std::placeholders::_1));
	_handlers.emplace(SipMessageTypes::CANCEL, std::bind(&RequestsHandler::OnCancel, this, std::placeholders::_1));
	_handlers.emplace(SipMessageTypes::INVITE, std::bind(&RequestsHandler::OnInvite, this, std::placeholders::_1));
	_handlers.emplace(SipMessageTypes::TRYING, std::bind(&RequestsHandler::OnTrying, this, std::placeholders::_1));
	_handlers.emplace(SipMessageTypes::RINGING, std::bind(&RequestsHandler::OnRinging, this, std::placeholders::_1));
	_handlers.emplace(SipMessageTypes::BUSY, std::bind(&RequestsHandler::OnBusy, this, std::placeholders::_1));
	_handlers.emplace(SipMessageTypes::UNAVAIALBLE, std::bind(&RequestsHandler::OnUnavailable, this, std::placeholders::_1));
	_handlers.emplace(SipMessageTypes::OK, std::bind(&RequestsHandler::OnOk, this, std::placeholders::_1));
	_handlers.emplace(SipMessageTypes::ACK, std::bind(&RequestsHandler::OnAck, this, std::placeholders::_1));
	_handlers.emplace(SipMessageTypes::BYE, std::bind(&RequestsHandler::OnBye, this, std::placeholders::_1));
	_handlers.emplace(SipMessageTypes::REQUEST_TERMINATED, std::bind(&RequestsHandler::onReqTerminated, this, std::placeholders::_1));
}

void RequestsHandler::handle(std::shared_ptr<SipMessage> request)
{
	if (_handlers.find(request->getType()) != _handlers.end())
	{
		_handlers[request->getType()](std::move(request));
	}
}

std::optional<std::shared_ptr<Session>> RequestsHandler::getSession(const std::string& callID)
{
	auto sessionIt = _sessions.find(callID);
	if (sessionIt != _sessions.end())
	{
		return sessionIt->second;
	}
	return {};
}

void RequestsHandler::OnRegister(std::shared_ptr<SipMessage> data)
{
	bool isUnregister = data->getContact().find("expires=0") != -1;

	if (!isUnregister)
	{
		auto newClient = std::make_shared<SipClient>(data->getFromNumber(), data->getSource());
		registerClient(std::move(newClient));
	}

	auto response = data;
	response->setHeader(SipMessageTypes::OK);
	response->setVia(data->getVia() + ";received=" + _serverIp);
	response->setTo(data->getTo() + ";tag=" + IDGen::GenerateID(9));
	response->setContact("Contact: <sip:" + data->getFromNumber() + "@" + _serverIp + ":" + std::to_string(_serverPort) + ";transport=UDP>");
	endHandle(response->getFromNumber(), response);

	if (isUnregister)
	{
		auto newClient = std::make_shared<SipClient>(data->getFromNumber(), data->getSource());
		unregisterClient(std::move(newClient));
	}
}

void RequestsHandler::OnSubscribe(std::shared_ptr<SipMessage> data)
{
	std::ostringstream okResponse;
	okResponse << "SIP/2.0 200 OK\r\n"
		<< data->getVia() << ";received=" + _serverIp << "\r\n"
		<< data->getFrom() << "\r\n"
		<< data->getTo() << ";tag=" << IDGen::GenerateID(9) << "\r\n"
		<< data->getCallID() << "\r\n"
		<< data->getCSeq() << "\r\n"
		<< data->getContact() << "\r\n"
		<< "Server: Sip server" << "\r\n"
		<< data->getContentLength() << "\r\n"
		<< "\r\n";

	auto response = std::make_shared<SipMessage>(okResponse.str(), data->getSource());
	endHandle(data->getFromNumber(), std::move(response));
}

void RequestsHandler::OnCancel(std::shared_ptr<SipMessage> data)
{
	endCall(data->getCallID(), data->getFromNumber(), data->getToNumber(), data->getFromNumber() + " canceled the session.");
	endHandle(data->getToNumber(), data);
}

void RequestsHandler::onReqTerminated(std::shared_ptr<SipMessage> data)
{
	endHandle(data->getFromNumber(), data);
}

void RequestsHandler::OnInvite(std::shared_ptr<SipMessage> data)
{
	// Check if the caller is registered
	auto caller = findClient(data->getFromNumber());
	if (!caller.has_value()) 
	{
		return;
	}

	// Check if the called is registered
	auto called = findClient(data->getToNumber());
	if (!called.has_value())
	{
		// Send "SIP/2.0 404 Not Found"
		data->setHeader(SipMessageTypes::NOT_FOUND);
		data->setContact("Contact: <sip:" + caller.value()->getNumber() + "@" + _serverIp + ":" + std::to_string(_serverPort) + ";transport=UDP>");
		endHandle(data->getFromNumber(), data);
		return;
	}

	auto messge = dynamic_cast<SipSdpMessage*>(data.get());
	auto newSession = std::make_shared<Session>(data->getCallID(), caller.value(), messge->getRtpPort());
	_sessions.emplace(data->getCallID(), newSession);

	auto response = data;
	response->setContact("Contact: <sip:" + caller.value()->getNumber() + "@" + _serverIp + ":" + std::to_string(_serverPort) + ";transport=UDP>");
	endHandle(data->getToNumber(), response);
}

void RequestsHandler::OnTrying(std::shared_ptr<SipMessage> data)
{
	setCallState(data->getCallID(), Session::State::Trying);
	endHandle(data->getFromNumber(), data);
}

void RequestsHandler::OnRinging(std::shared_ptr<SipMessage> data)
{
	setCallState(data->getCallID(), Session::State::Ringing);
	endHandle(data->getFromNumber(), data);
}

void RequestsHandler::OnBusy(std::shared_ptr<SipMessage> data)
{
	endCall(data->getCallID(), data->getFromNumber(), data->getToNumber(), data->getToNumber() + " is busy");
	endHandle(data->getFromNumber(), data);
}

void RequestsHandler::OnUnavailable(std::shared_ptr<SipMessage> data)
{
	endCall(data->getCallID(), data->getFromNumber(), data->getToNumber(), data->getToNumber() + " is unavailable");
	endHandle(data->getFromNumber(), data);
}

void RequestsHandler::OnBye(std::shared_ptr<SipMessage> data)
{
	endCall(data->getCallID(), data->getToNumber(), data->getFromNumber());
	endHandle(data->getToNumber(), data);
}

void RequestsHandler::OnOk(std::shared_ptr<SipMessage> data)
{
	auto session = getSession(data->getCallID());
	if (session.has_value())
	{
		auto sdpMessage = dynamic_cast<SipSdpMessage*>(data.get());
		auto client = findClient(data->getToNumber());
		if (!client.has_value()) return;
		session->get()->setDest(client.value(), sdpMessage->getRtpPort());
		session->get()->setState(Session::State::Connected);
	}

	auto response = data;
	if (data->getCSeq().find(SipMessageTypes::INVITE) != std::string::npos)
	{
		response->setContact("Contact: <sip:" + data->getToNumber() + "@" + _serverIp + ":" + std::to_string(_serverPort) + ";transport=UDP>");
	}
	endHandle(data->getFromNumber(), std::move(response));
}

void RequestsHandler::OnAck(std::shared_ptr<SipMessage> data)
{
	auto session = getSession(data->getCallID());
	if (!session.has_value()) 
	{
		return;
	}

	if (session.value()->getState() == Session::State::Connected)
	{
		endHandle(data->getToNumber(), data);
	}	
}

bool RequestsHandler::setCallState(const std::string& callID, Session::State state)
{
	auto session = getSession(callID);
	if (session)
	{
		session->get()->setState(state);
		return true;
	}

	return false;
}

void RequestsHandler::endCall(const std::string& callID, const std::string& srcNumber, const std::string& destNumber, const std::string& reason)
{
	if (_sessions.erase(callID) > 0)
	{
		std::ostringstream message;
		message << "Session has been disconnected between " << srcNumber << " and " << destNumber;
		if (!reason.empty())
		{
			message << " because " << reason;
		}
		std::cout << message.str() << std::endl;
	}
}

bool RequestsHandler::registerClient(std::shared_ptr<SipClient> client)
{
	if (_clients.find(client->getNumber()) == _clients.end()) {
		std::cout << "New Client: " << client->getNumber() << std::endl;
		_clients.emplace(client->getNumber(), client);
		return true;
	}
	return false;
}

void RequestsHandler::unregisterClient(std::shared_ptr<SipClient> client)
{
	std::cout << "unregister client: " << client->getNumber() << std::endl;
	_clients.erase(client->getNumber());
}

std::optional<std::shared_ptr<SipClient>> RequestsHandler::findClient(const std::string& number)
{
	auto it = _clients.find(number);
	if (it != _clients.end())
	{
		return it->second;
	}

	return {};
}

void RequestsHandler::endHandle(const std::string& destNumber, std::shared_ptr<SipMessage> message)
{	
	auto destClient = findClient(destNumber);
	if (destClient.has_value())
	{
		_onHandled(std::move(destClient.value()->getAddress()), std::move(message));
	}
	else
	{
		message->setHeader(SipMessageTypes::NOT_FOUND);
		auto src = message->getSource();
		_onHandled(src, std::move(message));
	}
}
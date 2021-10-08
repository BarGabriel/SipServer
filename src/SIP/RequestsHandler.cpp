#include "RequestsHandler.hpp"
#include <sstream>
#include "SipMessageTypes.h"
#include "SipSdpMessage.hpp"
#include "IDGen.hpp"

RequestsHandler::RequestsHandler(std::string serverIp, int serverPort, std::unordered_map<std::string, Session>& sessions,
	std::function<void(SipClient)> onNewClientEvent, std::function<void(SipClient)> onUnregisterEvent, OnHandledEvent onHandledEvent) :
	_serverIp(std::move(serverIp)), _serverPort(serverPort),
	_sessions(sessions),
	_onNewClient(onNewClientEvent),
	_onUnregister(onUnregisterEvent),
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
	else
	{
		std::cout << request->getType() << " not found in map handlers" << std::endl;
	}
}

std::optional<std::reference_wrapper<Session>> RequestsHandler::getSession(const std::string& callID) const
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
		SipClient newClient(data->getFromNumber(), data->getSource());
		_onNewClient(std::move(newClient));
	}

	auto response = data;
	response->setHeader(SipMessageTypes::OK);
	response->setVia(data->getVia() + ";received=" + _serverIp);
	response->setTo(data->getTo() + ";tag=" + IDGen::GenerateID(9));
	response->setContact("Contact: <sip:" + data->getFromNumber() + "@" + _serverIp + ":" + std::to_string(_serverPort) + ";transport=UDP>");
	_onHandled(response->getFromNumber(), response);

	if (isUnregister)
	{
		SipClient newClient(data->getFromNumber(), data->getSource());
		_onUnregister(std::move(newClient));
	}
}

void RequestsHandler::OnSubscribe(std::shared_ptr<SipMessage> data)
{
	std::ostringstream okResponse;
	okResponse << "SIP/2.0 200 OK\r\n"
		<< data->getVia() << ";received=" + _serverIp << "\r\n"
		<< data->getFrom() << "\r\n"
		<< data->getTo() << ";tag=" << IDGen::GenerateID(9) << "\r\n"//generete tag
		<< data->getCallID() << "\r\n"
		<< data->getCSeq() << "\r\n"
		<< data->getContact() << "\r\n"
		<< "Server: Sip server" << "\r\n"
		<< data->getContentLength() << "\r\n"
		<< "\r\n";

	_onHandled(data->getFromNumber(), std::make_shared<SipMessage>(SipMessage(okResponse.str(), {})));
}

void RequestsHandler::OnCancel(std::shared_ptr<SipMessage> data)
{
	endCall(data->getCallID(), data->getFromNumber(), data->getToNumber(), data->getFromNumber() + " canceled the session.");
	_onHandled(data->getToNumber(), data);
}

void RequestsHandler::onReqTerminated(std::shared_ptr<SipMessage> data)
{
	_onHandled(data->getFromNumber(), data);
}

void RequestsHandler::OnInvite(std::shared_ptr<SipMessage> data)
{
	auto messge = dynamic_cast<SipSdpMessage*>(data.get());
	SipClient client(data->getFromNumber(), data->getSource());
	Session newSession(data->getCallID(), client, messge->getRtpPort());
	_sessions.emplace(data->getCallID(), newSession);

	auto response = data;
	response->setContact("Contact: <sip:" + client.getNumber() + "@" + _serverIp + ":" + std::to_string(_serverPort) + ";transport=UDP>");
	_onHandled(data->getToNumber(), response);
}

void RequestsHandler::OnTrying(std::shared_ptr<SipMessage> data)
{
	setCallState(data->getCallID(), Session::State::Trying);
	_onHandled(data->getFromNumber(), data);
}

void RequestsHandler::OnRinging(std::shared_ptr<SipMessage> data)
{
	setCallState(data->getCallID(), Session::State::Ringing);
	_onHandled(data->getFromNumber(), data);
}

void RequestsHandler::OnBusy(std::shared_ptr<SipMessage> data)
{
	endCall(data->getCallID(), data->getFromNumber(), data->getToNumber(), data->getToNumber() + " is busy");
	_onHandled(data->getFromNumber(), data);
}

void RequestsHandler::OnUnavailable(std::shared_ptr<SipMessage> data)
{
	endCall(data->getCallID(), data->getFromNumber(), data->getToNumber(), data->getToNumber() + " is unavailable");
	_onHandled(data->getFromNumber(), data);
}

void RequestsHandler::OnBye(std::shared_ptr<SipMessage> data)
{
	endCall(data->getCallID(), data->getToNumber(), data->getFromNumber());
	_onHandled(data->getToNumber(), data);
}

void RequestsHandler::OnOk(std::shared_ptr<SipMessage> data)
{
	auto& session = getSession(data->getCallID());
	if (session)
	{
		auto sdpMessage = dynamic_cast<SipSdpMessage*>(data.get());
		SipClient client(data->getToNumber(), data->getSource());
		session->get().setDest(client, sdpMessage->getRtpPort());
		session->get().setState(Session::State::Connected);
	}


	auto response = data;
	if (data->getCSeq().find(SipMessageTypes::INVITE) != std::string::npos)
	{
		response->setContact("Contact: <sip:" + data->getToNumber() + "@" + _serverIp + ":" + std::to_string(_serverPort) + ";transport=UDP>");
	}
	_onHandled(data->getFromNumber(), std::move(response));
}

void RequestsHandler::OnAck(std::shared_ptr<SipMessage> data)
{
	_onHandled(data->getToNumber(), data);
}

bool RequestsHandler::setCallState(const std::string& callID, Session::State state)
{
	auto& session = getSession(callID);
	if (session)
	{
		session->get().setState(state);
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
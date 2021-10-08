#ifndef REQUESTS_HANDLER_HPP
#define REQUESTS_HANDLER_HPP

#include <functional>
#include <unordered_map>
#include <optional>
#include "SipMessage.hpp"
#include "SipClient.hpp"
#include "Session.hpp"

class RequestsHandler 
{
public:	

	using OnHandledEvent = std::function<void(std::string, std::shared_ptr<SipMessage>)>;

	RequestsHandler(std::string serverIp, int serverPort, std::unordered_map<std::string, Session>& sessions, 
		std::function<void(SipClient)> onNewClientEvent,
		std::function<void(SipClient)> onUnregisterEvent,
		OnHandledEvent onHandledEvent);

	void handle(std::shared_ptr<SipMessage> request);
	
	std::optional<std::reference_wrapper<Session>> getSession(const std::string& callID) const;

private:
	void initHandlers();

	void OnRegister(std::shared_ptr<SipMessage> data);
	void OnSubscribe(std::shared_ptr<SipMessage> data);
	void OnCancel(std::shared_ptr<SipMessage> data);
	void onReqTerminated(std::shared_ptr<SipMessage> data);
	void OnInvite(std::shared_ptr<SipMessage> data);
	void OnTrying(std::shared_ptr<SipMessage> data);
	void OnRinging(std::shared_ptr<SipMessage> data);
	void OnBusy(std::shared_ptr<SipMessage> data);
	void OnUnavailable(std::shared_ptr<SipMessage> data);
	void OnBye(std::shared_ptr<SipMessage> data);
	void OnOk(std::shared_ptr<SipMessage> data);
	void OnAck(std::shared_ptr<SipMessage> data);

	bool setCallState(const std::string& callID, Session::State state);
	void endCall(const std::string& callID, const std::string& srcNumber, const std::string& destNumber, const std::string& reason = "");

	std::unordered_map<std::string, std::function<void(std::shared_ptr<SipMessage> request)>> _handlers;
	std::unordered_map<std::string, Session>& _sessions;

	std::function<void(SipClient)> _onNewClient;
	std::function<void(SipClient)> _onUnregister;
	OnHandledEvent _onHandled;

	std::string _serverIp;
	int _serverPort;
};

#endif
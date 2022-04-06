#ifndef SESSION_HPP
#define SESSION_HPP

#include "SipClient.hpp"
#include "UdpServer.hpp"

class Session
{
public:

	enum class State
	{
		Invited,
		Trying,
		Ringing,
		Connected
	};


	Session(std::string callID, std::shared_ptr<SipClient> src, size_t srcRtpPort);
	
	void setState(State state);
	void setDest(std::shared_ptr<SipClient> dest, size_t destRtpPort);

	std::string getCallID() const;
	std::shared_ptr<SipClient> getSrc() const;
	std::shared_ptr<SipClient> getDest() const;

private:
	std::string _callID;
	std::shared_ptr<SipClient> _src;
	std::shared_ptr<SipClient> _dest;
	State _state;

	int _srcRtpPort;
	int _destRtpPort;	
};

#endif
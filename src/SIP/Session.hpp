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


	Session(std::string callID, SipClient src, size_t srcRtpPort);
	
	void setState(State state);
	void setDest(SipClient dest, size_t destRtpPort);

	std::string getCallID() const;
	SipClient getSrc() const;
	SipClient getDest() const;

private:
	std::string _callID;
	SipClient _src;
	SipClient _dest;
	State _state;

	int _srcRtpPort;
	int _destRtpPort;	
};

#endif
#include "Session.hpp"

Session::Session(std::string callID, SipClient src, size_t srcRtpPort) :
	_callID(std::move(callID)), _src(src), _state(State::Invited), _srcRtpPort(srcRtpPort), _destRtpPort(0)
{
}

void Session::setState(State state)
{
	if (state == _state)
		return;
	_state = state;
	std::cout << "New Session state: " << static_cast<int>(_state) << std::endl;
	if (state == State::Connected)
	{
		std::cout << "Session Created between " << _src.getNumber() << " and " << _dest.getNumber() << std::endl;		
	}
}

void Session::setDest(SipClient dest, size_t destRtpPort)
{
	_dest = dest;
	_destRtpPort = destRtpPort;
}

std::string Session::getCallID() const
{
	return _callID;
}

SipClient Session::getSrc() const
{
	return _src;
}

SipClient Session::getDest() const
{
	return _dest;
}
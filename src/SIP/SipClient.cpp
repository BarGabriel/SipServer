#include "SipClient.hpp"

SipClient::SipClient(std::string number, sockaddr_in address) : _number(std::move(number)), _address(std::move(address))
{
}

bool SipClient::operator==(SipClient other)
{
	if (_number == other.getNumber()) 
	{
		return true;
	}

	return false;
}

std::string SipClient::getNumber() const
{
	return _number;
}

sockaddr_in SipClient::getAddress() const
{
	return _address;
}
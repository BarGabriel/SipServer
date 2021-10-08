#ifndef SIP_CLIENT_HPP
#define SIP_CLIENT_HPP

#include <WinSock2.h>
#include <iostream>

class SipClient
{
public:
	SipClient() = default;
	SipClient(std::string number, sockaddr_in address);


	bool operator==(SipClient other);

	std::string getNumber() const;
	sockaddr_in getAddress() const;


private:
	std::string _number;
	sockaddr_in _address;
};

#endif
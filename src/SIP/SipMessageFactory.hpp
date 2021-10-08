#ifndef SIP_MESSAGE_FACTORY_HPP
#define SIP_MESSAGE_FACTORY_HPP

#include <optional>
#include "SipSdpMessage.hpp"

class SipMessageFactory
{
public:
	std::optional<std::shared_ptr<SipMessage>> createMessage(std::string message, sockaddr_in src);

};

#endif 
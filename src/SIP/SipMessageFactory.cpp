#include "SipMessageFactory.hpp"

std::optional<std::shared_ptr<SipMessage>> SipMessageFactory::createMessage(std::string message, sockaddr_in src)
{
	try
	{
		if (message.find("application/sdp") != std::string::npos)
		{
			return std::make_shared<SipSdpMessage>(std::move(message), std::move(src));
		}

		return std::make_shared<SipMessage>(std::move(message), std::move(src));
	}
	catch (const std::exception&)
	{
		return {};
	}
}
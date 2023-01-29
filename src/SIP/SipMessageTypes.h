#ifndef SIP_MESSAGE_TYPES_H
#define SIP_MESSAGE_TYPES_H

class SipMessageTypes
{
public:
	SipMessageTypes() = delete;

	static constexpr auto REGISTER           = "REGISTER";
	static constexpr auto INVITE             = "INVITE";
	static constexpr auto CANCEL             = "CANCEL";
	static constexpr auto REQUEST_TERMINATED = "SIP/2.0 487 Request Terminated";
	static constexpr auto TRYING             = "SIP/2.0 100 Trying";
	static constexpr auto RINGING            = "SIP/2.0 180 Ringing";
	static constexpr auto BUSY               = "SIP/2.0 486 Busy Here";
	static constexpr auto UNAVAIALBLE        = "SIP/2.0 480 Temporarily Unavailable";
	static constexpr auto OK                 = "SIP/2.0 200 OK";
	static constexpr auto ACK                = "ACK";
	static constexpr auto BYE                = "BYE";
	static constexpr auto NOT_FOUND          = "SIP/2.0 404 Not Found";
};

#endif
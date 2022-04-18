#include "SipSdpMessage.hpp"
#include "SipMessageHeaders.h"
#include <string>
#include <cstring>

SipSdpMessage::SipSdpMessage(std::string message, sockaddr_in src) : SipMessage(std::move(message), std::move(src))
{
	parse();
}

void SipSdpMessage::setMedia(std::string value)
{
	auto mPos = _messageStr.find(_media);
	_messageStr.replace(mPos, _media.length(), value);
	_media = value;
}

void SipSdpMessage::setRtpPort(int port)
{
	std::string currentRtpPort = std::to_string(_rtpPort);
	std::string copyM = _media;
	copyM.replace(_media.find(currentRtpPort), currentRtpPort.length(), std::to_string(port));
	_rtpPort = port;
	setMedia(std::move(copyM));
}

std::string SipSdpMessage::getVersion() const
{
	return _version;
}

std::string SipSdpMessage::getOriginator() const
{
	return _originator;
}

std::string SipSdpMessage::getSessionName() const
{
	return _sessionName;
}

std::string SipSdpMessage::getConnectionInformation() const
{
	return _connectionInformation;
}

std::string SipSdpMessage::getTime() const
{
	return _time;
}

std::string SipSdpMessage::getMedia() const
{
	return _media;
}

int SipSdpMessage::getRtpPort() const
{
	return _rtpPort;
}

void SipSdpMessage::parse()
{
	std::string msg = _messageStr;

	auto posOfM = msg.find("v=");
	msg.erase(0, posOfM);
	size_t pos = 0;
	while ((pos = msg.find(SipMessageHeaders::HEADERS_DELIMETER)) != std::string::npos)
	{
		std::string line = msg.substr(0, pos);
		if (line.find("v=") != std::string::npos)
		{
			_version = std::move(line);
		}
		else if (line.find("o=") != std::string::npos)
		{
			_originator = std::move(line);
		}
		else if (line.find("s=") != std::string::npos)
		{
			_sessionName = std::move(line);
		}
		else if (line.find("c=") != std::string::npos)
		{
			_connectionInformation = std::move(line);
		}
		else if (line.find("t=") != std::string::npos)
		{
			_time = std::move(line);
		}
		else if (line.find("m=") != std::string::npos)
		{
			_media = line;
			_rtpPort = extractRtpPort(std::move(line));
		}
		msg.erase(0, pos + std::strlen(SipMessageHeaders::HEADERS_DELIMETER));
	}
}

int SipSdpMessage::extractRtpPort(std::string data) const
{
	data.erase(0, data.find(" ") + 1);
	std::string portStr = data.substr(0, data.find(" "));
	return std::stoi(portStr);
}
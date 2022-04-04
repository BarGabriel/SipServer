#include <iostream>
#include "SipServer.hpp"
#include "cxxopts.hpp"

int main(int argc, char** argv)
{
	cxxopts::Options options("SipServer", "Open source server for handling voip calls based on sip.");

	options.add_options()
		("h,help", "Print usage")
		("i,ip", "Sip server ip", cxxopts::value<std::string>())
		("p,port", "Sip server ip.", cxxopts::value<int>()->default_value(std::to_string(5060)));

	auto result = options.parse(argc, argv);

	if (result.count("help"))
	{
		std::cout << options.help() << std::endl;
		exit(0);
	}

	try
	{
		std::string ip = result["ip"].as<std::string>();
		int port = result["port"].as<int>();
		SipServer server(std::move(ip), port);
		std::cout << "Server has been started. Listening..." << std::endl;
		getchar();
	}
	catch (const cxxopts::OptionException&)
	{
		std::cout << "Please enter ip and port." << std::endl;
	}
	return 0;
}
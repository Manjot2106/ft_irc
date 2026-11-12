#include "IRC.hpp"

bool isValidPort(const std::string &value, int &port)
{
	char *end;
	long parsed;
	if (value.empty())
		return (false);
	parsed = std::strtol(value.c_str(), &end, 10);
	if (*end != '\0' || parsed < 1 || parsed > 65535)
		return (false);
	port = static_cast<int>(parsed);
	return (true);
}

bool isValidNickname(const std::string &nickname)
{
	if (nickname.empty() || nickname.size() > 30)
		return (false);
	unsigned char first = static_cast<unsigned char>(nickname[0]);
	if (!std::isalpha(first) && std::string("[]\\`_^{|}").find(nickname[0]) == std::string::npos)
		return (false);
	for (std::size_t i = 1; i < nickname.size(); ++i)
	{
		unsigned char c = static_cast<unsigned char>(nickname[i]);
		if (!std::isalnum(c) && std::string("-[]\\`_^{|}").find(nickname[i]) == std::string::npos)
			return (false);
	}
	return (true);
}

std::string toUpper(const std::string &value)
{
	std::string result(value);
	for (std::size_t i = 0; i < result.size(); ++i)
		result[i] = static_cast<char>(std::toupper(static_cast<unsigned char>(result[i])));
	return (result);
}

#include "IRC.hpp"

static bool parsePositiveLimit(const std::string &value, std::size_t &limit)
{
	if (value.empty()) return (false);
	char *end = 0;
	long n = std::strtol(value.c_str(), &end, 10);
	if (*end != '\0' || n <= 0) return (false);
	limit = static_cast<std::size_t>(n);
	return (true);
}

void Server::handleMODE(Client &client, const std::string &arguments)
{
	std::istringstream iss(arguments);
	std::string channelName;
	if (!(iss >> channelName))
	{
		sendMessage(client.getFd(), ":irc.local 461 " + client.getNickname() + " MODE :Not enough parameters\r\n");
		return;
	}
	Channel *channel = findChannel(channelName);
	if (!channel)
	{
		sendMessage(client.getFd(), ":irc.local 403 " + client.getNickname() + " " + channelName + " :No such channel\r\n");
		return;
	}
	std::string modes;
	if (!(iss >> modes))
	{
		std::string replyModes = channel->modeString();
		if (replyModes.empty()) replyModes = "+";
		std::string extras;
		if (channel->hasKey()) extras += " " + channel->getKey();
		if (channel->hasLimit())
		{
			std::ostringstream out;
			out << channel->getLimit();
			extras += " " + out.str();
		}
		sendMessage(client.getFd(), ":irc.local 324 " + client.getNickname() + " " + channel->getName() + " " + replyModes + extras + "\r\n");
		return;
	}
	if (!channel->hasMember(client))
	{
		sendMessage(client.getFd(), ":irc.local 442 " + client.getNickname() + " " + channel->getName() + " :You're not on that channel\r\n");
		return;
	}
	if (!channel->isOperator(client))
	{
		sendMessage(client.getFd(), ":irc.local 482 " + client.getNickname() + " " + channel->getName() + " :You're not channel operator\r\n");
		return;
	}

	std::vector<std::string> params;
	std::string param;
	while (iss >> param) params.push_back(param);
	std::size_t p = 0;
	bool adding = true;
	std::string applied;
	std::string appliedParams;
	char lastSign = 0;

	for (std::size_t i = 0; i < modes.size(); ++i)
	{
		char mode = modes[i];
		if (mode == '+') { adding = true; continue; }
		if (mode == '-') { adding = false; continue; }
		bool changed = false;
		std::string usedParam;

		if (mode == 'i')
		{
			channel->setInviteOnly(adding);
			changed = true;
		}
		else if (mode == 't')
		{
			channel->setTopicRestricted(adding);
			changed = true;
		}
		else if (mode == 'k')
		{
			if (adding)
			{
				if (p >= params.size())
				{
					sendMessage(client.getFd(), ":irc.local 461 " + client.getNickname() + " MODE :Not enough parameters\r\n");
					continue;
				}
				usedParam = params[p++];
				channel->setKey(usedParam);
				changed = true;
			}
			else
			{
				channel->clearKey();
				changed = true;
			}
		}
		else if (mode == 'l')
		{
			if (adding)
			{
				if (p >= params.size())
				{
					sendMessage(client.getFd(), ":irc.local 461 " + client.getNickname() + " MODE :Not enough parameters\r\n");
					continue;
				}
				std::size_t limit;
				usedParam = params[p++];
				if (!parsePositiveLimit(usedParam, limit))
					continue;
				channel->setLimit(limit);
				changed = true;
			}
			else
			{
				channel->clearLimit();
				changed = true;
			}
		}
		else if (mode == 'o')
		{
			if (p >= params.size())
			{
				sendMessage(client.getFd(), ":irc.local 461 " + client.getNickname() + " MODE :Not enough parameters\r\n");
				continue;
			}
			usedParam = params[p++];
			Client *target = findClientByNickname(usedParam);
			if (!target)
			{
				sendMessage(client.getFd(), ":irc.local 401 " + client.getNickname() + " " + usedParam + " :No such nick/channel\r\n");
				continue;
			}
			if (!channel->hasMember(*target))
			{
				sendMessage(client.getFd(), ":irc.local 441 " + client.getNickname() + " " + target->getNickname() + " " + channel->getName() + " :They aren't on that channel\r\n");
				continue;
			}
			if (adding) channel->addOperator(*target);
			else channel->removeOperator(*target);
			usedParam = target->getNickname();
			changed = true;
		}
		else
		{
			sendMessage(client.getFd(), ":irc.local 472 " + client.getNickname() + " " + std::string(1, mode) + " :is unknown mode char to me\r\n");
			continue;
		}

		if (changed)
		{
			char sign = adding ? '+' : '-';
			if (sign != lastSign)
			{
				applied += sign;
				lastSign = sign;
			}
			applied += mode;
			if (!usedParam.empty()) appliedParams += " " + usedParam;
		}
	}

	if (!applied.empty())
		channel->broadcast(*this, clientPrefix(client) + " MODE " + channel->getName() + " " + applied + appliedParams + "\r\n", 0);
}

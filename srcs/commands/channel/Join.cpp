#include "IRC.hpp"

static std::vector<std::string> splitCommaJoin(const std::string &value)
{
	std::vector<std::string> result;
	std::string::size_type start = 0;
	while (start <= value.size())
	{
		std::string::size_type end = value.find(',', start);
		result.push_back(value.substr(start, end == std::string::npos ? std::string::npos : end - start));
		if (end == std::string::npos)
			break;
		start = end + 1;
	}
	return (result);
}

void Server::handleJOIN(Client &client, const std::string &arguments)
{
	std::istringstream iss(arguments);
	std::string channelsArg;
	std::string keysArg;
	iss >> channelsArg >> keysArg;
	if (channelsArg.empty())
	{
		sendMessage(client.getFd(), ":irc.local 461 " + client.getNickname() + " JOIN :Not enough parameters\r\n");
		return;
	}

	std::vector<std::string> names = splitCommaJoin(channelsArg);
	std::vector<std::string> keys = splitCommaJoin(keysArg);
	for (std::size_t i = 0; i < names.size(); ++i)
	{
		const std::string &name = names[i];
		if (name.empty() || (name[0] != '#' && name[0] != '&') || name.find(' ') != std::string::npos || name.find(',') != std::string::npos || name.find('\a') != std::string::npos)
		{
			sendMessage(client.getFd(), ":irc.local 403 " + client.getNickname() + " " + name + " :No such channel\r\n");
			continue;
		}

		Channel *channel = findChannel(name);
		bool created = false;
		if (!channel)
		{
			channel = new Channel(name);
			_channels[toUpper(name)] = channel;
			created = true;
		}
		if (channel->hasMember(client))
			continue;
		if (channel->isInviteOnly() && !channel->isInvited(client))
		{
			sendMessage(client.getFd(), ":irc.local 473 " + client.getNickname() + " " + channel->getName() + " :Cannot join channel (+i)\r\n");
			if (created) removeChannelIfEmpty(channel);
			continue;
		}
		if (channel->hasKey() && (i >= keys.size() || keys[i] != channel->getKey()))
		{
			sendMessage(client.getFd(), ":irc.local 475 " + client.getNickname() + " " + channel->getName() + " :Cannot join channel (+k)\r\n");
			if (created) removeChannelIfEmpty(channel);
			continue;
		}
		if (channel->hasLimit() && channel->memberCount() >= channel->getLimit())
		{
			sendMessage(client.getFd(), ":irc.local 471 " + client.getNickname() + " " + channel->getName() + " :Cannot join channel (+l)\r\n");
			if (created) removeChannelIfEmpty(channel);
			continue;
		}

		channel->addMember(client);
		channel->removeInvite(client);
	    if (created)
			channel->addOperator(client);
		channel->broadcast(*this, clientPrefix(client) + " JOIN :" + channel->getName() + "\r\n", 0);

		if (channel->getTopic().empty())
			sendMessage(client.getFd(), ":irc.local 331 " + client.getNickname() + " " + channel->getName() + " :No topic is set\r\n");
		else
			sendMessage(client.getFd(), ":irc.local 332 " + client.getNickname() + " " + channel->getName() + " :" + channel->getTopic() + "\r\n");

		sendNamesReply(client, *channel);
	}
}

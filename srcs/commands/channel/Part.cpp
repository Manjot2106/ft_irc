#include "IRC.hpp"

static std::vector<std::string> splitPartChannels(const std::string &value)
{
	std::vector<std::string> channels;
	std::string::size_type start = 0;

	while (start <= value.size())
	{
		std::string::size_type end = value.find(',', start);
		channels.push_back(value.substr(start,
			end == std::string::npos ? std::string::npos : end - start));
		if (end == std::string::npos)
			break;
		start = end + 1;
	}
	return (channels);
}

void Server::handlePART(Client &client, const std::string &arguments)
{
	Command cmd = parseCommand("PART " + arguments);

	if (cmd.params.empty())
	{
		sendMessage(client.getFd(), ":irc.local 461 " + client.getNickname()
			+ " PART :Not enough parameters\r\n");
		return;
	}

	std::vector<std::string> channels = splitPartChannels(cmd.params[0]);
	std::string reason = client.getNickname();
	if (cmd.params.size() > 1)
		reason = cmd.params[1];

	for (std::size_t i = 0; i < channels.size(); ++i)
	{
		Channel *channel = findChannel(channels[i]);
		if (!channel)
		{
			sendMessage(client.getFd(), ":irc.local 403 " + client.getNickname()
				+ " " + channels[i] + " :No such channel\r\n");
			continue;
		}
		if (!channel->hasMember(client))
		{
			sendMessage(client.getFd(), ":irc.local 442 " + client.getNickname()
				+ " " + channel->getName() + " :You're not on that channel\r\n");
			continue;
		}

		std::string message = clientPrefix(client) + " PART " + channel->getName()
			+ " :" + reason + "\r\n";
		channel->broadcast(*this, message, 0);
		channel->removeMember(client);
		removeChannelIfEmpty(channel);
	}
}

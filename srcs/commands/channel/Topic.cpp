#include "IRC.hpp"

void Server::handleTopic(Client &client, const Command &cmd)
{
	if (cmd.params.empty())
	{
		sendMessage(client.getFd(), ":irc.local 461 " + client.getNickname()
			+ " TOPIC :Not enough parameters\r\n");
		return;
	}
	const std::string &channelName = cmd.params[0];
	Channel *channel = findChannel(channelName);
	if (!channel)
	{
		sendMessage(client.getFd(), ":irc.local 403 " + client.getNickname() + " "
			+ channelName + " :No such channel\r\n");
		return;
	}
	if (!channel->hasMember(client))
	{
		sendMessage(client.getFd(), ":irc.local 442 " + client.getNickname() + " "
			+ channelName + " :You're not on that channel\r\n");
		return;
	}
	if (cmd.params.size() == 1)
	{
		if (channel->getTopic().empty())
			sendMessage(client.getFd(), ":irc.local 331 " + client.getNickname() + " "
				+ channelName + " :No topic is set\r\n");
		else
			sendMessage(client.getFd(), ":irc.local 332 " + client.getNickname() + " "
				+ channelName + " :" + channel->getTopic() + "\r\n");
		return;
	}
	if (channel->isTopicRestricted() && !channel->isOperator(client))
	{
		sendMessage(client.getFd(), ":irc.local 482 " + client.getNickname() + " "
			+ channelName + " :You're not channel operator\r\n");
		return;
	}
	const std::string &newTopic = cmd.params[1];
	channel->setTopic(newTopic);
	channel->broadcast(*this, clientPrefix(client) + " TOPIC " + channelName
		+ " :" + newTopic + "\r\n", 0);
}

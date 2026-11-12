#include "IRC.hpp"

void Server::handleKick(Client &client, const Command &cmd)
{
	if (cmd.params.size() < 2)
	{
		sendMessage(client.getFd(), ":irc.local 461 " + client.getNickname()
			+ " KICK :Not enough parameters\r\n");
		return;
	}
	const std::string &channelName = cmd.params[0];
	const std::string &nickname = cmd.params[1];
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
	if (!channel->isOperator(client))
	{
		sendMessage(client.getFd(), ":irc.local 482 " + client.getNickname() + " "
			+ channelName + " :You're not channel operator\r\n");
		return;
	}
	Client *targetClient = findClientByNickname(nickname);
	if (!targetClient || !channel->hasMember(*targetClient))
	{
		sendMessage(client.getFd(), ":irc.local 441 " + client.getNickname() + " "
			+ nickname + " " + channelName + " :They aren't on that channel\r\n");
		return;
	}
	std::string reason = client.getNickname();
	if (cmd.params.size() > 2)
		reason = cmd.params[2];
	std::string message = clientPrefix(client) + " KICK " + channelName + " "
		+ nickname + " :" + reason + "\r\n";
	channel->broadcast(*this, message, 0);
	channel->removeMember(*targetClient);
	removeChannelIfEmpty(channel);
}

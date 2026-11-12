#include "IRC.hpp"

void Server::handleInvite(Client &client, const Command &cmd)
{
	if (cmd.params.size() < 2)
	{
		sendMessage(client.getFd(), ":irc.local 461 " + client.getNickname()
			+ " INVITE :Not enough parameters\r\n");
		return;
	}
	const std::string &nickname = cmd.params[0];
	const std::string &channelName = cmd.params[1];
	Client *targetClient = findClientByNickname(nickname);
	if (!targetClient)
	{
		sendMessage(client.getFd(), ":irc.local 401 " + client.getNickname() + " "
			+ nickname + " :No such nick\r\n");
		return;
	}
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
	if (channel->hasMember(*targetClient))
	{
		sendMessage(client.getFd(), ":irc.local 443 " + client.getNickname() + " "
			+ nickname + " " + channelName + " :is already on channel\r\n");
		return;
	}
	channel->invite(*targetClient);
	sendMessage(client.getFd(), ":irc.local 341 " + client.getNickname() + " "
		+ nickname + " " + channelName + "\r\n");
	sendMessage(targetClient->getFd(), clientPrefix(client) + " INVITE " + nickname
		+ " :" + channelName + "\r\n");
}

#include "IRC.hpp"

void Server::sendNamesReply(Client &client, Channel &channel)
{
	std::string list;
	const std::vector<Client *> &members = channel.getMembers();
	for (std::size_t j = 0; j < members.size(); ++j)
	{
		if (!list.empty()) list += " ";
		if (channel.isOperator(*members[j])) list += "@";
		list += members[j]->getNickname();
	}
	sendMessage(client.getFd(), ":irc.local 353 " + client.getNickname() + " = " + channel.getName() + " :" + list + "\r\n");
	sendMessage(client.getFd(), ":irc.local 366 " + client.getNickname() + " " + channel.getName() + " :End of /NAMES list\r\n");
}

void Server::handleNAMES(Client &client, const std::string &arguments)
{
	std::istringstream iss(arguments);
	std::string channelName;

	if (!(iss >> channelName))
	{
		sendMessage(client.getFd(), ":irc.local 366 " + client.getNickname() + " * :End of /NAMES list\r\n");
		return;
	}
	Channel *channel = findChannel(channelName);
	if (!channel)
	{
		sendMessage(client.getFd(), ":irc.local 403 " + client.getNickname() + " " + channelName + " :No such channel\r\n");
		return;
	}
	sendNamesReply(client, *channel);
}

#include "IRC.hpp"

static void sendPrivmsgUser(Server &server, Client &client,
	const std::string &target, const std::string &message)
{
	Client *targetClient = server.findClientByNickname(target);
	if (!targetClient)
	{
		server.sendMessage(client.getFd(), ":irc.local 401 " + client.getNickname()
			+ " " + target + " :No such nick\r\n");
		return;
	}
	server.sendMessage(targetClient->getFd(), server.clientPrefix(client) + " PRIVMSG "
		+ target + " :" + message + "\r\n");
}

static void sendPrivmsgChannel(Server &server, Client &client,
	const std::string &target, const std::string &message)
{
	Channel *channel = server.findChannel(target);
	if (!channel)
	{
		server.sendMessage(client.getFd(), ":irc.local 403 " + client.getNickname()
			+ " " + target + " :No such channel\r\n");
		return;
	}
	if (!channel->hasMember(client))
	{
		server.sendMessage(client.getFd(), ":irc.local 404 " + client.getNickname()
			+ " " + target + " :Cannot send to channel\r\n");
		return;
	}
	channel->broadcast(server, server.clientPrefix(client) + " PRIVMSG "
		+ channel->getName() + " :" + message + "\r\n", &client);
	server.handleGuessChannel(client, *channel, message);
}

void Server::handlePRIVMSG(Client &client, const std::string &arguments)
{
	Command cmd = parseCommand("PRIVMSG " + arguments);

	if (cmd.params.empty())
	{
		sendMessage(client.getFd(), ":irc.local 411 " + client.getNickname()
			+ " :No recipient given (PRIVMSG)\r\n");
		return;
	}
	if (cmd.params.size() < 2 || cmd.params[1].empty())
	{
		sendMessage(client.getFd(), ":irc.local 412 " + client.getNickname()
			+ " :No text to send\r\n");
		return;
	}
	const std::string &target = cmd.params[0];
	const std::string &message = cmd.params[1];
	if (!target.empty() && (target[0] == '#' || target[0] == '&'))
		sendPrivmsgChannel(*this, client, target, message);
	else
		sendPrivmsgUser(*this, client, target, message);
}

static void sendNoticeUser(Server &server, Client &client,
	const std::string &target, const std::string &message)
{
	Client *targetClient = server.findClientByNickname(target);
	if (!targetClient)
		return;
	server.sendMessage(targetClient->getFd(), server.clientPrefix(client) + " NOTICE "
		+ targetClient->getNickname() + " :" + message + "\r\n");
}

static void sendNoticeChannel(Server &server, Client &client,
	const std::string &target, const std::string &message)
{
	Channel *channel = server.findChannel(target);
	if (!channel || !channel->hasMember(client))
		return;
	channel->broadcast(server, server.clientPrefix(client) + " NOTICE "
		+ channel->getName() + " :" + message + "\r\n", &client);
}

void Server::handleNOTICE(Client &client, const std::string &arguments)
{
	Command cmd = parseCommand("NOTICE " + arguments);

	if (cmd.params.empty() || cmd.params.size() < 2 || cmd.params[1].empty())
		return;
	const std::string &target = cmd.params[0];
	const std::string &message = cmd.params[1];
	if (!target.empty() && (target[0] == '#' || target[0] == '&'))
		sendNoticeChannel(*this, client, target, message);
	else
		sendNoticeUser(*this, client, target, message);
}

#include "IRC.hpp"

Server::Server(int port, const std::string &password)
	: _port(port), _listenFd(-1), _password(password)
{
	std::srand(static_cast<unsigned int>(std::time(0)));
}

Server::~Server()
{
	for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		close(it->first);
		delete it->second;
	}
	for (std::map<std::string, Channel *>::iterator it = _channels.begin(); it != _channels.end(); ++it)
		delete it->second;
	if (_listenFd >= 0)
		close(_listenFd);
}

void Server::checkRegistration(Client &client)
{
	if (client.isRegistered() && !client.hasBeenWelcomed())
	{
		sendMessage(client.getFd(), ":irc.local 001 " + client.getNickname() + " :Welcome to the IRC network " + client.getNickname() + "\r\n");
		sendMessage(client.getFd(), ":irc.local 002 " + client.getNickname() + " :Your host is irc.local\r\n");
		sendMessage(client.getFd(), ":irc.local 003 " + client.getNickname() + " :This server was created for ft_irc\r\n");
		sendMessage(client.getFd(), ":irc.local 004 " + client.getNickname() + " irc.local ft_irc 0.1 o itkol\r\n");
		client.setWelcomed(true);
	}
}

Client *Server::findClientByNickname(const std::string &nickname) const
{
	const std::string wanted = toUpper(nickname);
	for (std::map<int, Client *>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (toUpper(it->second->getNickname()) == wanted)
			return (it->second);
	}
	return (0);
}

Channel *Server::findChannel(const std::string &name) const
{
	std::map<std::string, Channel *>::const_iterator it = _channels.find(toUpper(name));
	if (it == _channels.end())
		return (0);
	return (it->second);
}

void Server::removeChannelIfEmpty(Channel *channel)
{
	if (!channel || !channel->empty())
		return;
	std::map<std::string, Channel *>::iterator it = _channels.find(toUpper(channel->getName()));
	if (it != _channels.end())
	{
		delete it->second;
		_channels.erase(it);
	}
}

std::string Server::clientPrefix(const Client &client) const
{
	std::string nick = client.getNickname().empty() ? "*" : client.getNickname();
	std::string user = client.getUsername().empty() ? "unknown" : client.getUsername();
	return (":" + nick + "!" + user + "@irc.local");
}

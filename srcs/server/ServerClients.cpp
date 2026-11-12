#include "IRC.hpp"

void Server::acceptClient()
{
	int fd = accept(_listenFd, 0, 0);
	if (fd < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
			return;
		return;
	}

	if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
	{
		close(fd);
		return;
	}

	struct pollfd entry;
	entry.fd = fd;
	entry.events = POLLIN;
	entry.revents = 0;
	_pollFds.push_back(entry);
	_clients[fd] = new Client(fd);
	std::cout << "Client connected on fd " << fd << std::endl;
	sendMessage(fd, ":irc.local NOTICE * :Connected. Authenticate with PASS, then set NICK and USER\r\n");
}

void Server::disconnectClient(int fd, const std::string &reason)
{
	std::map<int, Client *>::iterator clientIt = _clients.find(fd);
	if (clientIt != _clients.end())
	{
		Client *client = clientIt->second;
		std::string quitMessage = clientPrefix(*client) + " QUIT :" + reason + "\r\n";
		std::set<Client *> notified;
		std::vector<Channel *> touched;
		for (std::map<std::string, Channel *>::iterator it = _channels.begin(); it != _channels.end(); ++it)
		{
			if (it->second->hasMember(*client))
			{
				const std::vector<Client *> &members = it->second->getMembers();
				for (std::vector<Client *>::const_iterator mit = members.begin(); mit != members.end(); ++mit)
				{
					if (*mit != client && notified.insert(*mit).second)
						sendMessage((*mit)->getFd(), quitMessage);
				}
				it->second->removeMember(*client);
				touched.push_back(it->second);
			}
			else
				it->second->removeInvite(*client);
		}
		std::string nick = client->getNickname().empty() ? "*" : client->getNickname();
		std::cout << "Client disconnected: " << nick << std::endl;
		delete client;
		_clients.erase(clientIt);

		for (std::size_t i = 0; i < touched.size(); ++i)
			removeChannelIfEmpty(touched[i]);
	}

	close(fd);
	for (std::vector<struct pollfd>::iterator it = _pollFds.begin(); it != _pollFds.end(); ++it)
	{
		if (it->fd == fd)
		{
			_pollFds.erase(it);
			break;
		}
	}
}

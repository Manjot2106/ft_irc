#include "IRC.hpp"

void Server::updatePollEvents(int fd)
{
	std::map<int, Client *>::iterator clientIt = _clients.find(fd);
	if (clientIt == _clients.end())
		return;
	for (std::vector<struct pollfd>::iterator it = _pollFds.begin(); it != _pollFds.end(); ++it)
	{
		if (it->fd == fd)
		{
			it->events = POLLIN;
			if (clientIt->second->hasOutput())
				it->events |= POLLOUT;
			return;
		}
	}
}

void Server::sendMessage(int fd, const std::string &message)
{
	std::map<int, Client *>::iterator it = _clients.find(fd);
	if (it == _clients.end())
		return;
	it->second->queueOutput(message);
	updatePollEvents(fd);
}

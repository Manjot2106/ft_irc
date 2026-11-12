#include "IRC.hpp"

void Server::run()
{
	while (true)
	{
		int ready = poll(&_pollFds[0], _pollFds.size(), -1);
		if (ready < 0)
		{
			if (errno == EINTR)
				continue;
			throw std::runtime_error(std::string("poll failed: ") + std::strerror(errno));
		}

		for (std::size_t i = 0; i < _pollFds.size();)
		{
			short returnevents = _pollFds[i].revents;
			if (returnevents == 0)
			{
				++i;
				continue;
			}

			int fd = _pollFds[i].fd;
			bool removed = false;
			if (returnevents & (POLLERR | POLLHUP | POLLNVAL))
			{
				if (fd == _listenFd)
					throw std::runtime_error("listening socket failed");
				disconnectClient(fd);
				removed = true;
			}
			else if (fd == _listenFd && (returnevents & POLLIN))
				acceptClient();
			else
			{
				if (returnevents & POLLIN)
				{
					readFromClient(fd);
					if (_clients.find(fd) == _clients.end())
						removed = true;
				}
				if (!removed && (returnevents & POLLOUT))
				{
					writeToClient(fd);
					if (_clients.find(fd) == _clients.end())
						removed = true;
				}
			}
			if (!removed)
				++i;
		}
	}
}

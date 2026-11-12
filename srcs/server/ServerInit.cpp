#include "IRC.hpp"

void Server::initialize()
{
	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		throw std::runtime_error("signal(SIGPIPE) failed");
	int enabled = 1;
	struct sockaddr_in address;
	struct pollfd listener;

	_listenFd = socket(IPV4, TCP, 0);
	if (_listenFd < 0)
		throw std::runtime_error(std::strerror(errno));
	if (setsockopt(_listenFd, SOL_SOCKET, SO_REUSEADDR, &enabled, sizeof(enabled)) < 0)
		throw std::runtime_error(std::strerror(errno));
	if (fcntl(_listenFd, F_SETFL, O_NONBLOCK) < 0)
		throw std::runtime_error(std::strerror(errno));

	std::memset(&address, 0, sizeof(address));
	address.sin_family = IPV4;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(static_cast<unsigned short>(_port));
	if (bind(_listenFd, reinterpret_cast<struct sockaddr *>(&address), sizeof(address)) < 0)
		throw std::runtime_error(std::strerror(errno));
	if (listen(_listenFd, SOMAXCONN) < 0)
		throw std::runtime_error(std::strerror(errno));

	listener.fd = _listenFd;
	listener.events = POLLIN;
	listener.revents = 0;
	_pollFds.push_back(listener);
}

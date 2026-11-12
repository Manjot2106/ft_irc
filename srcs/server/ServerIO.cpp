#include "IRC.hpp"

void Server::readFromClient(int fd)
{
	char buffer[4096];
	ssize_t received = recv(fd, buffer, sizeof(buffer), 0);

	if (received <= 0)
	{
		if (received < 0)
			std::cerr << "[IO] fd=" << fd << " recv failed after POLLIN" << std::endl;
		disconnectClient(fd);
		return;
	}

	std::map<int, Client *>::iterator it = _clients.find(fd);
	if (it == _clients.end())
		return;
	Client *client = it->second;
	client->appendInput(buffer, static_cast<std::size_t>(received));
	if (client->inputTooLong())
	{
		std::cerr << "[PROTO] fd=" << fd << " rejected: IRC line exceeds 512 bytes" << std::endl;
		sendMessage(fd, "ERROR :Closing Link: IRC line too long\r\n");
		client->markCloseAfterWrite();
		return;
	}

	std::string line;
	while (client->takeLine(line))
	{
		if (line.size() > 510)
		{
			std::cerr << "[PROTO] fd=" << fd << " rejected: IRC line exceeds 512 bytes" << std::endl;
			sendMessage(fd, "ERROR :Closing Link: IRC line too long\r\n");
			client->markCloseAfterWrite();
			return;
		}
		handleLine(*client, line);
		if (_clients.find(fd) == _clients.end() || client->shouldCloseAfterWrite())
			return;
	}
}

void Server::writeToClient(int fd)
{
	std::map<int, Client *>::iterator it = _clients.find(fd);
	if (it == _clients.end() || !it->second->hasOutput())
	{
		updatePollEvents(fd);
		return;
	}
	Client *client = it->second;
	const std::string &output = client->getOutputBuffer();
	ssize_t sent = send(fd, output.c_str(), output.size(), 0);
	if (sent <= 0)
	{
		std::cerr << "[IO] fd=" << fd << " send failed after POLLOUT" << std::endl;
		disconnectClient(fd);
		return;
	}
	client->consumeOutput(static_cast<std::size_t>(sent));
	if (!client->hasOutput() && client->shouldCloseAfterWrite())
	{
		disconnectClient(fd);
		return;
	}
	updatePollEvents(fd);
}

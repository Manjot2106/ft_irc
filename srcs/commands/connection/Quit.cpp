# include "IRC.hpp"

void	Server::handleQuit(Client &client, const Command &cmd)
{
	std::string reason = "Client disconnected";
	if (!cmd.params.empty() && !cmd.params[0].empty())
		reason = cmd.params[0];
	disconnectClient(client.getFd(), reason);
}

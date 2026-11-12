#include "IRC.hpp"

void Server::handlePing(Client &client, const Command &cmd)
{
	std::string nick = client.getNickname().empty() ? "*" : client.getNickname();
	if (cmd.params.empty())
	{
		sendMessage(client.getFd(), ":irc.local 409 " + nick + " :No origin specified\r\n");
		return;
	}
	sendMessage(client.getFd(), ":irc.local PONG irc.local :" + cmd.params[0] + "\r\n");
}

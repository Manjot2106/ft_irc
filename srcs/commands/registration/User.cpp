#include "IRC.hpp"

void Server::handleUser(Client &client, const Command &cmd)
{
	if (client.hasBeenWelcomed() || !client.getUsername().empty())
	{
		std::string nick = client.getNickname().empty() ? "*" : client.getNickname();
		sendMessage(client.getFd(), ":irc.local 462 " + nick
			+ " :You may not reregister\r\n");
		return;
	}
	if (cmd.params.size() < 4)
	{
		std::string nick = client.getNickname().empty() ? "*" : client.getNickname();
		sendMessage(client.getFd(), ":irc.local 461 " + nick
			+ " USER :Not enough parameters\r\n");
		return;
	}
	client.setUsername(cmd.params[0]);
	checkRegistration(client);
}

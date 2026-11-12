#include "IRC.hpp"

void Server::handlePass(Client &client, const Command &cmd)
{
	std::string nick = client.getNickname().empty() ? "*" : client.getNickname();

	if (client.hasBeenWelcomed() || client.isPasswordAccepted())
	{
		sendMessage(client.getFd(), ":irc.local 462 " + nick
			+ " :You may not reregister\r\n");
		return;
	}
	if (cmd.params.empty())
	{
		sendMessage(client.getFd(), ":irc.local 461 " + nick
			+ " PASS :Not enough parameters\r\n");
		return;
	}
	if (cmd.params[0] != _password)
	{
		sendMessage(client.getFd(), ":irc.local 464 " + nick
			+ " :Password incorrect\r\n");
		return;
	}
	client.setPasswordAccepted(true);
	checkRegistration(client);
}

#include "IRC.hpp"

bool Server::isNicknameUsed(const std::string &nickname, int exceptFd) const
{
	const std::string wanted = toUpper(nickname);
	for (std::map<int, Client *>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (it->first != exceptFd && toUpper(it->second->getNickname()) == wanted)
			return (true);
	}
	return (false);
}

void Server::handleNick(Client &client, const Command &cmd)
{
	if (cmd.params.empty())
	{
		sendMessage(client.getFd(), ":irc.local 431 * :No nickname given\r\n");
		return;
	}
	if (!isValidNickname(cmd.params[0]))
	{
		sendMessage(client.getFd(), ":irc.local 432 * " + cmd.params[0] + " :Erroneous nickname\r\n");
		return;
	}
	if (isNicknameUsed(cmd.params[0], client.getFd()))
	{
		sendMessage(client.getFd(), ":irc.local 433 * " + cmd.params[0] + " :Nickname is already in use\r\n");
		return;
	}
	const std::string previous = client.getNickname();
	client.setNickname(cmd.params[0]);
	if (!previous.empty() && client.hasBeenWelcomed())
	{
		const std::string message = ":" + previous + "!" + client.getUsername()
			+ "@irc.local NICK :" + client.getNickname() + "\r\n";
		std::set<Client *> notified;
		for (std::map<std::string, Channel *>::iterator it = _channels.begin(); it != _channels.end(); ++it)
		{
			if (!it->second->hasMember(client))
				continue;
			const std::vector<Client *> &members = it->second->getMembers();
			for (std::vector<Client *>::const_iterator mit = members.begin(); mit != members.end(); ++mit)
			{
				if (notified.insert(*mit).second)
					sendMessage((*mit)->getFd(), message);
			}
		}
	}
	checkRegistration(client);
	std::cout << "[REG] fd=" << client.getFd() << " nickname=" << client.getNickname() << std::endl;
}

#include "IRC.hpp"

void Server::rejectMissingPassword(Client &client, const std::string &command)
{
	std::cerr << "[AUTH] fd=" << client.getFd() << " rejected: "
		<< command << " received before valid PASS" << std::endl;
	sendMessage(client.getFd(), ":irc.local 464 * :Password required\r\n");
	sendMessage(client.getFd(), "ERROR :Closing Link: valid PASS required\r\n");
	client.markCloseAfterWrite();
}

void Server::handleLine(Client &client, const std::string &line)
{
	Command cmd = parseCommand(line);
	if (cmd.name.empty())
		return;
	std::cout << "[CMD] fd=" << client.getFd() << " nick="
		<< (client.getNickname().empty() ? "*" : client.getNickname())
		<< " command=" << cmd.name << " params=" << cmd.params.size() << std::endl;

	if (cmd.name == "CAP")
	{
		if (!cmd.params.empty() && toUpper(cmd.params[0]) == "LS")
			sendMessage(client.getFd(), ":irc.local CAP * LS :\r\n");
		return;
	}
	if (cmd.name == "PASS")
		return (handlePass(client, cmd));
	if (cmd.name == "NICK")
		return (handleNick(client, cmd));
	if (cmd.name == "USER")
		return (handleUser(client, cmd));
	if (cmd.name == "PING")
		return (handlePing(client, cmd));
	if (cmd.name == "QUIT")
		return (handleQuit(client, cmd));
	if (cmd.name != "JOIN" && cmd.name != "PART" && cmd.name != "PRIVMSG"
		&& cmd.name != "NOTICE"
		&& cmd.name != "KICK" && cmd.name != "INVITE" && cmd.name != "TOPIC"
		&& cmd.name != "MODE" && cmd.name != "NAMES")
	{
		std::string nick = client.getNickname().empty() ? "*" : client.getNickname();
		sendMessage(client.getFd(), ":irc.local 421 " + nick + " " + cmd.name
			+ " :Unknown command\r\n");
		return;
	}

	if (!client.isRegistered())
	{
		std::string nick = client.getNickname().empty() ? "*" : client.getNickname();
		sendMessage(client.getFd(), ":irc.local 451 " + nick + " :You have not registered\r\n");
		return;
	}

	const std::string arguments = commandArguments(line);
	if (cmd.name == "JOIN")
		return (handleJOIN(client, arguments));
	if (cmd.name == "PART")
		return (handlePART(client, arguments));
	if (cmd.name == "PRIVMSG")
		return (handlePRIVMSG(client, arguments));
	if (cmd.name == "NOTICE")
		return (handleNOTICE(client, arguments));
	if (cmd.name == "KICK")
		return (handleKick(client, cmd));
	if (cmd.name == "INVITE")
		return (handleInvite(client, cmd));
	if (cmd.name == "TOPIC")
		return (handleTopic(client, cmd));
	if (cmd.name == "MODE")
		return (handleMODE(client, arguments));
	if (cmd.name == "NAMES")
		return (handleNAMES(client, arguments));

}

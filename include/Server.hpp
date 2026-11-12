#ifndef SERVER_HPP
# define SERVER_HPP

class Server
{
	private:
		int						_port;
		int						_listenFd;
		std::string				_password;
		std::vector<struct pollfd>	_pollFds;
		std::map<int, Client *>		_clients;
		std::map<std::string, Channel *>	_channels;

		Server(const Server &other);
		Server &operator=(const Server &other);

		std::map<std::string, int>	_guessChannelGames;
		std::map<int, int>			_guessPrivateGames;

		void updatePollEvents(int fd);
		void rejectMissingPassword(Client &client, const std::string &command);

	public:
		Server(int port, const std::string &password);
		~Server();

		void initialize();
		void run();
		void acceptClient();
		void readFromClient(int fd);
		void writeToClient(int fd);
		void disconnectClient(int fd, const std::string &reason = "Client disconnected");
		void handleLine(Client &client, const std::string &line);
		void handlePass(Client &client, const Command &cmd);
		void handleNick(Client &client, const Command &cmd);
		void handleUser(Client &client, const Command &cmd);
		void handleJOIN(Client &client, const std::string &arguments);
		void handlePART(Client &client, const std::string &arguments);
		void handlePRIVMSG(Client &client, const std::string &arguments);
		void handleNOTICE(Client &client, const std::string &arguments);
		void handleKick(Client &client, const Command &cmd);
		void handleInvite(Client &client, const Command &cmd);
		void handleTopic(Client &client, const Command &cmd);
		void handleMODE(Client &client, const std::string &arguments);
		void handleNAMES(Client &client, const std::string &arguments);
		void sendNamesReply(Client &client, Channel &channel);
		void handlePing(Client &client, const Command &cmd);
		void handleQuit(Client &client, const Command &cmd);
		void sendMessage(int fd, const std::string &message);
		bool isNicknameUsed(const std::string &nickname, int exceptFd) const;
		Client *findClientByNickname(const std::string &nickname) const;
		Channel *findChannel(const std::string &name) const;
		void removeChannelIfEmpty(Channel *channel);
		void checkRegistration(Client &client);
		std::string clientPrefix(const Client &client) const;
		void handleGuessChannel(Client &client, Channel &channel, const std::string &text);
		void handleGuessPrivate(Client &client, const std::string &text);
};
#endif

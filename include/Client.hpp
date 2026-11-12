#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "IRC.hpp"

class Client
{
private:
	int			_fd;
	std::string	_nickname;
	std::string	_username;
	std::string	_inputBuffer;
	std::string	_outputBuffer;
	bool		_passwordAccepted;
	bool		_welcomed;
	bool		_closeAfterWrite;

public:
	Client(int fd);
	~Client();

	int getFd() const;
	const std::string &getNickname() const;
	const std::string &getUsername() const;
	void setNickname(const std::string &nickname);
	void setUsername(const std::string &username);
	bool isPasswordAccepted() const;
	void setPasswordAccepted(bool accepted);
	bool isRegistered() const;
	bool hasBeenWelcomed() const;
	void setWelcomed(bool welcomed);
	void appendInput(const char *data, std::size_t length);
	bool takeLine(std::string &line);
	bool inputTooLong() const;
	void queueOutput(const std::string &message);
	bool hasOutput() const;
	const std::string &getOutputBuffer() const;
	void consumeOutput(std::size_t length);
	void markCloseAfterWrite();
	bool shouldCloseAfterWrite() const;
};
#endif

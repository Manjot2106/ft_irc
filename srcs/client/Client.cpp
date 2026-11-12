#include "IRC.hpp"

Client::Client(int fd) : _fd(fd), _passwordAccepted(false), _welcomed(false), _closeAfterWrite(false)
{
}

Client::~Client()
{
}

int Client::getFd() const
{
	return (_fd);
}

const std::string &Client::getNickname() const
{
	return (_nickname);
}

const std::string &Client::getUsername() const
{
	return (_username);
}

void Client::setNickname(const std::string &nickname)
{
	_nickname = nickname;
}

void Client::setUsername(const std::string &username)
{
	_username = username;
}

bool Client::isPasswordAccepted() const
{
	return (_passwordAccepted);
}

void Client::setPasswordAccepted(bool accepted)
{
	_passwordAccepted = accepted;
}

bool Client::isRegistered() const
{
	return (_passwordAccepted && !_nickname.empty() && !_username.empty());
}

bool Client::hasBeenWelcomed() const
{
	return (_welcomed);
}

void Client::setWelcomed(bool welcomed)
{
	_welcomed = welcomed;
}

void Client::appendInput(const char *data, std::size_t length)
{
	_inputBuffer.append(data, length);
}

bool Client::takeLine(std::string &line)
{
	std::string::size_type end = _inputBuffer.find('\n');

	if (end == std::string::npos)
		return (false);
	line = _inputBuffer.substr(0, end);
	_inputBuffer.erase(0, end + 1);
	if (!line.empty() && line[line.size() - 1] == '\r')
		line.erase(line.size() - 1);
	return (true);
}

bool Client::inputTooLong() const
{
	std::string::size_type end = _inputBuffer.find('\n');
	if (end == std::string::npos)
		return (_inputBuffer.size() > 510);
	return (end > 511);
}

void Client::queueOutput(const std::string &message)
{
	_outputBuffer += message;
}

bool Client::hasOutput() const
{
	return (!_outputBuffer.empty());
}

const std::string &Client::getOutputBuffer() const
{
	return (_outputBuffer);
}

void Client::consumeOutput(std::size_t length)
{
	if (length >= _outputBuffer.size())
		_outputBuffer.clear();
	else
		_outputBuffer.erase(0, length);
}

void Client::markCloseAfterWrite()
{
	_closeAfterWrite = true;
}

bool Client::shouldCloseAfterWrite() const
{
	return (_closeAfterWrite);
}

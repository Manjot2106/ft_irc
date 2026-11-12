#include "IRC.hpp"

static const char *GUESS_BOT_PREFIX = ":Bot!bot@irc.local";

static bool isPlainNumber(const std::string &text, int &value)
{
	if (text.empty())
		return (false);
	for (std::size_t i = 0; i < text.size(); ++i)
	{
		if (text[i] < '0' || text[i] > '9')
			return (false);
	}
	char *end = 0;
	long parsed = std::strtol(text.c_str(), &end, 10);
	if (*end != '\0')
		return (false);
	value = static_cast<int>(parsed);
	return (true);
}

static std::string intToString(int value)
{
	std::ostringstream out;
	out << value;
	return (out.str());
}

void Server::handleGuessChannel(Client &client, Channel &channel, const std::string &text)
{
	const std::string key = toUpper(channel.getName());

	if (text == "!start")
	{
		if (_guessChannelGames.find(key) != _guessChannelGames.end())
		{
			channel.broadcast(*this, std::string(GUESS_BOT_PREFIX) + " PRIVMSG " + channel.getName() +
				" :Une partie est deja en cours, devinez un nombre entre 1 et 100 !\r\n", 0);
			return;
		}
		int target = 1 + (std::rand() % 100);
		_guessChannelGames[key] = target;
		channel.broadcast(*this, std::string(GUESS_BOT_PREFIX) + " PRIVMSG " + channel.getName() +
			" :Jeu du juste prix lance par " + client.getNickname() +
			" ! Devinez le nombre entre 1 et 100.\r\n", 0);
		return;
	}

	int guess;
	if (!isPlainNumber(text, guess))
		return;

	std::map<std::string, int>::iterator it = _guessChannelGames.find(key);
	if (it == _guessChannelGames.end())
		return;

	if (guess < 1 || guess > 100)
	{
		channel.broadcast(*this, std::string(GUESS_BOT_PREFIX) + " PRIVMSG " + channel.getName() +
			" :" + client.getNickname() + ", choisis un nombre entre 1 et 100.\r\n", 0);
		return;
	}
	if (guess < it->second)
		channel.broadcast(*this, std::string(GUESS_BOT_PREFIX) + " PRIVMSG " + channel.getName() + " :+ haut\r\n", 0);
	else if (guess > it->second)
		channel.broadcast(*this, std::string(GUESS_BOT_PREFIX) + " PRIVMSG " + channel.getName() + " :+ bas\r\n", 0);
	else
	{
		channel.broadcast(*this, std::string(GUESS_BOT_PREFIX) + " PRIVMSG " + channel.getName() +
			" :Bravo " + client.getNickname() + ", c'etait bien " + intToString(it->second) + " !\r\n", 0);
		_guessChannelGames.erase(it);
	}
}

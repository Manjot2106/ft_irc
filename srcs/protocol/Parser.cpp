#include "IRC.hpp"

std::string commandName(const std::string &line)
{
	std::string::size_type start = line.find_first_not_of(' ');
	if (start == std::string::npos)
		return ("");
	std::string::size_type end = line.find(' ', start);
	return (toUpper(line.substr(start, end - start)));
}

std::string commandArgument(const std::string &line)
{
	std::string args = commandArguments(line);
	std::string::size_type end = args.find(' ');
	return (args.substr(0, end));
}

std::string commandArguments(const std::string &line)
{
	std::string::size_type start = line.find_first_not_of(' ');
	if (start == std::string::npos)
		return ("");
	std::string::size_type end = line.find(' ', start);
	if (end == std::string::npos)
		return ("");
	start = line.find_first_not_of(' ', end);
	if (start == std::string::npos)
		return ("");
	return (line.substr(start));
}

Command parseCommand(const std::string &line)
{
	Command cmd;
	std::string::size_type start = 0;
	std::string::size_type end;

	while (start < line.size() && line[start] == ' ')
		++start;
	end = line.find(' ', start);
	if (end == std::string::npos)
	{
		cmd.name = toUpper(line.substr(start));
		return (cmd);
	}
	cmd.name = toUpper(line.substr(start, end - start));
	start = end + 1;
	while (start < line.size())
	{
		while (start < line.size() && line[start] == ' ')
			++start;
		if (start >= line.size())
			break;
		if (line[start] == ':')
		{
			cmd.params.push_back(line.substr(start + 1));
			break;
		}
		end = line.find(' ', start);
		if (end == std::string::npos)
		{
			cmd.params.push_back(line.substr(start));
			break;
		}
		cmd.params.push_back(line.substr(start, end - start));
		start = end + 1;
	}
	return (cmd);
}

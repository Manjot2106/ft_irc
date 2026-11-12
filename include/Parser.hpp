#ifndef PARSER_HPP
# define PARSER_HPP

# include <string>
# include "Command.hpp"

std::string commandName(const std::string &line);
std::string commandArgument(const std::string &line);
std::string commandArguments(const std::string &line);
Command parseCommand(const std::string &line);

#endif

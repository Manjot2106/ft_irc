#ifndef UTILS_HPP
# define UTILS_HPP

# include "IRC.hpp"

bool	isValidPort(const std::string &value, int &port);
bool	isValidNickname(const std::string &nickname);
std::string	toUpper(const std::string &value);

#endif

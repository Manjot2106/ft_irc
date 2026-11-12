#ifndef IRC_HPP
# define IRC_HPP

# define IPV4 AF_INET
# define TCP SOCK_STREAM

// Bibliothèque standard
# include <algorithm>
# include <cstdlib>
# include <csignal>
# include <cstring>
# include <iostream>
# include <map>
# include <ctime>
# include <set>
# include <sstream>
# include <string>
# include <vector>

// Système et réseau
# include <arpa/inet.h>
# include <cerrno>
# include <fcntl.h>
# include <netinet/in.h>
# include <poll.h>
# include <sys/socket.h>
# include <unistd.h>

// Structures simples
# include "Command.hpp"

// Classes du projet
# include "Client.hpp"
# include "Channel.hpp"
# include "Parser.hpp"
# include "Replies.hpp"
# include "Utils.hpp"
# include "CommandHandler.hpp"
# include "Server.hpp"

#endif
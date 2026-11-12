NAME		:= ircserv

CXX			:= c++
CXXFLAGS	:= -Wall -Wextra -Werror -std=c++98
CPPFLAGS	:= -Iinclude -MMD -MP

SRC_DIR		:= srcs
OBJ_DIR		:= obj

SRCS		:= \
	main.cpp \
	bot.cpp \
	server/Server.cpp \
	server/ServerInit.cpp \
	server/ServerLoop.cpp \
	server/ServerIO.cpp \
	server/ServerClients.cpp \
	client/Client.cpp \
	channel/Channel.cpp \
	protocol/Parser.cpp \
	protocol/CommandHandler.cpp \
	protocol/Replies.cpp \
	commands/registration/Pass.cpp \
	commands/registration/Nick.cpp \
	commands/registration/User.cpp \
	commands/connection/Ping.cpp \
	commands/connection/Quit.cpp \
	commands/channel/Join.cpp \
	commands/channel/Part.cpp \
	commands/channel/Privmsg.cpp \
	commands/channel/Kick.cpp \
	commands/channel/Invite.cpp \
	commands/channel/Topic.cpp \
	commands/channel/Mode.cpp \
	commands/channel/Names.cpp \
	utils/Utils.cpp

SRCS		:= $(addprefix $(SRC_DIR)/, $(SRCS))
OBJS		:= $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
DEPS		:= $(OBJS:.o=.d)

INCLUDE	:=	include/IRC.hpp

all: $(NAME)

$(NAME): $(OBJS) $(INCLUDE)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

-include $(DEPS)

.PHONY: all clean fclean re

NAME = ircserv

SRCFILES = src/Clients/Client.cpp \
			src/Clients/ClientChanMgmt.cpp \
			src/Clients/reseau.cpp \
			src/Commands/capCmd.cpp \
			src/Commands/initAllCommands.cpp \
			src/Commands/inviteCmd.cpp \
			src/Commands/joinCmd.cpp \
			src/Commands/kickCmd.cpp \
			src/Commands/modeCmd.cpp \
			src/Commands/nickCmd.cpp \
			src/Commands/partCmd.cpp \
			src/Commands/passCmd.cpp \
			src/Commands/pingCmd.cpp \
			src/Commands/privmsgCmd.cpp \
			src/Commands/topicCmd.cpp \
			src/Commands/userCmd.cpp \
			src/Commands/quitCmd.cpp \
			src/Servers/reseau.cpp \
			src/Servers/Server.cpp \
			src/Servers/ServerChanMgmt.cpp \
			src/Servers/ServerUtils.cpp \
			src/Servers/set.cpp \
			src/Channel/Channel.cpp \
			src/Utils.cpp \
			src/main.cpp
# 			src/Servers/irc.cpp \

HEADERS = include/Server.hpp \
			include/Channel.hpp \
			include/Client.hpp \
			include/Numerics.hpp

OBJDIR = obj

OBJFILES = $(SRCFILES:%.cpp=$(OBJDIR)/%.o)

CFLAGS = -Wall -Wextra -Werror -g3 -std=c++98

CC = c++

RM = rm -rf

all: $(NAME)

$(NAME): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJFILES)

$(OBJDIR)/%.o: %.cpp $(HEADERS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJDIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: clean fclean all re

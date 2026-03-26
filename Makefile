NAME = ircserv

SRCFILES = Clients/Client.cpp \
			Clients/reseau.cpp \
			Clients/Set.cpp \
			Clients/irc.cpp \
			Commands/capCmd.cpp \
			Commands/initAllCommands.cpp \
			Commands/inviteCmd.cpp \
			Commands/joinCmd.cpp \
			Commands/kickCmd.cpp \
			Commands/modeCmd.cpp \
			Commands/nickCmd.cpp \
			Commands/partCmd.cpp \
			Commands/passCmd.cpp \
			Commands/pingCmd.cpp \
			Commands/privmsgCmd.cpp \
			Commands/topicCmd.cpp \
			Commands/userCmd.cpp \
			Servers/reseau.cpp \
			Servers/Server.cpp \
			Servers/ServerChanMgmt.cpp \
			Servers/ServerUtils.cpp \
			Servers/set.cpp \
			Channel.cpp \
			Utils.cpp \
			main.cpp
# 			Servers/irc.cpp \

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
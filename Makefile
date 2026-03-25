NAME = ircserv

SRCFILES = Clients/Client.cpp \
			Clients/reseau.cpp \
			Clients/Set.cpp \
			Clients/irc.cpp \
			Servers/irc.cpp \
			Servers/reseau.cpp \
			Servers/Server.cpp \
			Servers/set.cpp \
			Channel.cpp \
			Utils.cpp \
			main.cpp

HEADERS = Server.hpp \
			Channel.hpp \
			Client.hpp

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
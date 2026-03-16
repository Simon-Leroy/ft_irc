NAME = ircserv

SRCFILES = Clients/Client.cpp \
			Clients/reseau.cpp \
			Clients/Set.cpp \
			Clients/irc.cpp \
			Servers/irc.cpp \
			Servers/reseau.cpp \
			Servers/Server.cpp \
			Channel.cpp \
			Utils.cpp \
			main.cpp

HEADERS = Server.hpp \
			Channel.hpp \
			Client.hpp

OBJFILES = $(SRCFILES:.cpp=.o)

CFLAGS = -Wall -Wextra -Werror -g3 -std=c++98

CC = c++

RM = rm -f

$(NAME): $(OBJFILES)
			$(CC) $(CFLAGS) -o $(NAME) $(OBJFILES)

%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

all: $(NAME)

clean:
			$(RM) $(OBJFILES)

fclean: clean
			$(RM) $(NAME)

re: fclean all

.PHONY: clean fclean all re

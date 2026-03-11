NAME = ircserv

SRCFILES = main_simon.cpp \
			Server_simon.cpp \
			Client.cpp \
			Commands.cpp \
			Utils.cpp

HEADERS = Server_simon.hpp \
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

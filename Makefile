NAME = ft_irc
CC = clang++
RM = rm -f
CXXFLAGS = -Wall -Werror -Wextra -std=c++98 -g3 -fsanitize=address -Wno-unused-parameter

SRCS = src/main.cpp src/Server.cpp src/Client.cpp src/Command.cpp src/Channel.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(CXXFLAGS) $(OBJS) -o $(NAME)

clean:
	@clear
	@$(RM) $(OBJS)

fclean: clean
	@$(RM) $(NAME)

re: fclean all

.PHONY: all re clean fclean
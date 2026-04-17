NAME		= codexion
CC			= cc
CFLAGS		= -Wall -Wextra -Werror -pthread

SRCS		= codexion.c init.c heap.c simulation.c monitor.c parser.c utils.c
OBJS		= $(SRCS:.c=.o)
HEADER		= codexion.h

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
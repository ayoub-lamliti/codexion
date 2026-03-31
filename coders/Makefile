NAME        = codexion

CC          = cc
CFLAGS      = -Wall -Wextra -Werror -pthread -g

SRCS        = codexion.c \
              init.c \
              parsing.c \
              simulation.c \
			  utils_time.c \
			  routine.c \
			  logger.c \
			  monitor.c

OBJS        = $(SRCS:.c=.o)

RM          = rm -f

all:        $(NAME)

$(NAME):    $(OBJS)
			$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

clean:
			$(RM) $(OBJS)

fclean:     clean
			$(RM) $(NAME)

re:         fclean all

.PHONY:     all clean fclean re
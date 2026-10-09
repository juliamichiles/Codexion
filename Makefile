NAME        := codexion
CC          := cc
CFLAGS      := -Wall -Wextra -Werror
INC         := -I include
SRCS        := source/main.c \
               source/parser.c \
               source/utils.c

OBJS        := $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re

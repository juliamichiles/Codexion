NAME        := codexion
CC          := cc
CFLAGS      := -Wall -Wextra -Werror -pthread
INC         := -I include
SRCS        := source/main.c \
               source/parser.c \
               source/utils.c \
			   source/init.c \
	    	   source/wrappers.c \
	       	   source/simulation.c \
			   source/logging.c \
			   source/get_set.c \
			   source/utils_2.c \
			   source/coder.c

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

NAME        := codexion
CC          := cc
CFLAGS      := -Wall -Wextra -Werror -pthread
INC         := -I include
SRCS        := source/main.c \
               source/parsing/parser.c \
			   source/utils/utils.c \
	    	   source/utils/wrappers.c \
			   source/utils/get_set.c \
			   source/utils/sim_utils.c \
			   source/simulation/init.c \
	       	   source/simulation/simulation.c \
			   source/simulation/coder.c \
			   source/simulation/heap.c \
			   source/simulation/monitor.c \
			   source/simulation/dongle_pair.c \
			   source/simulation/dongle.c

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

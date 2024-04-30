



CC	= gcc
CFLAGS	= -Wall -Wextra -Werror

NAME	= ft_ping

SRC	= ping.c
OBJ	= $(SRC:.c=.o)



all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -g -o $(NAME) $^

%.o: %.c
	$(CC) $(CFLAGS) -g -o $@ -c $<

clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME)

re: fclean $(NAME)

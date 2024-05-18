CC				=    gcc
CFLAGS			=    -Wall -Wextra -Werror

NAME			=    ft_ping

SRCFOLDER		=    src
OBJFOLDER		=    obj

SRCFILES		=    ping.c utils.c send.c recv.c stat.c parse.c
SRC				=    $(addprefix $(SRCFOLDER)/, $(SRCFILES))
OBJ				=    $(addprefix $(OBJFOLDER)/, $(SRCFILES:.c=.o))

INCLUDE        =    ./

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -g -o $(NAME) $^ -lm

$(OBJFOLDER)/%.o: $(SRCFOLDER)/%.c
	mkdir -p $(OBJFOLDER)
	$(CC) $(CFLAGS) -g -o $@ -c $< -I $(INCLUDE)

clean:
	rm -rf $(OBJFOLDER)

fclean: clean
	rm -rf $(NAME)

re: fclean $(NAME)

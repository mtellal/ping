



CC		:=	gcc
CFLAGS		:=	-Wall -Wextra -Werror

NAME		:=	ft_ping

SRCFOLDER	:=	src
SRCFILES	:=	ping.c utils.c packet.c recv.c
SRC		:=	$(addprefix $(SRCFOLDER)/, SRCFILES)

OBJFOLDER 	:=	obj
OBJ		:=	$(addprefix $(OBJFOLDER)/, $(SRCFILES:.c=.o))


INCLUDE		:=	./


all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -g -o $(NAME) $^

$(OBJFOLDER)/%.o: $(SRCFOLDER)/%.c
	mkdir -p $(OBJFOLDER)
	$(CC) $(CFLAGS) -g -o $@ -c $< -I $(INCLUDE)

clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME)

re: fclean $(NAME)

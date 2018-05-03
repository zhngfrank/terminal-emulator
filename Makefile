NAME = myshell

SRC = history.c parser.c myshell.c exec.c

.PHONY: all clean fclean re

all: $(NAME)

$(NAME):
	gcc -o $(NAME) $(SRC)

clean:
	rm -f *.o

fclean:
	rm -f $(NAME)

re: fclean all

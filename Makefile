OBJS:	myshell
CC:	cc
SRC:	argshell.c lex.yy.c

clean:	myshell
	rm -rf myshell
myshell:	argshell.c lex.yy.c
	$(CC) argshell.c lex.yy.c -o myshell -lfl



all:
	lex lex.l
	yacc -d yacc.y
	gcc -o mainz lex.yy.c y.tab.c main.c

all:
	lex lex.l
	yacc -d yacc.y
	gcc lex.yy.c y.tab.c main.c -ll -ly -o db

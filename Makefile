all:
	lex lex.l
	yacc -d yacc.y
#	gcc y.tab.c -ll -ly -o programmet
	gcc lex.yy.c y.tab.c main.c -ll -ly -o programmet

all:
	lex lex.l
	yacc -d yacc.y
	gcc y.tab.c -ll -y -o programmet

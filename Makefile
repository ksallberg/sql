all:
	lex lex.l
	yacc -d yacc.y
	gcc -c double_link.c -o list.o
	gcc lex.yy.c y.tab.c main.c list.o -ll -ly -o db

clean:
	git clean -ffxd

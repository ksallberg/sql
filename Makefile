all: build

build:
	lex lex.l
	yacc -d yacc.y
	gcc -c double_link.c -o list.o
	gcc -c bplus_tree.c -o tree.o
	gcc -g lex.yy.c y.tab.c main.c list.o tree.o -ll -ly -o db

clean:
	git clean -ffxd

run_with_init_file:
	./db ./init_files/simple_two_col_index.txt

test: build
	lux tests

# dependencies

lex, yacc, gcc

(libbison-dev needed)

# compiling

make all

# run

./db init_files/simple_two_col.txt

> SELECT * FROM apa;

```
| name | weight |
| "gorilla" | 400 |
| "gibbon" | 5 |
```

# indexing with B+

From 20,000 iterations to 8, but with the cost of maintaining a tree with links
to certain rows.

```
sql>
SELECT * FROM apa WHERE name="opnsbyde";
trav_select, what to select: SELECTALL table name: apa where: (name,"opnsbyde")
| name | weight |
index NOT used for select, full scan!
| "opnsbyde" | 19998 |
Cost to run query: 20000
```

```
sql>
SELECT * FROM apa WHERE name="opnsbyde";
trav_select, what to select: SELECTALL table name: apa where: (name,"opnsbyde")
| name | weight |
index used for select
| "opnsbyde" | 19998 |
Cost to run query: 8
```

# help found in the following places:

* https://stackoverflow.com/questions/40999748/call-a-function-in-a-yacc-file-from-another-c-file
* http://www.hep.by/gnu/bison/Error-Reporting.html
* https://www.gnu.org/software/bison/manual/html_node/Parser-Function.html
* https://stackoverflow.com/questions/40089510/how-to-get-the-return-value-of-the-start-rule

# credits

## SQL Parser

not created by me, found it here and just modified it slightly

https://github.com/ronakgadia/SQL-Parser

## B+ tree

Aider, Claude-3.5-sonnet

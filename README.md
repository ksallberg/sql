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

# help found in the following places:

* https://stackoverflow.com/questions/40999748/call-a-function-in-a-yacc-file-from-another-c-file
* http://www.hep.by/gnu/bison/Error-Reporting.html
* https://www.gnu.org/software/bison/manual/html_node/Parser-Function.html
* https://stackoverflow.com/questions/40089510/how-to-get-the-return-value-of-the-start-rule

# credits

## SQL Parser

not created by me, found it here and just modified it slightly

https://github.com/ronakgadia/SQL-Parser

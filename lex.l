
%{
#include "y.tab.h"
%}

%%

[ \n\t]+
(C|c)(R|r)(E|e)(A|a)(T|t)(E|e) {return CREATE;}
(D|d)(E|e)(L|l)(E|e)(T|t)(E|e) {return DELETE;}
(D|d)(R|r)(O|o)(P|p) {return DROP;}
(D|d)(A|a)(T|t)(A|a)(B|b)(A|a)(S|s)(E|e) {return DATABASE;}
(T|t)(A|a)(B|b)(L|l)(E|e) {return TABLE;}
(S|s)(E|e)(L|l)(E|e)(C|c)(T|t) {return SELECT;}
"INSERT INTO" {return INSERT_INTO;}
(U|u)(P|p)(D|d)(A|a)(T|t)(E|e) {return UPDATE;}
(S|s)(E|e)(T|t) {return SET;}
(F|f)(R|r)(O|o)(M|m) {return FROM;}
(W|w)(H|h)(E|e)(R|r)(E|e) {return WHERE;}
(A|a)(S|s) {return AS;}
(A|a)(N|n)(Y|y) {return ANY;}
(A|a)(L|l)(L|l) {return ALL;}
(A|a)(S|s)(C|c) {return ASC;}
(D|d)(E|e)(S|s)(C|c) {return DESC;}
(O|o)(N|n) {return ON;}
"ORDER BY" {return ORDER_BY;}
(H|h)(A|a)(V|v)(I|i)(N|n)(G|g) {return HAVING;}
"GROUP BY" {return GROUP_BY;}
(D|d)(I|i)(S|s)(T|t)(I|i)(N|n)(C|c)(T|t) {return DISTINCT;}
"NOT LIKE" {return NOT_LIKE;}
(L|l)(I|i)(K|k)(E|e) {return LIKE;}
"NOT BETWEEN" {return NOT_BETWEEN;}
(B|b)(E|e)(T|t)(W|w)(E|e)(E|e)(N|n) {return BETWEEN;}
"INNER JOIN" {return INNER_JOIN;}
"LEFT JOIN" {return LEFT_JOIN;}
"RIGHT JOIN" {return RIGHT_JOIN;}
"FULL OUTER JOIN" {return FULL_JOIN;}
"UNION ALL" {return UNION_ALL;}
(U|u)(N|n)(I|i)(O|o)(N|n) {return UNION;}
"NOT IN" {return NOT_IN;}
(I|i)(N|n) {return IN;}
(L|l)(I|i)(M|m)(I|i)(T|t) {return LIMIT;}
(E|e)(X|x)(I|i)(S|s)(T|t)(S|s) {return EXISTS;}
(V|v)(A|a)(L|l)(U|u)(E|e)(S|s) {return VALUES;}
(C|c)(O|o)(U|u)(N|n)(T|t) {return COUNT;}
(A|a)(V|v)(G|g) {return AVERAGE;}
(S|s)(U|u)(M|m) {return SUM;}
(M|m)(I|i)(N|n) {return MINIMUM;}
(M|m)(A|a)(X|x) {return MAXIMUM;}
"=" {return EQUALITY_OPERATOR;}
("<"|">"|"<="|">="|"<>"|"!=") {return RELATIONAL_OPERATOR;}
(A|a)(N|n)(D|d) {return AND;}
(O|o)(R|r) {return OR;}
"IS NULL" {return IS_NULL;}
"IS NOT NULL" {return IS_NOT_NULL;}
(N|n)(O|o)(T|t) {return NOT;}
"(" {return '(';}
")" {return ')';}
";" {return ';';}
"," {return COMMA;}
(char\([0-9]+\)|char|varchar|varchar\([0-9]+\)|bool|int|bigint|smallint|float) {return DATATYPE;}
(([a-zA-Z][a-zA-Z0-9]{0,127})(\.([a-zA-Z][a-zA-Z0-9]{0,127}))?) {yylval.strval=strdup(yytext); return IDENTIFIER;}
\'(([^\']|\\\')*[^\\])?\' {yylval.strval=strdup(yytext); return STRING;}
\"(([^\"]|\\\")*[^\\])?\" {yylval.strval=strdup(yytext); return STRING;}
[0-9]+ {yylval.strval=strdup(yytext); return NUMBER;}
"*" {return SELECTALL;}
%%

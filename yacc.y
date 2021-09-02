%{
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "types.h"
%}

%{
int yylex();
void yyerror(struct Node *n, char *s);
%}



%start program
%token <node> CREATE
%token <node> DELETE
%token <node> DROP
%token <node> DATABASE
%token <node> TABLE
%token <node> SELECT
%token <node> INSERT_INTO
%token <node> UPDATE
%token <node> SET
%token <node> FROM
%token <node> WHERE
%token <node> AS
%token <node> ALL
%token <node> ANY
%token <node> ON
%token <node> ASC
%token <node> DESC
%token <node> DATATYPE
%token <node> NUMBER
%token <node> STRING
%token <node> PATTERN
%token <node> IDENTIFIER
%token <node> SELECTALL
%token <node> COMMA
%token <node> DISTINCT
%token <node> INNER_JOIN
%token <node> LEFT_JOIN
%token <node> RIGHT_JOIN
%token <node> FULL_JOIN
%token <node> COUNT
%token <node> AVERAGE
%token <node> SUM
%token <node> HAVING
%token <node> EXISTS
%token <node> LIMIT
%token <node> IN
%token <node> NOT_IN
%token <node> MINIMUM
%token <node> MAXIMUM
%token <node> UNION
%token <node> UNION_ALL
%token <node> VALUES
%token <node> RELATIONAL_OPERATOR
%token <node> OR
%token <node> AND
%token <node> EQUALITY_OPERATOR
%token <node> NOT
%token <node> IS_NULL
%token <node> IS_NOT_NULL
%token <node> LIKE
%token <node> NOT_LIKE
%token <node> BETWEEN
%token <node> NOT_BETWEEN
%token <node> ORDER_BY
%token <node> GROUP_BY
%token <node> '(' ')'

%type <node> program database_stmt create_db drop_db table_stmt create_table declare_col drop_table union_stmt union_types insert_table valuelist query_stmt from_stmt origintable join_stmt join_types rename select_col selectways aggfunc aggfunctypes counttuples counttuplestypes diffcolumns where_stmt conditions relational_stmt query_bracket isbetween ispresent value groupby_stmt part1 having_stmt havingcond aggcond oper1 orderby_stmt part2 part3 sortorder logical_op rel_oper limit_stmt delete_stmt update_stmt intializelist isdistinct

%parse-param {struct Node* top_node}

%union{
  char *strval;
  struct Node* node;
}
%%

program : database_stmt ';' {
  $$ = mk_node("program");
  $$->child = $1;
  *top_node = *$$;
 }
| table_stmt ';' {
  $$ = mk_node("program");
  $$->child = $1;
  *top_node = *$$;
};

database_stmt : create_db {
  $$ = mk_node("database_stmt");
  $$->child = $1;
 }
| drop_db {
  $$ = mk_node("database_stmt");
  $$->child = $1;
  };

create_db : CREATE DATABASE IDENTIFIER {
  $$ = mk_node("create_db");
  $1 = mk_node("CREATE");
  $2 = mk_node("DATABASE");
  $3 = mk_node("IDENTIFIER");
  $$->child = $1;
  $1->sibling=$2;
  $2->sibling=$3;
};

drop_db : DROP DATABASE IDENTIFIER {
  $$ = mk_node("drop_db");
  $1 = mk_node("DROP");
  $2 = mk_node("DATABASE");
  $3 = mk_node("IDENTIFIER");
  $$->child = $1;
  $1->sibling=$2;
  $2->sibling=$3;
};

table_stmt : create_table {
  $$ = mk_node("table_stmt");
  $$->child = $1;
 }
| drop_table {
  $$ = mk_node("table_stmt");
  $$->child = $1;
  }
| insert_table {
  $$ = mk_node("table_stmt");
  $$->child = $1;
  }
| query_stmt {
  $$ = mk_node("table_stmt");
  $$->child = $1;
  }
| union_stmt {
  $$ = mk_node("table_stmt");
  $$->child = $1;
  }
| delete_stmt {
  $$ = mk_node("table_stmt");
  $$->child = $1;
  }
| update_stmt {
  $$ = mk_node("table_stmt");
  $$->child = $1;
  };

create_table : CREATE TABLE IDENTIFIER '(' declare_col ')' {
  $$ = mk_node("create_table");
  $1 = mk_node("CREATE");
  $2 = mk_node("TABLE");
  $3 = mk_node((char*) $3);
  $4 = mk_node("(");
  $6 = mk_node(")");
  $$->child = $1;
  $1->sibling=$2;
  $2->sibling=$3;
  $3->sibling=$4;
  $4->sibling=$5;
  $5->sibling=$6;
 }
| CREATE TABLE IDENTIFIER AS query_stmt	{
  $$ = mk_node("create_table");
  $1 = mk_node("CREATE");
  $2 = mk_node("TABLE");
  $3 = mk_node("IDENTIFIER");
  $4 = mk_node("AS");
  $$->child = $1;
  $1->sibling=$2;
  $2->sibling=$3;
  $3->sibling=$4;
  $4->sibling=$5;
 };

declare_col: IDENTIFIER DATATYPE COMMA declare_col {
  $$ = mk_node("declare_col");
  $1 = mk_node((char*) $1);
  //  free(yylval.strval);
  $2 = mk_node("DATATYPE");
  $3 = mk_node("COMMA");
  $$->child = $1;
  $1->sibling=$2;
  $2->sibling=$3;
  $3->sibling=$4;
 }
| IDENTIFIER DATATYPE {
  $$ = mk_node("declare_col");
  $1 = mk_node((char *) $1);
  //  $1 = mk_node("IDENTIFIER");
  $2 = mk_node("DATATYPE");
  $$->child = $1;
  $1->sibling=$2;
 };

drop_table : DROP TABLE IDENTIFIER {
  $$ = mk_node("drop_table");
  $1 = mk_node("DROP");
  $2 = mk_node("TABLE");
  $3 = mk_node("IDENTIFIER");
  $$->child = $1;
  $1->sibling=$2;
  $2->sibling=$3;
};

union_stmt : query_stmt union_types query_stmt {
  $$ = mk_node("union_stmt");
  $$->child = $1;
  $1->sibling=$2;
  $2->sibling=$3;
 }
| query_stmt union_types union_stmt {
  $$ = mk_node("union_stmt");
  $$->child = $1;
  $1->sibling=$2;
  $2->sibling=$3;
 };

union_types : UNION {
  $$ = mk_node("union_types");
  $1 = mk_node("UNION");
  $$->child = $1;
 }
| UNION_ALL {
  $$ = mk_node("union_types");
  $1 = mk_node("UNION_ALL");
  $$->child = $1;
};

insert_table : INSERT_INTO IDENTIFIER VALUES '(' valuelist ')' {
  $$ = mk_node("insert_table");
  $1 = mk_node("INSERT_INTO");
  $2 = mk_node((char *) $2);
  $3 = mk_node("VALUES");
  $4 = mk_node("(");
  $6 = mk_node(")");
  $$->child = $1;
  $1->sibling=$2;
  $2->sibling=$3;
  $3->sibling=$4;
  $4->sibling=$5;
  $5->sibling=$6;
 }
| INSERT_INTO IDENTIFIER '(' origintable ')' VALUES '(' valuelist ')' {
  $$ = mk_node("insert_table");
  $1 = mk_node("INSERT_INTO");
  $2 = mk_node("IDENTIFIER");
  $3 = mk_node("(");
  $5 = mk_node(")");
  $6 = mk_node("VALUES");
  $7 = mk_node("(");
  $9 = mk_node(")");
  $$->child = $1;
  $1->sibling=$2;
  $2->sibling=$3;
  $3->sibling=$4;
  $4->sibling=$5;
  $5->sibling=$6;
  $6->sibling=$7;
  $7->sibling=$8;
  $8->sibling=$9;
 }
| INSERT_INTO IDENTIFIER '(' origintable ')' query_stmt {
  $$ = mk_node("insert_table");
  $1 = mk_node("INSERT_INTO");
  $2 = mk_node("IDENTIFIER");
  $3 = mk_node("(");
  $5 = mk_node(")");
  $$->child = $1;
  $1->sibling=$2;
  $2->sibling=$3;
  $3->sibling=$4;
  $4->sibling=$5;
  $5->sibling=$6;
 }
| INSERT_INTO IDENTIFIER query_stmt {
  $$ = mk_node("insert_table");
  $1 = mk_node("INSERT_INTO");
  $2 = mk_node("IDENTIFIER");
  $$->child = $1;
  $1->sibling=$2;
  $2->sibling=$3;
 };

valuelist : value COMMA valuelist {
  $$ = mk_node("valuelist");
  $2 = mk_node("COMMA");
  $$->child = $1;
  $1->sibling=$2;
  $2->sibling=$3;
 }
| value	{
  $$ = mk_node("valuelist");
  $$->child = $1;
  };

query_stmt : SELECT isdistinct select_col from_stmt where_stmt groupby_stmt having_stmt orderby_stmt limit_stmt {
  $$ = mk_node("query_stmt");
  $1 = mk_node("SELECT");
  $$->child = $1;
  $1->sibling=$2;
  $2->sibling=$3;
  $3->sibling=$4;
  $4->sibling=$5;
  $5->sibling=$6;
  $6->sibling=$7;
  $7->sibling=$8;
  $8->sibling=$9;
 };

isdistinct : DISTINCT {
  $$ = mk_node("isdistinct");
  $1 = mk_node("DISTINCT");
  $$->child = $1;
 }
| {
  $$ = mk_node("isdistinct");
 };

from_stmt : FROM origintable {
  $$ = mk_node("from_stmt");
  $1 = mk_node("FROM");
  $$->child = $1;
  $1->sibling=$2;
 }
| FROM '(' query_stmt ')' {
  $$ = mk_node("from_stmt");
  $1 = mk_node("FROM");
  $2 = mk_node("(");
  $4 = mk_node(")");
  $$->child = $1;
  $1->sibling=$2;
  $2->sibling=$3;
  $3->sibling=$4;
  }
| FROM join_stmt {
  $$ = mk_node("from_stmt");
  $1 = mk_node("FROM");
  $$->child = $1;
  $1->sibling=$2;
 };

origintable : IDENTIFIER rename {
  $$ = mk_node("origintable");
  $1 = mk_node((char *) $1);//yylval.strval);
  //  free(yylval.strval);
  $$->child = $1;
  $1->sibling = $2;
 }
| IDENTIFIER rename COMMA origintable {
  $$ = mk_node("origintable");
  $1 = mk_node("IDENTIFIER");
  $3 = mk_node("COMMA");
  $$->child = $1;
  $1->sibling=$2;
  $2->sibling=$3;
  $3->sibling=$4;
 };

join_stmt : IDENTIFIER join_types IDENTIFIER ON IDENTIFIER EQUALITY_OPERATOR IDENTIFIER
{
  $$ = mk_node("join_stmt");
  $1 = mk_node("IDENTIFIER");
  $3 = mk_node("IDENTIFIER");
  $4 = mk_node("ON");
  $5 = mk_node("IDENTIFIER");
  $6 = mk_node("VALUES");
  $6 = mk_node("EQUALITY_OPERATOR");
  $7 = mk_node("IDENTIFIER");
  $$->child = $1;
  $1->sibling=$2;
  $2->sibling=$3;
  $3->sibling=$4;
  $4->sibling=$5;
  $5->sibling=$6;
  $6->sibling=$7;
}
| join_stmt join_types IDENTIFIER ON IDENTIFIER EQUALITY_OPERATOR IDENTIFIER
{
  $$ = mk_node("join_stmt");
  $3 = mk_node("IDENTIFIER");
  $4 = mk_node("ON");
  $5 = mk_node("IDENTIFIER");
  $6 = mk_node("VALUES");
  $6 = mk_node("EQUALITY_OPERATOR");
  $7 = mk_node("IDENTIFIER");
  $$->child = $1;
  $1->sibling=$2;
  $2->sibling=$3;
  $3->sibling=$4;
  $4->sibling=$5;
  $5->sibling=$6;
  $6->sibling=$7;
}
| '(' join_stmt ')' join_types IDENTIFIER ON IDENTIFIER EQUALITY_OPERATOR IDENTIFIER
| '(' '(' join_stmt ')' join_types IDENTIFIER ON IDENTIFIER EQUALITY_OPERATOR IDENTIFIER ')';

join_types : INNER_JOIN {
  $$ = mk_node("join_types");
  $1 = mk_node("INNER_JOIN");
  $$->child = $1;
 }
| LEFT_JOIN {
  $$ = mk_node("join_types");
  $1 = mk_node("LEFT_JOIN");
  $$->child = $1;
  }
| RIGHT_JOIN {
  $$ = mk_node("join_types");
  $1 = mk_node("RIGHT_JOIN");
  $$->child = $1;
  }
| FULL_JOIN {
  $$ = mk_node("join_types");
  $1 = mk_node("FULL_JOIN");
  $$->child = $1;
  };

rename : AS IDENTIFIER {
  $$ = mk_node("rename");
  $1 = mk_node("AS");
  $2 = mk_node("IDENTIFIER");
  $$->child = $1;
  $1->sibling=$2;
 }
| {$$ = mk_node("rename");};

select_col : selectways rename COMMA select_col {
  $$ = mk_node("select_col");
  $3 = mk_node("COMMA");
  $$->child = $1;
  $1->sibling=$2;
  $2->sibling=$3;
  $3->sibling=$4;
 }
| selectways rename {
  $$ = mk_node("select_col");
  $$->child = $1;
  $1->sibling = $2;
 };

selectways : diffcolumns {
  $$ = mk_node("selectways");
  $$->child = $1;
 }
| counttuples {
  $$ = mk_node("selectways");
  $$->child = $1;
  }
| aggfunc {
  $$ = mk_node("selectways");
  $$->child = $1;
  };

aggfunc : aggfunctypes '(' IDENTIFIER ')' {
  $$ = mk_node("aggfunc");
  $2 = mk_node("(");
  $3 = mk_node("IDENTIFIER");
  $4 = mk_node(")");
  $$->child = $1;
  $1->sibling=$2;
  $2->sibling=$3;
  $3->sibling=$4;
 };

aggfunctypes : AVERAGE {
  $$ = mk_node("aggfunctypes");
  $1 = mk_node("AVERAGE");
  $$->child = $1;
 }
| SUM {
  $$ = mk_node("aggfunctypes");
  $1 = mk_node("SUM");
  $$->child = $1;
  }
| MINIMUM {
  $$ = mk_node("aggfunctypes");
  $1 = mk_node("MINIMUM");
  $$->child = $1;
  }
| MAXIMUM {
  $$ = mk_node("aggfunctypes");
  $1 = mk_node("MAXIMUM");
  $$->child = $1;
  };

counttuples : COUNT '(' counttuplestypes ')' {
  $$ = mk_node("counttuples");
  $1 = mk_node("COUNT");
  $2 = mk_node("(");
  $3 = mk_node("IDENTIFIER");
  $4 = mk_node(")");
  $$->child = $1;
  $1->sibling=$2;
  $2->sibling=$3;
  $3->sibling=$4;
 };

counttuplestypes: IDENTIFIER {
  $$ = mk_node("counttuplestypes");
  $1 = mk_node("IDENTIFIER");
  $$->child = $1;
 }
| SELECTALL {
  $$ = mk_node("counttuplestypes");
  $1 = mk_node("SELECTALL");
  $$->child = $1;
  }
| DISTINCT IDENTIFIER {
  $$ = mk_node("counttuplestypes");
  $1 = mk_node("DISTINCT");
  $2 = mk_node("IDENTIFIER");
  $$->child = $1;
  $1->sibling = $2;
 }
| DISTINCT SELECTALL {
  $$ = mk_node("counttuplestypes");
  $1 = mk_node("DISTINCT");
  $2 = mk_node("SELECTALL");
  $$->child = $1;
  $1->sibling = $2;
 };

diffcolumns : SELECTALL {
  $$ = mk_node("diffcolumns");
  $1 = mk_node("SELECTALL");
  $$->child = $1;
 }
| IDENTIFIER {
  $$ = mk_node("diffcolumns");
  $1 = mk_node((char *) $1);
  //  free(yylval.strval);
  // $1 = mk_node("IDENTIFIER");
  $$->child = $1;
  };

where_stmt : WHERE conditions {
  $$ = mk_node("where_stmt");
  $1 = mk_node("WHERE");
  $$->child = $1;
  $1->child = $2;
 }
| {
  $$ = mk_node("where_stmt");
 };

conditions : relational_stmt logical_op conditions {
  $$ = mk_node("conditions");
  $$->child = $1;
  $1->sibling=$2;
  $2->sibling=$3;
 }
| NOT relational_stmt logical_op conditions {
  $$ = mk_node("conditions");
  $1 = mk_node("NOT");
  $$->child = $1;
  $1->sibling=$2;
  $2->sibling=$3;
  $3->sibling=$4;
 }
| NOT relational_stmt {
  $$ = mk_node("conditions");
  $1 = mk_node("NOT");
  $$->child = $1;
  $1->sibling=$2;
 }
| relational_stmt {
  $$ = mk_node("conditions");
  $$->child = $1;
  };

relational_stmt : IDENTIFIER rel_oper value {
  $$ = mk_node("relational_stmt");
  $1 = mk_node("IDENTIFIER");
  $$->child = $1;
  $1->sibling=$2;
  $2->sibling=$3;
 }
| IDENTIFIER EQUALITY_OPERATOR IDENTIFIER {
  $$ = mk_node("relational_stmt");
  $1 = mk_node("IDENTIFIER");
  $2 = mk_node("EQUALITY_OPERATOR");
  $3 = mk_node("IDENTIFIER");
  $$->child = $1;
  $1->sibling=$2;
  $2->sibling=$3;
 }
| IDENTIFIER IS_NULL {
  $$ = mk_node("relational_stmt");
  $1 = mk_node("IDENTIFIER");
  $2 = mk_node("IS_NULL");
  $$->child = $1;
  $1->sibling=$2;
 }
| IDENTIFIER IS_NOT_NULL {
  $$ = mk_node("relational_stmt");
  $1 = mk_node("IDENTIFIER");
  $2 = mk_node("IS_NOT_NULL");
  $$->child = $1;
  $1->sibling=$2;
 }
| IDENTIFIER LIKE STRING {
  $$ = mk_node("relational_stmt");
  $1 = mk_node("IDENTIFIER");
  $2 = mk_node("LIKE");
  $3 = mk_node("STRING");
  $$->child = $1;
  $1->sibling=$2;
  $2->sibling=$3;
 }
| IDENTIFIER NOT_LIKE STRING {
  $$ = mk_node("relational_stmt");
  $1 = mk_node("IDENTIFIER");
  $2 = mk_node("NOT_LIKE");
  $3 = mk_node("STRING");
  $$->child = $1;
  $1->sibling=$2;
  $2->sibling=$3;
 }
| IDENTIFIER ispresent '(' valuelist ')' {
  $$ = mk_node("relational_stmt");
  $1 = mk_node("IDENTIFIER");
  $3 = mk_node("(");
  $5 = mk_node(")");
  $$->child = $1;
  $1->sibling=$2;
  $2->sibling=$3;
  $3->sibling=$4;
  $4->sibling=$5;
 }
| IDENTIFIER ispresent query_bracket {
  $$ = mk_node("relational_stmt");
  $1 = mk_node("IDENTIFIER");
  $$->child = $1;
  $1->sibling=$2;
  $2->sibling=$3;
 }
| IDENTIFIER isbetween NUMBER AND NUMBER {
  $$ = mk_node("relational_stmt");
  $1 = mk_node("IDENTIFIER");
  $3 = mk_node("NUMBER");
  $4 = mk_node("AND");
  $5 = mk_node("NUMBER");
  $$->child = $1;
  $1->sibling = $2;
  $2->sibling = $3;
  $3->sibling = $4;
  $4->sibling = $5;
 }
| IDENTIFIER isbetween STRING AND STRING {
  $$ = mk_node("relational_stmt");
  $1 = mk_node("IDENTIFIER");
  $3 = mk_node("STRING");
  $4 = mk_node("AND");
  $5 = mk_node("STRING");
  $$->child = $1;
  $1->sibling=$2;
  $2->sibling=$3;
  $3->sibling=$4;
  $4->sibling=$5;
 }
| IDENTIFIER rel_oper ANY query_bracket {
  $$ = mk_node("relational_stmt");
  $1 = mk_node("IDENTIFIER");
  $3 = mk_node("ANY");
  $$->child = $1;
  $1->sibling=$2;
  $2->sibling=$3;
  $3->sibling=$4;
 }
| IDENTIFIER rel_oper ALL query_bracket	{
  $$ = mk_node("relational_stmt");
  $1 = mk_node("IDENTIFIER");
  $3 = mk_node("ALL");
  $$->child = $1;
  $1->sibling=$2;
  $2->sibling=$3;
  $3->sibling=$4;
 }
| EXISTS query_bracket {
  $$ = mk_node("relational_stmt");
  $1 = mk_node("EXISTS");
  $$->child = $1;
  $1->sibling=$2;
 };

query_bracket : '(' query_stmt ')' {
  $$ = mk_node("query_bracket");
  $1 = mk_node("(");
  $3 = mk_node(")");
  $$->child = $1;
  $1->sibling=$2;
  $2->sibling=$3;
 };

isbetween : BETWEEN {
  $$ = mk_node("isbetween");
  $1 = mk_node("BETWEEN");
  $$->child = $1;
 }
| NOT_BETWEEN {
  $$ = mk_node("isbetween");
  $1 = mk_node("NOT_BETWEEN");
  $$->child = $1;
  };

ispresent : IN {
  $$ = mk_node("ispresent");
  $1 = mk_node("IN");
  $$->child = $1;
 }
| NOT_IN {
  $$ = mk_node("ispresent");
  $1 = mk_node("NOT_IN");
  $$->child = $1;
  };

value : NUMBER {
  $$ = mk_node("value");
  $1 = mk_node((char *) $1);
  $$->child = $1;
 }
| STRING {
  $$ = mk_node("value");
  $1 = mk_node((char *) $1);
  $$->child = $1;
  };

groupby_stmt : GROUP_BY part1 {
  $$ = mk_node("groupby_stmt");
  $1 = mk_node("GROUP_BY");
  $$->child = $1;
  $1->sibling=$2;
 }
| {
  $$ = mk_node("groupby_stmt");
 };

part1 : IDENTIFIER COMMA part1 {
  $$ = mk_node("part1");
  $1 = mk_node("IDENTIFIER");
  $2 = mk_node("COMMA");
  $$->child = $1;
  $1->sibling=$2;
  $2->sibling=$3;
 }
| IDENTIFIER {
  $$ = mk_node("part1");
  $1 = mk_node("IDENTIFIER");
  $$->child = $1;
  };

having_stmt : HAVING havingcond {
  $$ = mk_node("having_stmt");
  $1 = mk_node("HAVING");
  $$->child = $1;
  $1->sibling=$2;
 }
| {
  $$ = mk_node("having_stmt");
 };

havingcond : aggcond logical_op havingcond {
  $$ = mk_node("havingcond");
  $$->child = $1;
  $1->sibling=$2;
  $2->sibling=$3;
 }
| aggcond {
  $$ = mk_node("havingcond");
  $$->child = $1;
  };

aggcond : oper1 rel_oper NUMBER {
  $$ = mk_node("aggcond");
  $3 = mk_node("NUMBER");
  $$->child = $1;
  $1->sibling=$2;
  $2->sibling=$3;
 }
| oper1 BETWEEN NUMBER AND NUMBER {
  $$ = mk_node("aggcond");
  $2 = mk_node("BETWEEN");
  $3 = mk_node("NUMBER");
  $4 = mk_node("AND");
  $5 = mk_node("NUMBER");
  $$->child = $1;
  $1->sibling=$2;
  $2->sibling=$3;
  $3->sibling=$4;
  $4->sibling=$5;
 };

oper1 : aggfunc {
  $$ = mk_node("oper1");
  $$->child = $1;
 }
| counttuples {
  $$ = mk_node("oper1");
  $$->child = $1;
  };

orderby_stmt : ORDER_BY part2 {
  $$ = mk_node("orderby_stmt");
  $1 = mk_node("ORDER_BY");
  $$->child = $1;
  $1->sibling=$2;
 }
| {
  $$ = mk_node("orderby_stmt");
 };

part2 : part3 sortorder COMMA part2 {
  $$ = mk_node("part2");
  $3 = mk_node("COMMA");
  $$->child = $1;
  $1->sibling=$2;
  $2->sibling=$3;
  $3->sibling=$4;
 }
| part3 sortorder {
  $$ = mk_node("part2");
  $$->child = $1;
  $1->sibling=$2;
 };

part3 : IDENTIFIER {
  $$ = mk_node("part3");
  $1 = mk_node("IDENTIFIER");
  $$->child = $1;
 }
| oper1	{
  $$ = mk_node("part3");
  $$->child = $1;
  };

sortorder : ASC {
  $$ = mk_node("sortorder");
  $1 = mk_node("ASC");
  $$->child = $1;
 }
| DESC{
  $$ = mk_node("sortorder");
  $1 = mk_node("ASC");
  $$->child = $1;
  }
| {
  $$ = mk_node("sortorder");
  };

logical_op : AND {
 $$ = mk_node("logical_op");
 $1 = mk_node("AND");
 $$->child = $1;
 }
| OR {
  $$ = mk_node("logical_op");
  $1 = mk_node("OR");
  $$->child = $1;
  };

rel_oper : RELATIONAL_OPERATOR {
  $$ = mk_node("rel_oper");
  $1 = mk_node("RELATIONAL_OPERATOR");
  $$->child = $1;
 }
| EQUALITY_OPERATOR {
  $$ = mk_node("rel_oper");
  $1 = mk_node("EQUALITY_OPERATOR");
  $$->child = $1;
  };

limit_stmt : LIMIT NUMBER {
  $$ = mk_node("limit_stmt");
  $1 = mk_node("LIMIT");
  $2 = mk_node("NUMBER");
  $$->child = $1; $1->sibling = $2;
 }
| {
  $$ = mk_node("limit_stmt");
 };

delete_stmt : DELETE from_stmt where_stmt {
  $$ = mk_node("delete_stmt");
  $1 = mk_node("DELETE");
  $$->child = $1;
  $1->sibling=$2;
  $2->sibling=$3;
 };

update_stmt : UPDATE IDENTIFIER SET intializelist where_stmt {
  $$ = mk_node("update_stmt");
  $1 = mk_node("UPDATE");
  $2 = mk_node("IDENTIFIER");
  $3 = mk_node("SET");
  $$->child = $1;
  $1->sibling=$2;
  $2->sibling=$3;
  $3->sibling=$4;
  $4->sibling=$5;
 };

intializelist : IDENTIFIER EQUALITY_OPERATOR value COMMA intializelist {
  $$ = mk_node("intializelist");
  $1 = mk_node("IDENTIFIER");
  $2 = mk_node("EQUALITY_OPERATOR");
  $4 = mk_node("COMMA");
  $$->child = $1;
  $1->sibling=$2;
  $2->sibling=$3;
  $3->sibling=$4;
  $4->sibling=$5;
 }
| IDENTIFIER EQUALITY_OPERATOR value {
  $$ = mk_node("intializelist");
  $1 = mk_node("IDENTIFIER");
  $2 = mk_node("EQUALITY_OPERATOR");
  $$->child = $1;
  $1->sibling=$2;
  $2->sibling=$3;
 };

%%

struct Node* mk_node(char* s) {
  struct Node *node = malloc(sizeof(struct Node));
  node->child = NULL;
  node->sibling = NULL;
  strcpy(node->str,s);
  return node;
}

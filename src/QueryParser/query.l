%option caseless
%{
#include "query.hpp"
%}

%%

".dbinfo"    { return DBINFO; }
".tables"    { return TABLES; }
[ \t\n]      ; // Ignore whitespace
.            { return yytext[0]; } // Unknown characters

%%

int yywrap() {
    return 1;
}
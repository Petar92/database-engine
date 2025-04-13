%{
#include "CommandHandler.h"
#include "ExecutionContext.h"
extern ExecutionContext* currentContext;
extern CommandHandler handler;
extern int yylex();
void yyerror(const char *s);
CommandHandler handler;
%}

%token DBINFO TABLES

%%

commands:
    command commands
    | /* empty */
    ;

command:
    DBINFO { handler.handleDbInfo(currentContext->database_file); }
    | TABLES { handler.handleTables(currentContext->database_file); }
    ;

%%
void yyerror(const char *s) {
    std::cerr << "Parsing error: " << s << std::endl;
}

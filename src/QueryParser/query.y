%{
#include <fstream>
#include "CommandHandler.h"
#include "FileManager.h"
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
    DBINFO { handler.handleDbInfo((std::ifstream)getDatabaseFile("sample.db")); }
    | TABLES { handler.handleTables((std::ifstream)getDatabaseFile("sample.db")); }
    ;

%%
void yyerror(const char *s) {
    std::cerr << "Parsing error: " << s << std::endl;
}

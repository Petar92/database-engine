#SQL Query Handling Using Flex/Bison and Shared Execution Context

**Context & Motivation**  
I initially handled SQL-like commands in my C++ CLI tool using simple `if/else` logic: every invocation would open the database file, inspect the command string, and drive actions accordingly. To explore how production systems parse and manage complex queries, I introduced Flex/Bison for grammar-based handling. This change highlighted the need to inject shared data�such as the database file path�into parser actions without resorting to uncontrolled globals. Below is my **Lessons Learned** article detailing a clean, extensible solution using an `ExecutionContext`.

---

## Execution Context

Define a context struct to hold shared state such as the database file path.

```cpp
// ExecutionContext.h
#ifndef EXECUTION_CONTEXT_H
#define EXECUTION_CONTEXT_H

#include <string>

/// Holds shared state for command parsing and execution
struct ExecutionContext {
    std::string database_file;
    // Add more shared data here if needed
};

#endif // EXECUTION_CONTEXT_H
```
# Bison Grammar (parser.y)
Use an external pointer to the ExecutionContext and your command handler inside Bison actions.

```yacc
%{
#include "ExecutionContext.h"
#include "CommandHandler.h"

extern ExecutionContext* currentContext;  // Defined in Server.cpp
extern CommandHandler handler;
%}

%token DBINFO TABLES

%%

command:
    DBINFO {
        // Access the database file from the execution context
        handler.handleDbInfo(currentContext->database_file);
    }
  | TABLES {
        handler.handleTables(currentContext->database_file);
    }
;

%%

# Server Initialization (Server.cpp)
Initialize the execution context before parsing.

#include "ExecutionContext.h"
#include "parser.tab.h"   // Bison-generated header
#include "CommandHandler.h"

extern int yyparse();
extern void yy_scan_string(const char*);

// Global pointer used by Bison actions
ExecutionContext* currentContext = nullptr;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <db_file> <command>\n";
        return 1;
    }

    // Populate execution context
    ExecutionContext ctx;
    ctx.database_file = argv[1];
    currentContext = &ctx;

    // Scan and parse the command string
    std::string command = argv[2];
    yy_scan_string(command.c_str());
    yyparse();

    return 0;
}
```

# Command Handler Implementation
Provide concrete implementations for handling parsed commands.

```h
// CommandHandler.h
#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include <string>

/// Executes actions based on parsed commands and shared context
class CommandHandler {
public:
    void handleDbInfo(const std::string& db_file);
    void handleTables(const std::string& db_file);
};

#endif // COMMAND_HANDLER_H
```
```cpp
// CommandHandler.cpp
#include "CommandHandler.h"
#include <iostream>

void CommandHandler::handleDbInfo(const std::string& db_file) {
    std::cout << "Running .dbinfo on " << db_file << "\n";
}

void CommandHandler::handleTables(const std::string& db_file) {
    std::cout << "Running .tables on " << db_file << "\n";
}

# Usage Example
```
```bash
$ ./my_cli_app my_database.db dbinfo
Running .dbinfo on my_database.db

$ ./my_cli_app my_database.db tables
Running .tables on my_database.db
```
# Result
Commands are parsed via Flex/Bison.

Shared state (database_file) is accessed cleanly through ExecutionContext.

The design remains extensible and testable without excessive global variables.
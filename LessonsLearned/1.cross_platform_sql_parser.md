This summary was formatted and reworded using ChatGPT

# Building a Cross-Platform SQL Parser in C++ with Flex, Bison, and CMake

Recently, I started working on a SQLite-like query engine in C++. I needed to use **Flex and Bison** to parse SQL queries — things like:

```sql
SELECT name FROM users;
```

The catch? I wanted to build and run the project both from **WSL (Linux-like dev environment)** and **Windows (Visual Studio 2022)**.

It took a bit of work to get there. Here's what I ran into, how I fixed it, and what I learned along the way.

---

## 🧱 The Setup

I structured my project like this:

```
codecrafters-sqlite-cpp/
├── CMakeLists.txt
├── build-wsl/                # WSL build output
├── build-win/                # Windows build output
├── src/
│   ├── main.cpp
│   └── QueryParser/
│       ├── query.y           # Bison grammar
│       ├── query.l           # Flex lexer
│       ├── query_logic.h     # Logic header
│       └── query_logic.cpp   # Stub logic
```

---

## 🚨 Problems I Encountered

### ❌ `BISON_TARGET` not found

CMake threw an error saying `Unknown CMake command "BISON_TARGET"`. Turns out I needed to:

- Add `find_package(BISON REQUIRED)` and `find_package(FLEX REQUIRED)` before using the macros.
- Also, on Windows, Bison isn’t installed by default — more on that below.

---

### ❌ Chocolatey installs `win_bison`, not `bison`

When I installed Bison and Flex via Chocolatey:

```powershell
choco install winflexbison -y
```

…it gave me `win_bison.exe` and `win_flex.exe` instead of the expected `bison` and `flex`. So, CMake couldn’t find them.

The fix was to explicitly tell CMake which binary to use depending on the platform:

```cmake
if (WIN32)
    find_program(BISON_EXECUTABLE win_bison REQUIRED)
    find_program(FLEX_EXECUTABLE win_flex REQUIRED)
else()
    find_program(BISON_EXECUTABLE bison REQUIRED)
    find_program(FLEX_EXECUTABLE flex REQUIRED)
endif()
```

---

### ❌ Visual Studio’s CMake cache conflicts with WSL builds

If I ran `cmake ..` from WSL and then opened the same folder in Visual Studio, I got this lovely error:

```
CMake Error: The current CMakeCache.txt directory ... is different than the directory ...
```

Turns out, **you can’t share the same build folder across WSL and Windows**. So I created two build folders:
- `build-wsl/`
- `build-win/`

Problem solved.

## ✅ What Finally Worked (Code & Build)

### CMakeLists.txt (simplified)

```cmake
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

if (WIN32)
    find_program(BISON_EXECUTABLE win_bison REQUIRED)
    find_program(FLEX_EXECUTABLE win_flex REQUIRED)
else()
    find_program(BISON_EXECUTABLE bison REQUIRED)
    find_program(FLEX_EXECUTABLE flex REQUIRED)
endif()

BISON_TARGET(QueryParser src/QueryParser/query.y ${CMAKE_BINARY_DIR}/query.tab.c)
FLEX_TARGET(QueryParser src/QueryParser/query.l ${CMAKE_BINARY_DIR}/query.yy.c)
ADD_FLEX_BISON_DEPENDENCY(QueryParser QueryParser)

add_executable(sqlite
    src/main.cpp
    ${BISON_QueryParser_OUTPUTS}
    ${FLEX_QueryParser_OUTPUTS}
    src/QueryParser/query_logic.cpp
)

target_include_directories(sqlite PRIVATE ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/src/QueryParser)
```

---

### Bison Grammar (`query.y`)

```bison
%{
#include "query_logic.h"
%}

%token SELECT FROM IDENTIFIER
%union { char* str }
%type <str> IDENTIFIER

%%
query:
    SELECT IDENTIFIER FROM IDENTIFIER {
        handleSelectQuery($2, $4);
        free($2); free($4);
    }
    ;
%%
int yyerror(const char* s) {
    fprintf(stderr, "Parse error: %s\n", s);
    return 1;
}
```

---

### Flex Lexer (`query.l`)

```lex
%{
#include "query.tab.h"
%}

IDENTIFIER [a-zA-Z_][a-zA-Z0-9_]*

%%
"SELECT"        { return SELECT; }
"FROM"          { return FROM; }
{IDENTIFIER}    { yylval.str = strdup(yytext); return IDENTIFIER; }
[\t\r\n ]+      ;
.               { return yytext[0]; }

%%
int yywrap() { return 1; }
```

---

### Query Logic Stub (`query_logic.cpp`)

```cpp
void handleSelectQuery(const char* column, const char* table) {
    // TODO: select query logic goes here
}
```

---

### Main Entrypoint (`main.cpp`)

```cpp
extern int yyparse();
extern FILE* yyin;

int main() {
    yyin = stdin;
    yyparse();
    return 0;
}
```

---

## 📦 Build Script (WSL)

```bash
#!/bin/bash
set -e
mkdir -p build-wsl && cd build-wsl
cmake ..
cmake --build .
```

Don’t forget to convert it to Unix format!

---

## 🧠 Lessons Learned

- Never share `CMakeCache.txt` between Windows and WSL builds — always use separate `build/` folders.
- When using Chocolatey for Bison/Flex, watch out for `win_bison.exe` instead of `bison`.
- `BISON_TARGET` won't work unless `find_package(BISON REQUIRED)` is called first.
- Always convert scripts to Unix line endings before running them in WSL.
- Keeping parsing logic separated from business logic makes your codebase a lot easier to grow and maintain.

---

If you're building a compiler, interpreter, or SQL engine in C++, this setup should save you from some major headaches. I’ll likely extend this further to support `INSERT`, `WHERE`, and basic expression evaluation next.


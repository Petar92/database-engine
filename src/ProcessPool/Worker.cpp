#include <iostream>
#include <string>

extern int yyparse();
extern void yy_scan_string(const char*);

class Worker {
	int createProcess() {

	}

	int executeCommand(std::string command) {
        yy_scan_string(command.c_str());

        int result = yyparse();

        if (result != 0) {
            std::cerr << "Error parsing command.\n";
            return 1;
        }

        return 0;
	}
};
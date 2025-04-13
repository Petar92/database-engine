#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include <iostream>

class CommandHandler {
public:
    void handleDbInfo(std::ifstream& database_file);
    void handleTables(std::ifstream& database_file);
};

#endif

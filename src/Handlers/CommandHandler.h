#pragma once

#include <iostream>

class CommandHandler {
public:
    void handleDbInfo(std::ifstream& database_file);
    void handleTables(std::ifstream& database_file);
};

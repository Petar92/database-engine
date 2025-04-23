#include "CommandHandler.h"

void CommandHandler::handleDbInfo(std::ifstream& database_file) {
    //char header[100];
    //database_file.read(header, 100);
    //std::cerr << std::string(header, header + 16) << std::endl; // print the sqlite header message

    database_file.seekg(16);  // Skip the first 16 bytes of the header
     
    char buffer[2];
    database_file.read(buffer, 2);
     
    unsigned short page_size = (static_cast<unsigned char>(buffer[1]) | (static_cast<unsigned char>(buffer[0]) << 8));
     
    std::cout << "database page size: " << page_size << std::endl;
}

void CommandHandler::handleTables(std::ifstream& database_file) {
    database_file.seekg(103);
    char tables_count_bufffer[2];
    database_file.read(tables_count_bufffer, 2);
    unsigned short table_count = (static_cast<unsigned char>(tables_count_bufffer[1]) | (static_cast<unsigned char>(tables_count_bufffer[0]) << 8));
    std::cout << "number of tables: " << table_count << std::endl;
}

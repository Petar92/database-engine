#include <iostream>

#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

std::ifstream getDatabaseFile(std::string& database_file_path);
void readDatabaseFileByOffset(std::ifstream& database_file, std::uint16_t offset, std::uint16_t buffer_size);

#endif
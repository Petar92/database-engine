#include <iostream>

#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

static std::ifstream getDatabaseFile(std::string database_file_path);
static void readDatabaseFileByOffset(std::ifstream& database_file, std::uint16_t offset, std::uint16_t buffer_size);

#endif
#include <cstdint>
#include <fstream>
#include <iostream>
#include "FileManager.h"
#include <vector>

static std::ifstream getDatabaseFile(std::string database_file_path)
{
    std::ifstream database_file(database_file_path, std::ios::binary);
    if (!database_file) {
        std::cerr << "Failed to open the database file" << std::endl;
    }
    return database_file;
}

static void readDatabaseFileByOffset(std::ifstream& database_file, std::uint16_t offset, std::uint16_t buffer_size)
{
    database_file.seekg(offset);
    std::vector<char> bufffer(buffer_size);
    database_file.read(bufffer.data(), buffer_size);
}
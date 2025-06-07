#pragma once

#include <iostream>

std::ifstream getDatabaseFile(const std::string& database_file_path);
void readDatabaseFileByOffset(std::ifstream& database_file, std::uint16_t offset, std::uint16_t buffer_size);

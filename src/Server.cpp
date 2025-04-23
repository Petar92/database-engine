#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include "FileManager/FileManager.h"

extern void yy_scan_string(const char*);
extern int yyparse();


int main(int argc, char* argv[]) {
    // Flush after every std::cout / std::cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    if (argc != 3) {
        std::cerr << "Expected two arguments" << std::endl;
        return 1;
    }

    std::string database_file_path = argv[1];
    std::string command = argv[2];

    auto database_file = getDatabaseFile(database_file_path);

    if (!database_file) {
        std::cerr << "Error getting database file.\n";
        return EXIT_FAILURE;
    }

    yy_scan_string(command.c_str());
    int result = yyparse();

    if (result != 0) {
        std::cerr << "Error parsing command.\n";
        return EXIT_FAILURE;
    }

    return 0;

    //if (command == ".dbinfo") {


        //char header[100];
        //database_file.read(header, 100);
        //std::cerr << std::string(header, header + 16) << std::endl;

        //database_file.seekg(16);  // Skip the first 16 bytes of the header
        // 
        //char buffer[2];
        //database_file.read(buffer, 2);
        // 
        //unsigned short page_size = (static_cast<unsigned char>(buffer[1]) | (static_cast<unsigned char>(buffer[0]) << 8));
        // 
        //std::cout << "database page size: " << page_size << std::endl;

        //database_file.seekg(103);
        //char tables_count_bufffer[2];
        //database_file.read(tables_count_bufffer, 2);
        //unsigned short table_count = (static_cast<unsigned char>(tables_count_bufffer[1]) | (static_cast<unsigned char>(tables_count_bufffer[0]) << 8));

        //std::cout << "number of tables: " << table_count << std::endl;
    //}
    //else if (command == ".tables") {
    //    auto database_file = getDatabaseFile(database_file_path);

    //    database_file.seekg(103);
    //    char table_buffer[2];
    //    database_file.read(table_buffer, 2);

    //    unsigned short table_count = (static_cast<unsigned char>(table_buffer[1]) | (static_cast<unsigned char>(table_buffer[0]) << 8));

    //    database_file.seekg(108);

    //    std::vector<uint16_t> cells(table_count); 

    //    for (int i = 0; i < table_count; ++i) {
    //        char cell_offset[2];
    //        database_file.read(cell_offset, 2);
    //        cells[i] = (static_cast<unsigned char>(cell_offset[1]) |
    //            (static_cast<unsigned char>(cell_offset[0]) << 8));
    //    }

    //    for (int i = 0; i < table_count; i++) {

    //        database_file.seekg(cells[i]);

    //        char record_size_buffer[1];
    //        database_file.read(record_size_buffer, 1);

    //        unsigned short record_size = static_cast<unsigned char>(record_size_buffer[0]);
    //        //std::cerr << "Record size " << record_size << std::endl;

    //        database_file.seekg(cells[i]+1);
    //        char row_id_buffer[1];
    //        database_file.read(row_id_buffer, 1);

    //        unsigned short row_id = static_cast<unsigned char>(row_id_buffer[0]);
    //        //std::cerr << "Row id " << row_id << std::endl;


    //        database_file.seekg(cells[i]+2);
    //        char header_size_buffer[1];
    //        database_file.read(header_size_buffer, 1);

    //        unsigned short header_size = static_cast<unsigned char>(header_size_buffer[0]);
    //        //std::cerr << "Header size " << header_size << std::endl;


    //        database_file.seekg(cells[i]+3);
    //        char schema_type_buffer[1];
    //        database_file.read(schema_type_buffer, 1);

    //        unsigned short schema_type = static_cast<unsigned char>(schema_type_buffer[0]);
    //        unsigned short schema_type_size = (schema_type >= 13 && schema_type % 2 == 1) ? (schema_type - 13) / 2 : (schema_type - 12) / 2;
    //        //std::cerr << "Size of schema type " << schema_type_size << std::endl;

    //        
    //        database_file.seekg(cells[i]+4);
    //        char schema_name_buffer[1];
    //        database_file.read(schema_name_buffer, 1);

    //        unsigned short schema_name = static_cast<unsigned char>(schema_name_buffer[0]);
    //        unsigned short schema_name_size = (schema_name >= 13 && schema_name % 2 == 1) ? (schema_name - 13) / 2 : (schema_name - 12) / 2;
    //        //std::cerr << "Size of schema name " << schema_name_size << std::endl;



    //        database_file.seekg(cells[i]+5);
    //        char table_name_buffer[1];
    //        database_file.read(table_name_buffer, 1);

    //        unsigned short table_name = static_cast<unsigned char>(table_name_buffer[0]);
    //        unsigned short table_name_size = (table_name >= 13 && table_name % 2 == 1) ? (table_name - 13) / 2 : (table_name - 12) / 2;
    //        //std::cerr << "Size of table name " << table_name_size << std::endl;



    //       /* database_file.seekg(cells[i] + 2 + header_size);

    //        std::vector<char> tb_type(schema_type_size);
    //        database_file.read(tb_type.data(), schema_type_size);
    //        std::string tb_type_value(tb_type.begin(), tb_type.end());
    //        std::cout << tb_type_value << std::endl;


    //        database_file.seekg(cells[i] + 2 + header_size + schema_type_size);

    //        std::vector<char> sc_name(schema_name_size);
    //        database_file.read(sc_name.data(), table_name_size);
    //        std::string sc_name_value(sc_name.begin(), sc_name.end());
    //        std::cout << sc_name_value << std::endl;*/


    //        database_file.seekg(cells[i] + 2 + header_size + schema_type_size + schema_name_size);

    //        std::vector<char> tb_name(table_name_size);
    //        database_file.read(tb_name.data(), table_name_size);
    //        std::string tb_name_value(tb_name.begin(), tb_name.end());
    //        std::cout << tb_name_value << std::endl;

    //    }
    //}
}

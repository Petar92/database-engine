#include <iostream>
#include <thread>
#include <string>
#include "ProcessPool/ProcessPool.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: database_engine_server <port>" << std::endl;
        return 1;
    }

    uint16_t port = static_cast<uint16_t>(std::stoi(argv[1]));

    ProcessPool::getProcessPoolInstance(port);

    return 0;
}

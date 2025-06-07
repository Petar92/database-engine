#include <iostream>
#include <thread>
#include "ProcessPool.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: database_engine_server <port>" << std::endl;
        return 1;
    }

    uint16_t port = std::static_pointer_cast(argv[i]);

    ProcessPool::getProcessPoolInstance(port);

    return 0;
}

#include "/mnt/c/Projects/database-engine/include/network/server.hpp"

int main() {
    boost::asio::io_context io_context;
    Server server(io_context, 56000);
    io_context.run();
    return 0;
}
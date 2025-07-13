#pragma once

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class Server {

public:
    boost::asio::io_context io_context;
    
    Server();
    ~Server();

};
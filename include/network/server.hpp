#pragma once

#include "tcpconnection.hpp"
#include <boost/asio.hpp>
#include <memory>

using boost::asio::ip::tcp;

class Server {

public:
    Server(boost::asio::io_context& io_context, unsigned short port);
    ~Server();

private:
    boost::asio::io_context& io_context_;
    tcp::acceptor acceptor_;
    
    void start_accept();
    void handle_accept(tcp_connection::TcpConnection::pointer new_connection, const boost::system::error_code& error);
};


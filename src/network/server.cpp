#include "network/server.hpp"
#include "network/tcpconnection.hpp"

using boost::asio::ip::tcp;

Server::Server(boost::asio::io_context& io_context, unsigned short port)
  : io_context_(io_context),
    acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
{
    start_accept();
}

Server::~Server() {}

void Server::start_accept() {
    tcp_connection::TcpConnection::pointer new_connection = tcp_connection::TcpConnection::create(io_context_);

    acceptor_.async_accept(new_connection->socket(),
    std::bind(&Server::handle_accept, this, new_connection,
    boost::asio::placeholders::error));
}

void Server::handle_accept(tcp_connection::TcpConnection::pointer new_connection,
    const boost::system::error_code& error) {
    if (!error)
    {
        new_connection->start();
    }
    
    start_accept();
}

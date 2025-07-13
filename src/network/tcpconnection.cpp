#include "/mnt/c/Projects/database-engine/include/network/tcpconnection.hpp"
#include <boost/asio/write.hpp>           
#include <boost/asio/placeholders.hpp>    
#include <functional>                     
#include <memory>                         

namespace tcp_connection {

TcpConnection::pointer
TcpConnection::create(boost::asio::io_context& io_context) {
    return std::make_shared<TcpConnection>(io_context);
}

boost::asio::ip::tcp::socket&
TcpConnection::socket() {
    return socket_;
}

void
TcpConnection::start() {
    message_ = "query received";
    boost::asio::async_write(
        socket_,
        boost::asio::buffer(message_),
        std::bind(
            &TcpConnection::handle_write,
            shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred
        )
    );
}

TcpConnection::TcpConnection(boost::asio::io_context& io_context)
  : socket_(io_context)
{}

void
TcpConnection::handle_write(
    const boost::system::error_code&,
    std::size_t
) {

}

} 

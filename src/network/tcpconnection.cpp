#include "/mnt/c/Projects/database-engine/include/network/tcpconnection.hpp"
#include <boost/asio/write.hpp>           
#include <boost/asio/placeholders.hpp>    
#include <functional>                     
#include <memory>                         

namespace tcp_connection {

TcpConnection::pointer
TcpConnection::create(boost::asio::io_context& io_context) {
    return pointer( new TcpConnection(io_context) );
}

boost::asio::ip::tcp::socket&
TcpConnection::socket() {
    return socket_;
}

void TcpConnection::start() {
    message_ = "query received";

    boost::asio::async_write(
        socket_,
        boost::asio::buffer(message_),
        [self = shared_from_this()] // extend the lieftime of the object until write completes in order to prevent the object from being destroyed mid-opearation
        (const boost::system::error_code& ec, std::size_t bytes_transferred) {
            self->handle_write(ec, bytes_transferred);
        }
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

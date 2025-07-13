// TcpConnection.hpp
#pragma once
#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <string>
#include <memory>

namespace tcp_connection {
class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
public:
    using pointer = std::shared_ptr<TcpConnection>;
    static pointer create(boost::asio::io_context& io_context);
    boost::asio::ip::tcp::socket& socket();
    void start();

private:
    explicit TcpConnection(boost::asio::io_context& io_context);
    void handle_write(const boost::system::error_code&, std::size_t);

    boost::asio::ip::tcp::socket socket_;
    std::string message_;
};
}

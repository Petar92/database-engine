#pragma once

#include <boost/asio.hpp>
class Server {
public:
	Server(boost::asio::io_context& io_context)
		: socket_(io_context)
	{
	}

};
#include <iostream>
#include "ProcessPool.h"
#include "Worker.h"

#ifdef _WIN32
	#include <windows.h>
	typedef DWORD pid_t;
	#define getpid GetCurrentProcessId
#else
	#include <sys/types.h>
#endif
#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>

unsigned int number_of_cores = std::thread::hardware_concurrency();
std::vector<pid_t> workers;
int m_listen_Port;

ProcessPool::ProcessPool() {
	try {
		if (number_of_cores == 0) {
			std::cout << "Could not detect number of cores, assuming 1" << std::endl;
			number_of_cores = 1;
		}
		boost::asio::io_context io_context(1);
		for (int i = 0; i < number_of_cores; i++) {
			workers[i] = create_worker(m_listen_Port, io_context);
		}
		boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
		signals.async_wait([&](auto, auto) { io_context.stop(); });

		io_context.run();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

};

ProcessPool& ProcessPool::getProcessPoolInstance(uint16_t port) {
	m_listen_Port = port;
	static ProcessPool processPool;
	return processPool;
}

ProcessPool::~ProcessPool() {
	// clean up if needed
}



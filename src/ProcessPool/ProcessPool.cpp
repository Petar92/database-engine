#include <iostream>
#include "ProcessPool.h"
#include <pthread.h>
#include <sched.h>

unsigned int number_of_cores = std::thread::hardware_concurrency();
int available_cores;
std::vector<std::thread> workers;
int m_listen_Port;

ProcessPool::ProcessPool() {
	try {
		if (number_of_cores == 0) {
			std::cout << "Could not detect number of cores, assuming 1" << std::endl;
			number_of_cores = 1;
		}
		available_cores = number_of_cores;
		int current_core = 0;
		for (int i = 0; i < number_of_cores; i++) {
			std::atomic<bool> shouldStop{false};
			std::thread thread(execute_command, available_cores, i, std::ref(shouldStop));
			cpu_set_t cpu_set;
			CPU_ZERO(&cpu_set);
			CPU_SET(i, &cpu_set);
			if (pthread_setaffinity_np(thread.native_handle(), sizeof(cpu_set), &cpu_set) > 0) {
				available_cores--;
				shouldStop = true;
        		thread.join();
			}
			else {
				thread.join();
				std::cout << "Thread " << i << " set to core " << current_core << std::endl;
				workers[current_core] = thread;
				current_core++;
			}
		}
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



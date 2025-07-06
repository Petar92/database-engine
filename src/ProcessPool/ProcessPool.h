#pragma once

#include <iostream>
#include <vector>
#include "ProcessPool.h"

class ProcessPool {
private:
	unsigned int number_of_cores;
	int available_cores;
	std::vector<std::thread> workers;
	int m_listen_Port;
	ProcessPool();
	~ProcessPool();
public:
	static ProcessPool& getProcessPoolInstance(uint16_t port);
};


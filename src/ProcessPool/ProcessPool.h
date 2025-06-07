#pragma once

#include <mutex>
#include <queue>
#include "Worker.h"

class ProcessPool {
private:
	unsigned int number_of_cores;
	std::vector<pid_t> workers;
	int m_listen_Port;
	ProcessPool();
	~ProcessPool();
public:
	static ProcessPool& getProcessPoolInstance(int listenfd, int addrlen);
};


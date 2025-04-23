#ifndef PROCESS_POOL_H
#define PROCESS_POOL_H

#include <mutex>
#include <queue>
#include "Worker.h"

class ProcessPool {
private:
	static constexpr size_t MAX_POOL_SIZE = 10; // get this value from config instead of 10
	std::mutex mtx;
	std::queue<Worker*> availableWorkers;
	ProcessPool();
	~ProcessPool();
	void cleanupAvailableWorkers();
public:
	static ProcessPool& getProcessPoolInstance();
	Worker* acquireProcess();
	void returnProcess(Worker* worker);

	ProcessPool(const ProcessPool&) = delete;
	ProcessPool& operator = (const ProcessPool&) = delete;
};
#endif 


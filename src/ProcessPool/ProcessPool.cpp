#include <queue>
#include "ProcessPool.h"
#include "Worker.h"

static constexpr size_t MAX_POOL_SIZE = 10; // get this value from config instead of 10
std::queue<Worker*> availableWorkers;
std::mutex mtx;
ProcessPool::ProcessPool() {
	for (int i = 0; i < MAX_POOL_SIZE; i++) {
		availableWorkers.push(new Worker());
	}
};
ProcessPool::~ProcessPool() {
	while (!availableWorkers.empty()) {
		delete availableWorkers.front();
		availableWorkers.pop();
	}
}
void cleanupAvailableWorkers() {
	while (availableWorkers.size() > MAX_POOL_SIZE) {
		delete availableWorkers.front();
		availableWorkers.pop();
	}
}

ProcessPool& ProcessPool::getProcessPoolInstance() {
	static ProcessPool processPool;
	return processPool;
}

Worker* ProcessPool::acquireProcess() {
	std::lock_guard<std::mutex> lk(mtx);
	if (availableWorkers.empty()) {
		return new Worker();
	}
	Worker* worker = availableWorkers.front();
	availableWorkers.pop();
	return worker;
}

void ProcessPool::returnProcess(Worker* worker) {
	std::lock_guard<std::mutex> lk(mtx);
	availableWorkers.push(worker);
	cleanupAvailableWorkers();
}

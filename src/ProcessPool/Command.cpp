#include "Command.h"

void Command::execute(std::string& command) {
	auto& processPool = ProcessPool::getProcessPoolInstance();
	auto* worker = processPool.acquireProcess();
	auto result = worker->executeCommand(command);
	if (result == 0) {
		processPool.returnProcess(worker);
	}
}
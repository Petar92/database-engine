#ifndef WORKER_H
#define WORKER_H

#include <string>

class Worker {
public:
	int createProcess();
	int executeCommand(std::string command);
};
#endif 

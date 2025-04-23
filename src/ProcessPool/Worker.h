#ifndef WORKER_H
#define WORKER_H

#include <string>

class Worker {
private:
	pid_t process;
public:
	Worker();
	~Worker();
	int executeCommand(const std::string& command);
};
#endif 

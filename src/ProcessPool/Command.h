#ifndef COMMAND_H
#define COMMAND_H

#include <string>
#include "ProcessPool.h"

class Command {
public:
	void execute(std::string& command);
};
#endif 

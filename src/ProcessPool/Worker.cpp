#include <iostream>
#include <string>
#include <vector>
#include "Worker.h"

void execute_command(int available_cores, int current_core, std::atomic<bool>& shouldStop) {
	if (shouldStop) {
        std::cout << "Attempting to set Thread affinity for core " << current_core << " failed, stopped before starting work\n";
        return;
    }
	std::cout << "Executing command on core " << current_core << " with " << available_cores << " available cores." << std::endl;
}




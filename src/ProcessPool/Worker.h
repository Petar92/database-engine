#pragma once

class Worker {
public:
	void execute_command(int available_cores, int current_core, std::atomic<bool>& shouldStop);
};
#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/wait.h> 
#include "Worker.h"

Worker::Worker() {
    auto pid = fork();
	if (pid < 0) {
		std::cerr << "fork failed\n";
	}
	else {
		process = pid;
	}
}

Worker::~Worker() {
	kill(process, SIGTERM);
	waitpid(process, nullptr, 0);
}

int Worker::executeCommand(const std::string& command) {
	if (process == 0) {
		const char* queryArgs[] = {
		  "./database_engine.sh", // executable to be run
		  "sample.db", // path to the db file
		  command.c_str(), // command to be executed
		  nullptr
		};
		execvp(queryArgs[0], const_cast<char* const*>(queryArgs));
		std::perror("execvp failed");
		_exit(EXIT_FAILURE);
	}
	else {
		int status = 0;
		if (waitpid(process, &status, 0) < 0)
			return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
	}

    return 0;
}

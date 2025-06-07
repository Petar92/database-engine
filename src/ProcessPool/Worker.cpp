#include <iostream>
#include <string>
#include <vector>
#include "Worker.h"

#ifdef _WIN32
	#include <windows.h>
	typedef DWORD pid_t;
	#define getpid GetCurrentProcessId
#else
	#include <sys/types.h>
#endif

pid_t create_worker(int listen_fd, int addr_len) {
	/*pid_t pid = fork();
	if (pid < 0) {
		std::cerr << "Error forking a new worker process" << std::endl;
	}
	else if (pid > 0) {
		return pid;
	}
	else {
		handle_request_from_child(listen_fd, addr_len);
	}*/
    return 0;
}



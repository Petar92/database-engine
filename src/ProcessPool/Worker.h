#pragma once

#include <sys/types.h>     

int create_worker(int listen_fd, int addr_len);

namespace {
	void handle_request_from_child(int listen_fd, int addr_len);
	void execute_command(const int& conn_fd);
}

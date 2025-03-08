#include <iostream>
#include "thread_pool.hpp"

std::mutex m;

inline void WAIT (int time) {
	std::this_thread::sleep_for(std::chrono::milliseconds(time));
}

void foo (int a, int b, int* res) {
	m.lock();
	*res += a + b;
	m.unlock();
}

int main () {
	const std::size_t thread_count = 8;
	thread_pool<thread_count>& th = thread_pool<thread_count>::get_pool();
	int res = 0;
	for (int i = 0; i < 100; ++i) {
		th.add_task(std::bind(foo, i, i * 2, &res));
	}
	th.wait_all_done();
	std::cout << "res: " << res << std::endl;
	for (int i = 0; i < 100; ++i) {
		th.add_task(std::bind(foo, i, i * 2, &res));
	}
	th.wait_all_done();
	std::cout << "res: " << res << std::endl;
	th.join_all();
}


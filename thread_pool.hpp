#ifndef THREAD_POOL
#define THREAD_POOL

#include <thread>
#include <vector>
#include <algorithm>
#include <mutex>
#include <chrono>
#include <queue>
#include <condition_variable>

template <std::size_t N>
class thread_pool {
public:
	using task = std::function<void()>;
private:
	std::condition_variable cv;
	std::queue<task> task_queue;
	std::vector<std::thread> threads;
	std::mutex m;
	bool stop_threads{false};
	bool tasks_are_done{true};

	thread_pool ();
	void do_task ();
public:
	thread_pool(const thread_pool&) = delete;
	thread_pool& operator=(const thread_pool&) = delete;
public:
	static thread_pool& get_pool ();
	~thread_pool();
public:
	void join_all();
	void detach_all();
	void add_task(task);
	void wait_all_done();
};

template <std::size_t N>
void thread_pool<N>::wait_all_done () {
	std::unique_lock<std::mutex> l(m);
	cv.wait(l, [this](){return this->tasks_are_done;});
}

template <std::size_t N>
void thread_pool<N>::add_task(task t) {
	std::lock_guard<std::mutex> l(m);
	task_queue.push(t);
	cv.notify_one();
	tasks_are_done = false;
}

template <std::size_t N>
thread_pool<N>::thread_pool () : threads(N) {
	if (N > std::thread::hardware_concurrency() - 1)
		throw std::invalid_argument("Thread count is too big");
	
	for (std::size_t i = 0; i < N; ++i) {
		threads[i] = std::thread(&thread_pool::do_task, this);
	}

}

template <std::size_t N>
void thread_pool<N>::do_task () {
	task t;
	while (!stop_threads) {
		std::unique_lock<std::mutex> l(m);
		cv.wait(l, [this](){
			return !this->task_queue.empty() || stop_threads; // condition for returning to process
		});
		if (stop_threads)
			break;
		t = task_queue.front();
		task_queue.pop();
		l.unlock();
		t();
		{
			std::lock_guard<std::mutex> l(m);
			if (task_queue.empty()) {
				tasks_are_done = true;
				cv.notify_all();
			}
		}
	}
}

template <std::size_t N>
thread_pool<N>& thread_pool<N>::get_pool () {
	static thread_pool tp;
	return tp;
}

template <std::size_t N>
void thread_pool<N>::join_all() {
	stop_threads = true;
	cv.notify_all();
	std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::join));
}

template <std::size_t N>
void thread_pool<N>::detach_all() {
	stop_threads = true;
	cv.notify_all();
	std::for_each(threads.begin(), threads.end(), std::mem_fn(&std::thread::detach));
}

template <std::size_t N>
thread_pool<N>::~thread_pool() {}

#endif // THREAD_POOL
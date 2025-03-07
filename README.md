# Thread Pool Implementation in C++

## Overview
This project provides a **thread pool** implementation in C++ that allows efficient task execution using a fixed number of threads. The implementation utilizes **mutexes, condition variables, and a task queue** to manage concurrent execution effectively.

## Features
- **Fixed-size thread pool**: The number of threads is set at compile time.
- **Task queue**: Allows adding and executing tasks asynchronously.
- **Synchronization**: Ensures tasks are completed before shutdown.
- **Singleton pattern**: Uses a static instance for global thread pool access.
- **Graceful shutdown**: Supports both `join_all()` and `detach_all()` methods.

## Code Structure
- **`thread_pool.hpp`**: Implementation of the thread pool class.
- **`main.cpp`**: Example usage of the thread pool.

## Usage
### 1. Create a thread pool
The thread pool is a **singleton**, so you can get the instance using:
```cpp
const std::size_t thread_count = 8;
thread_pool<thread_count>& th = thread_pool<thread_count>::get_pool();
```

### 2. Add tasks to the thread pool
Use the `add_task()` method to enqueue tasks for execution:
```cpp
int res = 0;
for (int i = 0; i < 100; ++i) {
    th.add_task(std::bind(foo, i, i * 2, &res));
}
```

### 3. Wait for all tasks to complete
Before proceeding, ensure all tasks are completed:
```cpp
th.wait_all_done();
```

### 4. Shutdown the thread pool
Call `join_all()` to gracefully stop all threads before exiting:
```cpp
th.join_all();
```

## Example
Here’s a complete example demonstrating how to use the thread pool:
```cpp
#include <iostream>
#include "thread_pool.hpp"

std::mutex m;

void foo(int a, int b, int* res) {
    std::lock_guard<std::mutex> lock(m);
    *res += a + b;
}

int main() {
    const std::size_t thread_count = 8;
    thread_pool<thread_count>& th = thread_pool<thread_count>::get_pool();
    int res = 0;
    
    for (int i = 0; i < 100; ++i) {
        th.add_task(std::bind(foo, i, i * 2, &res));
    }
    th.wait_all_done();
    
    std::cout << "res: " << res << std::endl;
    th.join_all();
    return 0;
}
```

## Build & Run
### Compilation (using g++)
```sh
g++ -std=c++11 main.cpp -o thread_pool_example
```

### Run the program
```sh
./thread_pool_example
```

## Requirements
- C++11 or later
- POSIX-compliant system (Linux/macOS) or Windows with MinGW

## License
This project is licensed under the **MIT License**.

## Author
[Tigran Yavroyan](https://github.com/TigranYavroyan)


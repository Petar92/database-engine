﻿# Journal Entry - June 3rd, 2025

I have invested quite some time in understanding how process creation and inter-process communication works, but the thing is, everything I have learned so far is only applicable on UNIX-like systems, and using C libraries which doesn't quite align with my goal to learn C++. So I have started looking into different options that would enable me to use C++ libraries for implementing a process pool, and I found them.

I discovered boost::asio and process libraries and studied them from the documentation. I even implemented some basic functionality and wanted to write about it, but I realized that even this is not quite what I was looking for. For example, the process creation using boost differs from using the fork() - exec() model, as with the boost library I would have to spawn a completely new process whereas in the fork() - exec() model it is a parent-child process relationship.

I wasn't sure how to proceed or what the right way was, so I decided to reconsider my approach. It seems that in order to implement this using the best modern C++ practices and in alignment with my goals, I would have to switch to implementing a thread pool instead of a process pool to handle incoming connection requests on my database server.

Another reason — although not a very important one right now — came from *Unix Network Programming* by W. Richard Stevens and Bill Fenner, where the authors compare various server architectures. In their tests, the pre-threaded server demonstrated the best performance in terms of process control CPU time and connection distribution per execution unit. (*Unix Network Programming* by W. Richard Stevens and Bill Fenner, Chapter 30, page 962)

In order to gain better understanding of multi-threading in C++, I read relevant chapters of the book *C++ Concurrency in Action* by Anthony Williams, which is a great resource for learning about multi-threading in C++.

## Key Takeaways from the Book

### Why Use Concurrency?
- **Separation of concerns** → multiple threads that handle multiple unrelated tasks concurrently/in parallel
- **Performance** → multicore processors can run multiple threads in parallel
	1. Algorithms that can take advantage of this by having one thread run one part of the algorithm and another thread run another part of the algorithm, the so-called "embarrassingly parallel" (note that embarrassingly is good in this context)
	2. Process multiple things at the same time, so instead of waiting for one thing to finish before starting another, you can start multiple things at the same time so that the total time to complete all tasks is significantly reduced

### Why Not to Use Concurrency?
- **Complexity** → concurrency adds complexity to the code, which can make it harder to understand and maintain, so it's a tradeoff between performance and complexity
- **Threads are a limited resource** → creating too many threads can lead to performance issues, so it's important to use them judiciously

### Concurrency in C++

#### History of Concurrency in C++
- Older C++ standards (C++11 and earlier) had limited support for concurrency. Compiler vendors added extensions to the language to support concurrency, which led to the prevalence of C APIs for multithreading (POSIX C standard, Microsoft Windows API), which then caused various platform-specific implementations of multithreading libraries
- Not content with using the platform-specific C APIs for handling multithreading, C++ programmers have looked to their class libraries to provide object-oriented multithreading facilities (MFC, Boost, ACE)
- The lack of standard support meant that there are occasions where the lack of a thread-aware memory model causes problems, particularly for those who try to gain higher performance by using knowledge of the processor hardware or for those writing cross-platform code where the actual behavior of the compilers varies between platforms
- C++11 introduced a standardized threading library, which provides a consistent API for multithreading across different platforms, making it easier to write portable code
- If you're after utmost performance, be aware of the implementation costs associated with using any high-level facilities, compared to using the underlying low-level facilities directly - "the abstraction penalty"

#### Using C++11 Threading Library

##### Basics
- `#include <thread>` - the header file that provides the threading facilities
- `std::thread` - the class that represents a thread of execution
- Every thread has to have a function to execute, an initial function, which is that thread's entry point and it can be a free function, a member function, or a lambda expression, an instance of a class with a function call operator, pretty much any callable object (for the main thread of the application, this is the main() function)
	Example: `std::thread t1(my_function);` // creates a new thread that runs my_function, and at this point there are 2 threads running: the main thread and the new thread t1
- Example of creating a thread that runs an instance of a class with a function call operator:
```cpp
class background_task { 
public:    
	void operator()() const {
		do_something(); 
		do_something_else();    
	} 
}; 
background_task f; 
std::thread my_thread(f);
```
- When we do this, we don't use the original object f, but rather a copy of it that is stored in the new thread's memory space

| If the functor holds…                            | What the copy means                                                                   |
| ------------------------------------------------ | ------------------------------------------------------------------------------------- |
| **No state** (as in the example)                 | Nothing to worry about. Both copies behave identically.                               |
| **Pointers / references** to data                | Both copies point to the **same** data → you must synchronize access.                 |
| **By-value members** (e.g. an `int`)             | The new thread sees its **own private copy** → changing it won't change the original. |
| **Move-only resources** (e.g. `std::unique_ptr`) | Pass the functor with `std::move`; otherwise compilation fails.                       |

- If we don't pass a named variable to the thread constructor, but rather a temporary, the compiler will interpret it as a function declaration rather than an object definition
	Example: `std::thread t1(my_function());` // this is a function declaration, not an object definition
- If we do this, the constructor will return a thread object rather than launching a new thread
- Ways to avoid that:
	1. Name the function object: `std::thread t1(my_function);` // this is a function object, not a function declaration
	2. Use an extra set of parentheses: `std::thread my_thread((background_task()));`
	3. Use the new uniform initialization syntax: `std::thread my_thread{background_task()};`
- If you want to avoid this problem whatsoever, you can use a lambda expression instead of a function object
	Example: `std::thread my_thread([](){ do_something(); do_something_else(); });`

##### Joining and Detaching Threads
- After a new thread is launched, the main thread continues to run concurrently with the new thread, so both threads can run at the same time and it can happen that the main thread finishes before the new thread, which can lead to undefined behavior if the new thread is still running when the main thread finishes
- In order to avoid this, you can use the join() method on the thread object, which blocks the main thread until the new thread finishes executing
	Example: `t1.join();` // blocks the main thread until t1 finishes executing
- Once a thread is started, we can either wait for it to finish using join() or detach it using detach(), which allows the thread to run independently of the main thread, but we won't be able to join it later
- If we don't join or detach a thread before the thread object is destroyed, the program will terminate with an exception
- If we don't wait for a thread to finish, then we need to ensure that the data accessed by the thread is valid until the thread finishes, otherwise we can end up with a dangling pointer or reference
- One way to handle this is to copy the data to the thread's memory space, so that the thread has its own copy of the data and doesn't access the original data, but we still need to be wary of objects containing pointers or references
- It is a bad practice to create a thread within a function that has access to local variables, because the thread will access the local variables after the function has returned, which can lead to undefined behavior
- The act of calling join cleans up the thread object, so we can't call join on a thread that has already been joined (std::thread object is no longer joinable), and calling joinable() would return false
- We should pay attention when deciding where to call join(). If an exception is thrown between the time the thread is created and the time join() is called, join() will be skipped
- We can use a try-catch block and put join() in the catch block to ensure that the thread is joined even if an exception is thrown
- Another way to handle this is to use a RAII-style class that automatically joins the thread when it goes out of scope, which is a good practice to ensure that the thread is always joined
- Detached threads (daemon threads) don't have a thread object associated with them. They run in the background, we don't have a way of communicating with them and they usually run for almost the entire lifetime of the application
- They perform background tasks, such as monitoring system resources, handling network connections, or performing periodic tasks
- We can only call `t.detach()` for a std::thread object t when `t.joinable()` returns true

##### Passing Arguments to a Thread Function
- Passing arguments to the callable object or function is fundamentally as simple as passing arguments to std::thread constructor
- It is very important to note that by default, the arguments are **copied into internal storage where they can be accessed by the newly created thread of execution, even if the corresponding parameter in the function is expecting a reference**
- The std::thread constructor is oblivious to the types of the arguments expected by the function and blindly copies the supplied values

##### Transferring Ownership of a Thread
- Many resource-owning types in the C++ Standard Library are movable but not copyable. `std::thread` is one of them
	Example:
```cpp
void some_function(); 
void some_other_function(); 
std::thread t1(some_function);        
std::thread t2=std::move(t1);              
t1=std::thread(some_other_function);   
std::thread t3;                  
t3=std::move(t2);               
t1=std::move(t3); 
```

- Most of the code here is pretty self-explanatory, but the last line is a bit tricky. What happens here is that the assignment will terminate the program because t1 already had an associated thread
- The reason for that is that we must explicitly wait for a thread to complete or detach it before destruction, and the same applies to assignment. We cannot just drop a thread by assigning a new value to the std::thread object that manages it

##### Choosing the Number of Threads
- `std::thread::hardware_concurrency()` returns the number of hardware threads available on the system, which is the number of threads that can run concurrently on the system
- This is a hint, not a guarantee, so it may return 0 if the number of hardware threads is unknown or if the system doesn't support it
- This information might be useful for the thread pool of my database server as it can help me decide at runtime how many threads to create without hardcoding values or passing them from the command line or from the environment
- We need to pay attention to exception handling when using this functionality

##### Identifying Threads
- Thread identifiers are of type `std::thread::id` and can be retrieved in two ways:
	1. By calling the `get_id()` method on the std::thread object
	2. By using the `std::this_thread::get_id()` function, which returns the ID of the current thread
- If a std::thread object doesn't have an associated running thread, `get_id()` returns a default-constructed `std::thread::id` object, which indicates "not any thread"

This is a summary of the first two chapters of the book. I will continue in the next entry.
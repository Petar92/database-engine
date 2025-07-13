# Journal Entry - June 11th, 2025

Moving on with Chapter 4, I feel like at this point I'm past the most important parts, chapter 4 deals with synchronizing concurrent operations and I will cover this in detail and the rest of the book I will just read without trying to understand everything like I did for these 4 chapters. Except for parts that deal with designing and implementing thread pools as I will need htat for my database server.

## Synchronizing concurrent operations
- C++ Standard Library provides facilities to handle synchronizing operations between threads in the form of condition variables and futures

## Condition Variables
- The most basic mechanism for waiting for an event to be triggered by another thread is the condition variable
- Conceptually, a condition variable is associated with some event or other condition, and one or more threads can wait for that condition to be satisfied. When some thread has determined that the condition is satisfied, it can then notify one or more of the threads waiting on the condition variable, in order to wake them up and allow them to continue processing
- Standard library provides two implementations of a condition variable:
  - std::condition_variable
  - std::condition_variable_any
Example:
```cpp
std::mutex mut;
std::queue<data_chunk> data_queue;  
std::condition_variable data_cond;
void data_preparation_thread() { 
	while(more_data_to_prepare()) {
		data_chunk const data=prepare_data(); 
		std::lock_guard<std::mutex> lk(mut);        
		data_queue.push(data);                     
		data_cond.notify_one();  
	} 
}
void data_processing_thread() { 
	while(true) {
		std::unique_lock<std::mutex> lk(mut);  
		data_cond.wait(           
			lk,[]{return !data_queue.empty();});   
		data_chunk data=data_queue.front(); 
		data_queue.pop();      
		lk.unlock();        
		process(data); 
		if(is_last_chunk(data))           
			break;
	}
}
```
This example is sort of a inter thread communication, the first thread prepares data and when it's ready, protects the queue using std::lock_guard then it pushes it on the queue and then notifies the other thread using the std::condition_variable.
The second thread locks the mutex (using std::unique_lock) and then it waits on the std::condition_variable, passing in the lock and a lambda function that expresses the condition being waited for (in this case that the queue is not empty).

## Futures
- If a thread needs to wait for a specific one-off event, it somehow obtains a future representing this event
- The thread can then periodically wait on the future for short periods of time to see if the event has occurred while performing some other task in between checks
- Alternatively, it can do another task until it needs the event to have happened before it can proceed and then just wait for the future to become ready
- A future may have data associated with it, or it may not
- Once an event has happened (and the future has become ready), the future can’t be reset

Example:
```cpp
// Traditional approach - no easy way to get result
void calculate_in_thread() {
    std::thread t([]() {
        int result = complex_calculation();
        // How do we return this result??
    });
    t.join();
}

// With futures - clean result handling
std::future<int> calculate_with_future() {
    return std::async(std::launch::async, []() {
        return complex_calculation();  // Result will be available in the future
    });
}
```

- There are two sorts of futures in the C++ Standard Library, declared in the <future> library header: unique futures (std::future<>) and shared futures (std::shared_future<>)

1. std::future (Unique Future)

Only ONE future can wait for a specific result
Like having the only boarding pass for your seat
```cpp
std::future<int> fut = get_future_for_calculation();
int result = fut.get();  // Moves the result out
fut.get();  // ERROR! Can't get twice from same future
```

2. std::shared_future (Shared Future)

Multiple futures can wait for the same result
Like multiple family members each having info about the same flight
```cpp
std::shared_future<int> sfut = get_future_for_calculation().share();
std::shared_future<int> sfut2 = sfut;  // Copy is OK

// Multiple threads can each get the result
int result1 = sfut.get();   // OK
int result2 = sfut2.get();  // OK - same value
```

Usage Example:
```cpp
// WRONG - Unsynchronized access to same future
std::future<int> fut = get_future();
std::thread t1([&fut]() { fut.wait(); });  // Race condition!
std::thread t2([&fut]() { fut.wait(); });  // Race condition!

// RIGHT - Each thread has its own shared_future copy
std::shared_future<int> sfut = get_future().share();
std::thread t1([sfut]() { sfut.wait(); });  // OK - copy
std::thread t2([sfut]() { sfut.wait(); });  // OK - copy
```
Another example:
```cpp
#include <future>
#include <iostream>
#include <thread>

// Function that takes time to compute
int expensive_computation(int x) {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    return x * x;
}

int main() {
    // Launch async computation
    std::future<int> fut = std::async(std::launch::async, 
                                     expensive_computation, 10);
    
    // Do other work while computation runs
    std::cout << "Doing other work..." << std::endl;
    
    // Two ways to check/wait:
    
    // Option 1: Periodically check (like checking departure board)
    while (fut.wait_for(std::chrono::milliseconds(100)) 
           != std::future_status::ready) {
        std::cout << "Still waiting..." << std::endl;
        // Do something else
    }
    
    // Option 2: Just wait until ready
    // int result = fut.get();  // Blocks until result is ready
    
    std::cout << "Result: " << fut.get() << std::endl;  // 100
}
```

## Returning values form background tasks
This addresses the chapter 2 limitation:
```cpp
// Chapter 2 problem - no easy return value
std::thread t([]() {
    int result = 42;
    // How to return this?
});

// Chapter 4 solution - futures!
std::future<int> fut = std::async(std::launch::async, []() {
    return 42;  // Easy!
});
int result = fut.get();  // Clean retrieval
```
## Associating a task with a future
- std::packaged_task<> ties a future to a function or callable object
- When the std:: packaged_task<> object is invoked, it calls the associated function or callable object and makes the future ready, with the return value stored as the associated data
- *This can be used as a building block for thread pools* (important, I will need to come back to this when I start implementing my database server)
- It is sort of a wrapper that connects a callable (function/lambda) to a future, but doesn't execute it immediately, it's like it's "packaging up" work to be done later
- The return type of the specified function signature identifies the type of the std::future<> returned from the get_future() member function
Example:
```cpp
#include <future>
#include <thread>

int calculate(int x) {
    return x * x;
}

int main() {
    // std::async - executes immediately
    std::future<int> f1 = std::async(std::launch::async, calculate, 10);
    // Function is already running in background!
    
    // std::packaged_task - packages but doesn't execute
    std::packaged_task<int(int)> task(calculate);
    std::future<int> f2 = task.get_future();
    // Function hasn't run yet!
    
    // Execute when YOU want
    task(10);  // NOW it runs
    
    std::cout << "Result: " << f2.get() << std::endl;  // 100
}
```

## std::promise
- std::promise<T> provides a means of setting a value (of type T), which can later be read through an associated std::future<T> object
Use Case:
- Problem: Using one thread per network connection wastes resources and causes excessive context switching when handling many connections (100s-1000s)
- Solution: Use a small number of threads (possibly one) where each thread handles multiple connections in a loop
- Challenge: Data arrives randomly from different connections, but other threads need specific responses from specific connections
- std::promise/std::future provides a communication channel: waiting threads block on futures while the network thread sets values via promises
```cpp
std::promise<payload_type> p;
std::future<payload_type> f = p.get_future();  // Get future before moving promise
p.set_value(data);  // Sets value and wakes up thread waiting on f.get()
```
- Incoming Data Flow: Network thread receives data with an ID, looks up the corresponding promise, and sets the payload as its value
```cpp
if(connection->has_incoming_data()) {
    data_packet data = connection->incoming();
    std::promise<payload_type>& p = connection->get_promise(data.id);
    p.set_value(data.payload);  // Wakes up waiting thread
}
```
- Outgoing Data Flow: Network thread sends queued data and sets a boolean promise to indicate successful transmission
```cpp
if(connection->has_outgoing_data()) {
    outgoing_packet data = connection->top_of_outgoing_queue();
    connection->send(data.payload);
    data.promise.set_value(true);  // Signals "send complete"
}
```
- Promise Destruction Rule: If a promise is destroyed without setting a value, an exception is automatically stored in the associated future
- Exception Handling: Promises can store exceptions (covered in section 4.2.4), allowing errors to be propagated across threads just like normal return values
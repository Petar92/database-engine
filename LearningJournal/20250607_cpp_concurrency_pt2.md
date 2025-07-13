# Journal Entry - June 7th, 2025

Picking up where I left off. I will be reading through Chapter 3 of the book "C++ Concurrency in Action" by Anthony Williams. This chapter focuses on the topic of "Sharing data between thread" and is probably the most important chapter, at least in my oppinion.

## Race condition:
-  Any situation where the outcome depends on the relative ordering of execution of operations on two or more threads; the threads race to perform their respective operations
	Example: Two persons buying the last ticket, which one gets the ticket depends on the relative ordering of the two purchases
- Data race - a specific type of race condition that arises beacuse of concurrent modification of a single object and this can cause undefined behavior
- Because race conditions are generally timing sensitive, they can often disappear entirely when the application is run under the debugger, because the debugger affects the timing of the program, even if only slightly.
- Investigate if I can apply Deterministic Simulation Testing to catch these exceptions early as shown in the [Resonate HQ presentatoin about DST](https://journal.resonatehq.io/p/how-we-test-the-resonate-server)

## Avoiding race conditions
- The simplest option is to wrap your data structure with a protection mechanism, to ensure that only the thread actually performing a modification can see the intermediate states where the invariants are broken
- Another option is to modify the design of your data structure and its invariants so that modifications are done as a series of indivisible changes, each of which preserves the invariants. This is generally referred to as lock-free programming and is difficult to get right
- Finally, we could handle the updates to the data structure as a transaction, just as updates to a database are done within a transaction (software transactional memory), currently under research and doesn't have support in C++

## Mutexes
- Most general of the data-protection mechanisms
- It’s important to structure the code to protect the right data 
- Mutexes come with the problems in the form of a deadlock and protecting either too much or too little data
- Mutex creation:
	- create an instance of std::mutex
	- lock it using the lock() member function
	- unlock it using the unlock() member function
	- not recommended to use the lock() and unlock() member functions directly, instead use RAII wrappers like std::lock_guard or std::unique_lock (locks the mutex on construciton and unlocks on destruction)
	Example:
```cpp
#include <list>
#include <mutex>
#include <algorithm>
 
std::list<int> some_list;
std::mutex some_mutex;

void add_to_list(int new_value) {
	std::lock_guard<std::mutex> guard(some_mutex);
	some_list.push_back(new_value);
}
bool list_contains(int value_to_find){
	std::lock_guard<std::mutex> guard(some_mutex);
	return std::find(some_list.begin(), some_list.end(), value_to_find) != some_list.end();
}
```
- in majority of real-world examples, we would group the mutex and the data in a class rather than use global vaiables, that is, the two functions would be member functions of the class and the mutex and the shared data (some_list) would become private
- there is one issue with this approach, **if one of the member functions returns a pointer or a reference to the shared data, then it doesn't matter that all member functions lock the mutex, because any caller can now access the protected data without locking the mutex**
- with that being said, it is also very important **not to pass pointers or references to the shared data in to functions that they [our member functions] call, that are not under our control**
- moral of the story: **never pass pointers or references to shared data that is protected by a mutex outside of the scope of the lock, whether by returning them from a function, storing them in externaly visible memory, or passing them as arguments to user-supplied functions, always return copies of the data instead**

## Race conditions even when using mutex
- Just because we are using mutexes doesn't mean that our shared data (data structures) are protected from race conditions - **protect entire operations, not just individual steps**
- one example is the standard stack implementation which works find for single-threaded applications, but in a multi-threaded application, it is possible that due to the ordering of operations we might end up with unexpected behavior
- pop() and top() opeartions -> single step or two step operation?

	### Possible solutions:

	#### Option 1: pass the shared data (in whatever shape and form it comes) by reference

	Example:
	```cpp
	// Modified pop that takes a reference parameter
	void pop(T& value) {
        std::lock_guard<std::mutex> lock(m);
        if(data.empty()) {
            throw std::runtime_error("empty stack");
        }
        value = data.top();  // Assignment happens here
        data.pop();
    }		
	```
	problems with option 1:
	- requires pre-construction of the shared data before passing it to the pop() funciton
	Example:
	```cpp
	T shared_data;
	stack.pop(shared_data);
	```
	- not only that it's an extra step but pre-construction can be expensive and even wastefull
	Example:
	```cpp
	ExpensiveObject shared_data; // Allocates a lot of memory (relative but let's say 1 MB)
	stack.pop(shared_data); // if the stack is empty, we waisted all the allocated memory for nothing
	```
	- some types don't have default constuctors
	Example:
	```cpp
	DatabaseConnection shared_data; // This causes an error because DatabaseConnection doesn't have a default constructor
	DatabaseConnection shared_data("localhost", "root", "password"); // This works but it's pointless
	stack.pop(shared_data); //  shared_data gets overwritten
	```
	- some types are non-assignable
	Example:
	```cpp
	class NonAssignable {
		std::unique_ptr<int> shared_data; // Can't be copied or assigned
	};
	```

	#### Option 2: allow pop() to return by value but require a no-throw copy constuctor or move constructor (that is, copy or move constructors are guaranteed not to throw exceptions)
	Example:
	```cpp
	// Can safely return by value - won't throw during return
	T pop() {
		std::lock_guard<std::mutex> lock(m);
		if (data.empty()) {
			throw std::runtime_error("empty stack");
		}
		T value = std::move(data.top()); // Move if possible, copy otherwise
		data.pop();
		return value; // This return won't throw
	}
	```
	Besides this implementation of the pop() function, this class makes sure that the type is safe at compile time by using a static_assert to check if the type is either copyable or movable:
	```cpp
	static_assert(
		std::is_nothrow_copy_constructible<T>::value ||
		std::is_nothrow_move_constructible<T>::value,
		"Type T must be either nothrow copy or move constructor"
	);
	```
	pros of this approach:
	- the value is safely copied/moved before being removed from the stack
	- either we succeed and return the value or we throw an exception earlier (in case of an empty stack)
	problems with this approach:
	- there are many more user-defined types with copy constructors that can throw and don’t have move constructors than there are types with copy and/or move constructors that can’t throw
	- this implementation would be unusable for those types

	#### Option 3: return a pointer to the popped item (rather than return by value)

	Example:
	```cpp
	std::shared_ptr<T> pop() {
		std::lock_guard<std::mutex> lock(m);
		if (data.empty()) {
			throw std::runtime_error("empty stack");
		}
		std::shared_ptr<T> result = data.top(); // Copty ptr, never throws
		data.pop();
		return result; // Return ptr, never throws
	}
	```
	pros of this approach:
	- pointers can be freely copied without throwing an exception 
	- using stsd::shared_ptr is an excellent choice beacause it ensures that the memory is managed automatically, so we don't have to worry about memory leaks, besides that, the library is in full control of the memory allocation scheme and doesn’t have to use new and delete
	
	problems with this approach:
	- returning a pointer requres a means of managing the memory allocated to the object, for example for integers, the overhead of memory management can exceed the cost of just returning by value

	#### Option 4: Combination of option 1 with options 2 or 3

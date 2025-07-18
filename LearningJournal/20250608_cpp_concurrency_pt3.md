﻿# Journal Entry - June 8th, 2025

I will still be going through Chapter 3 today. I am getting a bit annoyed that I am not doing any practiucal work but I believe that a strong foundation when it comes to concurrency will help me in th elong run.

## Deadlock
- Multiple threads, having separate mutex locks, and waiting for each other to release the locks
- The most common approach to solving this is to always lock the mutexes in the same order, if you always lock mutex A before mutex B, then you will never have a deadlock
- This is not always possible, for example, when the mutexes are each protecting a separate instance of the same class,if two threads to try to exchange data between the same two instances with the parameters swapped

Example:
```cpp
// Thread 1 does buf1.exchange(buf2): locks buf1 then buf2
std::thread t1(threadFunc, std::ref(buf1), std::ref(buf2));

// Thread 2 does buf2.exchange(buf1): locks buf2 then buf1
std::thread t2(threadFunc, std::ref(buf2), std::ref(buf1));
```

- C++ Standard Library has a solution for this, std::lock is a function that can lock two or more mutexes at once without risk
- Deadlocks can also happen without locks, all it takes is two threads calling .join() on each other's std::thread objects

## Recommendations on how to avoid deadlocks
- Don't acquire a lock if you're already holding one
- If you really need multiple locks, use std::lock
- Avoid calling user-supplied code while holding a lock
- Use a lock hierarchy

### Lock Hierarchy
- Each mutex is assigned a "numerical level" or "hierarchy value"
- You can only lock a mutex if its hierarchy value is lower than any mutex you currently hold (not even of the same hierarchy value)
- This prevents circular dependencies that cause deadlocks 
- Hierarchical_mutex is not part of the standard library, however, if it implements the thre member functions required to satisfy the mutex concept (lock(), unlock(), try_lock()), it can be used in the same way as any other mutex
- Simple implementation of hierarchical_mutex can be found in the book on page 52/53

Example (for the full code, refer to the book on page 51):
```cpp
hierarchical_mutex high_level_mutex(10000);  // Highest level
hierarchical_mutex low_level_mutex(5000);    // Lower level  
hierarchical_mutex other_mutex(100);         // Lowest level
```
Thread A (follows the rules - works fine):
```cpp
void thread_a() {
	high_level_function(); // 
}
// high_level_func locks high_level_mutex (10000)
// then calls low_level_func which locks low_level_mutex (5000)
// 10000 → 5000 is descending, so it's allowed ✓
```
Thread B (violates the rules - fails):
```cpp
	void thread_b() {
	std::lock_guard<hierarchical_mutex> lk(other_mutex);  // Locks mutex with value 100
	other_stuff();  // Calls high_level_func()
}

// thread_b first locks other_mutex (100)
// then tries to lock high_level_mutex (10000) inside high_level_func()
// 100 → 10000 is ascending, so it's FORBIDDEN ✗
// This will throw an exception!
```
Why This Prevents Deadlocks?
Deadlocks require circular dependencies:

Thread 1: holds A, wants B
Thread 2: holds B, wants A

With hierarchy:

If A has value 10000 and B has value 5000
Thread 1: locks A(10000), then B(5000) ✓
Thread 2: locks B(5000), then tries A(10000) ✗ FORBIDDEN!

The circular dependency is impossible because both threads must follow the same order (high to low)

Important Details

- Storing previous values: When unlocking, it restores the previous hierarchy value so you can lock higher-level mutexes again after releasing all locks
- Works with std::lock_guard: Because hierarchical_mutex implements lock(), unlock(), and try_lock(), it satisfies the "Lockable" concept
- Runtime checking: This catches violations at runtime, not compile time, but it's deterministic - violations are caught immediately, not just during rare conditions

The beauty is that designing the system with hierarchy levels forces us to think about lock ordering during design, often preventing deadlock issues before they're even coded

## Flexible locking wuth std::unique_lock
- std::unique_lock provides a bit more flexibility than std::lock_guard
- std::unique_lock instance doesn’t always own the mutex that it’s associated with
- pass std::adopt_lock as a second argument to the constructor to have the lock object manage the lock on a mutex
- pass std::defer_lock as the second argument to indicate that the mutex should remain unlocked on construction
- lock can then be acquired later by calling lock() on the std::unique_lock object (not the mutex) or by passing the std:: unique_lock object itself to std::lock()
Example:
```cpp
std::mutex m1, m2;

void transfer_money(Account& from, Account& to, int amount) {
    // std::lock locks both mutexes without deadlock
    std::lock(m1, m2);
    
    // Both are now locked, adopt ownership for RAII
    std::lock_guard<std::mutex> lock1(m1, std::adopt_lock);
    std::lock_guard<std::mutex> lock2(m2, std::adopt_lock);
    
    // Do transfer...
}  // Both automatically unlocked
```
- The flexibility of allowing a std::unique_lock instance not to own the mutex comes at a price: this information has to be stored, and it has to be updated
Examples showing difference between std::lock_guard and std::unique_lock:

- **std::lock_guard - no extra storage needed**
```cpp
template<typename Mutex>
class simple_lock_guard {
    Mutex& m;  // Just stores a reference to the mutex
public:
    explicit simple_lock_guard(Mutex& mutex) : m(mutex) {
        m.lock();  // Always locks
    }
    ~simple_lock_guard() {
        m.unlock();  // Always unlocks
    }
};
```

**std::unique_lock - must track ownership**
```cpp
template<typename Mutex>
class simple_unique_lock {
    Mutex* m;           // Pointer to mutex (can be null)
    bool owns;          // Do we currently own the lock?  <-- EXTRA STORAGE!
    
public:
    explicit simple_unique_lock(Mutex& mutex) 
        : m(&mutex), owns(true) {  
        m->lock();
    }
    
    // Can create without locking
    simple_unique_lock(Mutex& mutex, std::defer_lock_t)
        : m(&mutex), owns(false) {  // Note: owns = false
        // Don't lock
    }
    
    void lock() {
        if (m && !owns) {
            m->lock();
            owns = true;    // Must UPDATE ownership status
        }
    }
    
    void unlock() {
        if (m && owns) {
            m->unlock();
            owns = false;   // Must UPDATE ownership status
        }
    }
    
    ~simple_unique_lock() {
        if (owns) {         // Must CHECK before unlocking
            m->unlock();
        }
    }
};
```

**extra memory**
```cpp
sizeof(std::lock_guard<std::mutex>)   // Typically 8 bytes (just a pointer)
sizeof(std::unique_lock<std::mutex>)  // Typically 16 bytes (pointer + bool + padding)
```

**extra opeartions**
```cpp
// lock_guard destructor
~lock_guard() {
    mutex.unlock();  // Just unlock, no checks
}

// unique_lock destructor  
~unique_lock() {
    if (owns_lock()) {  // Must check ownership first!
        mutex->unlock();
    }
}
```

**every operation must update state**
```cpp
void example() {
    std::unique_lock<std::mutex> lock(m, std::defer_lock);
    
    lock.lock();      // Sets owns = true
    lock.unlock();    // Sets owns = false
    lock.lock();      // Sets owns = true again
    
    // Transfer ownership
    auto lock2 = std::move(lock);  
    // Must update: lock.owns = false, lock2.owns = true
}
```

**trade-off**
```cpp
// lock_guard: Simple and fast
{
    std::lock_guard<std::mutex> lock(m);  // Always: lock
    // work
}  // Always: unlock

// unique_lock: Flexible but with overhead
{
    std::unique_lock<std::mutex> lock(m);  // Lock + set owns=true
    if (need_to_unlock) {
        lock.unlock();  // Unlock + set owns=false
    }
}  // Check owns flag, conditionally unlock
```

**Rule of thumb** -> use lock_guard by default, unique_lock when we need flexibility

## Locking at an appropriate granularity
- Where possible, lock a mutex only while actually accessing the shared data; try to do any processing of the data outside the lock. In particular, don’t do any really time-consuming activities like file I/O while holding a lock
- std::unique_lock works well in this situation, because you can call unlock() when the code no longer needs access to the shared data and then call lock() again if access is required later in the code

Example:
```cpp
void get_and_process_data() {
    std::unique_lock<std::mutex> my_lock(the_mutex);   
    some_class data_to_process=get_next_data_chunk();   
    my_lock.unlock();                                     
    result_type result=process(data_to_process);    
    my_lock.lock();                                
    write_result(data_to_process,result);
}
```

## Reader-writer mutex
- exclusive access by a single "writer" thread
- shared, concurrent access by multiple "reader" threads
- not part of the standard libary, boost has implementation

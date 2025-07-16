# Journal Entry - July 16th, 2025

I have removed almost everything from my old project and decided to start over with a clean project.
Since I decided to use boost asio on the client, I finally implemented a simple tcp server that is able to accept connections and receive queries in the form of a abstract syntax tree.
Here are some things I learned from the boost asio documentation that I used extensively to implement my client and server:

### boost asio

#### execution context
- every asio program needs an I/O execution context (io_context, thread_pool, etc.), it acts as the core event loop and dispathcer, all async operations are registered through it
- timers and sockets are tied to an execution context, so we must passs an io_context object when constructing them, this tells asio when and how to manage operations
- we can think of it as the front desk of a hotel, every guest (I/O task) checks in here, and the desk knows what to do when it's time
- we must call the io_context::run() member function on the io_context object.
- asio library provides a guarantee that completion handlers will only be called from threads that are currently calling io_context::run(), therefore unless the io_context::run() function is called the completion handler for the asynchronous wait completion will never be invoked.

#### sync vs async operations
- this is classic blocking vs non-blocking behavior, we need something like a timer and we can call a blocking timer.wait() or non-blocking operation timer.async_wait()
- we have to pass in the execution context 
- using asynchronous functionality means supplying a completion token, which is usually a lambda but can be any callback (e.g. a future), so we say wait for something and when the wait is completed invoke the completition token
- async waits can loop (self scheduling), initially we call it from somewhere else, and then inside the callback call itself again
- use boost::bind or lambdas to pass extra arguments since asio callbacks normally only get the error code, to address this we can use boost::bind (or better, lambdas) to attach extra arguments.
    usage:
```cpp
timer.async_wait(boost::bind(print, _1, &timer, &count));
```

#### asynchronous TCP server implementation
- in order to turn the server name that was specified as a parameter to the application, into a TCP endpoint we use an ip::tcp::resolver object
- resolver takes a host name and service name and turns them into a list of endpoints
- ip::tcp::acceptor object needs to be created to listen for new connections
- important thing to note is that when we call async_accept on the acceptor (which is the same thing as calling async_wait on the timer we mentioned above) we capture the new_connection in the lambda in order to keep it alive until the client connects and then, when a connection is accepted, handle_accept() is called
- if we didn't capture it in the lambda we risk new_connection being destroyed before a client connects and the socket becomes invalid which results in undefined behavior

    Example:

```cpp
acceptor_.async_accept(new_connection->socket(), ...);
// then `new_connection` goes out of scope and is deleted
```

- we need to create a server object to accept incoming client connections. The io_context object provides I/O services, such as sockets, that the server object will use
- function start_accept() creates a socket and initiates an asynchronous accept operation to wait for a new connection
- function handle_accept() is called when the asynchronous accept operation initiated by start_accept() finishes, it services the client request, and then calls start_accept() to initiate the next accept operation
- we use shared_ptr and enable_shared_from_this because we want to keep the tcp_connection object alive as long as there is an operation that refers to it, in order to achieve this we make the tcp_connection constructor private and return the instance (shared_pointer to the instance) from the create method
- in the function start(), we call boost::asio::async_write() to serve the data to the client
- in this function (start()), we take advantage of "shared_from_this()" in order to extend the lieftime of the tcp connection object until write completes in order to prevent the object from being destroyed mid-opearation, for example, if we used [this] and the object got deleted, the lambda would cause a crash
- the data to be sent is stored in the class member message_ as we need to keep the data valid until the asynchronous operation is complete
- any further actions for this client connection are now the responsibility of handle_write()

And I think that's it, it explains what I did, most of the code is from the boost asio tutorial that I adjusted to fit my use case. 
I will have to think about what the next step would be, maybe use the boost asio thread pool instead of my own or maybe I should just move on to the fun stuff, e.g. query planner or the memory buffer. I'll have to think about it, I'll probably go with the query planner.
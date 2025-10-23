# cppws - C++ based HTTP/HTTPS server

This project is intended as a learning experience in web development and server infrastructures.
The intention is to create a web framework (similar to others such as Rest) that is compatible with
C++20 programming.

## Framework structure

The inspiration for the framework itself is to be taken from the Java SpringBoot libraries. This means
that the end user of the framework will be assigning _handlers_ to a set of URL "endpoint mappings" that
determine what handler gets what request.

Requests are to be managed using either separate _threads_ or possibly a custom _fiber_ implementation.
This is in order to ensure that all requests are guaranteed some execution time.

### Fork on new request

One other possible solution is to assign a separate process to every request. This would have a number
of benefits as opposed to threaded request handlers:

1. Request handlers are guaranteed to not access each other's memory.
2. Handlers are significantly easier to sandbox (potentially one container per request).
3. Crashes in one handler does not propagate to the server as a whole (`SIGSEGV`, `std::terminate`, etc.).

This solution would however come with quite a few problems as well:

1. Potentially a lot of overhead from launching new processes.
2. Having to deal with inter-process communication.
3. More difficult to terminate the server.
4. More work to mitigate leaked processes (zombies, or runaway processes).

### New thread on new request

This is the most common solution, and comes with it's own set of benefits:

1. Thread pooling could potentially reduce overhead as opposed to other methods.
2. Generally more lightweight than separate processes.
3. Simpler intercommunication between processes.

Potential downsides are:

1. Shared memory between request handlers.
2. Potentially more limited in number of simultaneous connections.

### One fiber per request

This is an extension of the threaded version of request handling, and may increase the number of requests
that can be handled at once. The main problem here however, is that it comes with added complexity, as you
now also need to manage scheduling for each individual fiber/task.

Moreover, it could also significantly increase the complexity of I/O handling, as you would need to ensure
that the fiber thread is not blocked on read/write.

To summarize, the potential benefits are:

1. Potentially higher capacity than pure threaded method.
2. More control over request scheduling (custom priorities, etc.)
3. Would be a good learning experience.

Downsides being:

1. Memory (heap) is still shared between requests.
2. Added complexity.
3. A poor implementation may be very slow (if a fiber blocks on I/O tasks scheduled on that thread will starve).

### Choice of language

Choosing C++ as the language of choice for this was done due to the following benefits:

- It exposes lower level APIs for interacting with the web.
- Allows for more freedom in implementing things like the runtime environment for the server.
- It is a language that I, personally, have experience in.



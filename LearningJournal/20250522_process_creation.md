# Journal Entry - May 22nd, 2025

It's been a while since I wrote anything, but the reason for that is that I thought it would take me a lot less time to figure out everything I wanted to do. Truth is, even today (**May 22nd**), after reading so much and coming up with different approaches and changes to my approaches, I still haven't decided what exactly I want to do.

So let's document what I have learned so far.

> From now on, I will include dates of my entries, as it would be cool to keep an actual journal.  
> I've also decided I won't be using AI to make summaries anymore, as I find that I reinforce my learning by doing this myself.  
> I will still use AI to create markdown and check for spelling errors.

---

As already mentioned in the previous entry, I decided to go with the **process pool** for handling my connections. And so I reached my first major obstacle — the one that delayed my work for so long as I had to research extensively.

My knowledge of systems programming and low-level OS stuff is very limited, so I had to refresh my fundamentals. I remembered a brilliant book recommended by a friend a while ago that I read but never had the chance to put to practical use — until now.

The book in question is *Operating Systems: Three Easy Pieces* by Remzi H. Arpaci-Dusseau and Andrea C. Arpaci-Dusseau (University of Wisconsin-Madison).  
A physical copy of the book is available for purchase, but there is also the [book website](https://pages.cs.wisc.edu/~remzi/OSTEP/) which is completely free.

## Key Learnings

### Chapter 4: Process Abstraction

This chapter is a great refresher (or a learning resource if you know little about the topic). Since I needed to create processes for my process pool and had no idea how to do so, this was a great starting point.

### Chapter 5: Process API

This is where I learned how to create a process in UNIX systems using `fork()`.

#### Main takeaways from using `fork()`:

- `fork()` returns a new process PID to the parent.
- `fork()` returns a PID of 0 to the child.
- The child process is an (almost) exact copy of the parent process.
- To the OS, it looks like there are two copies of the parent program running.
- If we want the parent to wait for the child process to finish, we can use `wait()` or `waitpid()`, which makes our code deterministic.
- `fork()` by itself lets us run the same program twice, but we usually need the child to run a different program — for that we use `exec()`.
- `exec()` loads the code from the executable we want to run and overwrites the current code segment and static data.
- The heap, stack, and other parts of the memory space are re-initialized, and then the OS runs the new program.
- Long story short, `exec()` doesn't create a new process but transforms the currently running one into a different program.
- After calling `exec()` in the child, it's almost as if the original process never ran.
- A successful call to `exec()` never returns.

---

Now this is a good starting point, but in order to do anything useful, the parent and child processes need to communicate with each other.

For example, one of the ways servers are handling new connection request is by creating a pool of child processes and having the parent accept the request and then use inter process communication to communicatie with child processes. This is where we can use a mechanism in UNIX called a **pipe**.
Another way is to have a blocking accept by each child but in this case they use the socket's file descriptor.

---

*To be continued...*

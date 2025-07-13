# Concurrency Models for On-Disk Database Access

## Context

When designing how our engine accesses and manages the on‑disk database file, we considered three primary models for handling concurrency:

1. **Process per DBMS Worker**  
   - Easy isolation  
   - Heavy resource usage  
   - Requires shared memory for buffer pools  

2. **Thread per DBMS Worker**  
   - Lightweight context switching  
   - Complex synchronization  

3. **Process Pool**  
   - Fixed or dynamic pool of processes  
   - Balances overhead and isolation  

After evaluating the trade‑offs, we chose **Process Pool** along with the **Object Pool** design pattern for efficient resource management.

## Design Decision

The process‑per‑worker model provides strong isolation at the cost of spawning new OS processes for each connection, leading to high memory and CPU overhead. The thread‑per‑worker model reduces process creation cost but introduces synchronization challenges around shared data structures. A process pool presents a middle ground, reusing a set of long‑lived processes and avoiding the overhead of repeated creation and teardown.

## Object Pool Pattern

To implement the process pool, we adopted the Object Pool design pattern as outlined in the Sourcemaking article:  
https://sourcemaking.com/design_patterns/object_pool

This pattern allows us to:

- Initialize a pool of pre‑allocated `Worker` instances (processes)  
- Reuse idle workers without expensive creation  
- Configure maximum pool size to control resource usage  
- Automatically clean up excess workers

## Implementation Details

The implementation resides in the `ProcessPool` folder under the `source` directory and includes the following files:

- `ProcessPool.h` and `ProcessPool.cpp`  
- `Worker.h` and `Worker.cpp`  
- `Command.h`  

Key highlights:

- `ProcessPool::getProcessPoolInstance()` returns a singleton instance.  
- `ProcessPool::acquireProcess()` provides an idle `Worker` or creates a new one if the pool is empty.  
- `ProcessPool::returnProcess(Worker* worker)` adds the worker back to the pool and enforces `MAX_POOL_SIZE`.  

## Lessons Learned

- **Singleton Management**: Using Meyers’ Singleton ensures thread‑safe initialization without explicit locking.  
- **Resource Control**: A configurable `MAX_POOL_SIZE` prevents unbounded resource growth.  
- **Error Handling**: Worker processes should include robust error detection and reporting for command execution failures.

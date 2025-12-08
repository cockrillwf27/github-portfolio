# Cache Lab Design

## Overview
This project implements a **direct-mapped cache** with the following characteristics:

- **Cache size:** 1 KB (1024 bytes / 256 words)  
- **Block size:** 4 words per block (16 bytes per block)  
- **Number of blocks:** 64 (256 words ÷ 4 words per block)  
- **Write policy:** Write-back  
- **System components:** Cache, CPU, and main memory (simulated for testing)  

The goal of this lab is to design a functional cache using a hierarchical SystemVerilog structure, including a **cache controller** and a **cache data array**.

---

## Memory Addressing
A 32-bit memory address is divided as follows:

| Field         | Bits     | Purpose                                           |
|---------------|---------|--------------------------------------------------|
| Byte offset   | [1:0]   | Selects byte within a word (always 00 for word accesses) |
| Block offset  | [3:2]   | Selects a word within a 4-word cache block      |
| Index         | [9:4]   | Selects the cache block in the cache data array |
| Tag           | [31:10] | Identifies the memory block stored in the cache |

---

## Cache Controller
The **cache controller** manages:

- Tag array and Valid/Dirty bits  
- Determining cache hits and misses  
- Interfacing with main memory  

### Inputs
- `clk`, `reset` – clock and reset signals  
- `addr` – 32-bit memory address from the CPU  
- `cpuRead`, `cpuWrite` – CPU control signals  
- `memReady` – indicates memory readiness  

### Outputs
- `stall` – stalls the CPU on a cache miss  
- `memRead`, `memWrite` – signals to main memory  
- `memAddr` – 32-bit memory address for memory operations  
- `refill` – signals cache data array to store a 4-word block  
- `update` – signals cache data array to store a 32-bit word and mark the block as dirty  

The controller is implemented as a **Mealy finite state machine (FSM)** with the following states:

- **IDLE:** No cache access or hit; CPU is not stalled  
- **MISS:** Cache miss; load block from memory and stall CPU  
- **WRITE_BACK:** Write dirty block to memory before refill; CPU is stalled  

---

## Cache Data Array
The cache data array handles:

- **READ_HIT:** Selects the correct word from the 4-word block using a 4:1 multiplexer  
- **WRITE_HIT:** Updates a word in the block using a 2:4 decoder and multiplexer; marks block as dirty  
- **WRITE_BACK:** Outputs the dirty block to memory  
- **MISS:** Stores the block received from memory and marks it as valid and clean  

---

## Testing
The lab includes a **testbench** simulating a CPU and main memory:

- The CPU module generates read/write requests  
- The memory module models access delays using `memReady`  
- The testbench verifies the correctness of read and written data  

The test scenario mimics a matrix multiplication program, performing repeated reads and writes to memory locations to test cache hit, miss, and write-back behavior.

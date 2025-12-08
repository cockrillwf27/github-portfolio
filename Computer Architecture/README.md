# Computer Architecture 

---

## Overview

This collection represents the culmination of two major hardware design efforts:

### **1. A Complete 32-bit Pipelined CPU**
A fully functional five-stage pipelined CPU capable of executing arithmetic, logical, memory, branch, and subroutine instructions. The design includes:

- **Combinational ALU** with status flags  
- **32-bit register file** with falling-edge write  
- **Control unit** supporting immediate, register, load/store, branch, BL, and BR instructions  
- **Instruction memory + PC** with sequential and branch updating  
- **Data memory** with word-aligned LDR/STR  
- **5 pipeline stages:** IF, ID, EX, MEM, WB  
- **Hazard detection unit** (forwarding + stalling + branch control)  
- **NOP instruction** for safe pipeline execution  
- **Support for loops, functions, recursion, and array processing**

This CPU successfully ran programs such as array summation and Fibonacci generation in simulation.

---

### **2. Direct-Mapped Write-Back Cache**
A 1KB cache built to interface with a simulated CPU and memory bus.  
Key properties:

- **1 KB total size (256 words)**  
- **4-word blocks → 64 total blocks**  
- **Write-back + write-allocate policy**  
- **Valid, Dirty, and Tag arrays**  
- **Cache controller FSM** with 3 states:  
  - `IDLE`  
  - `WRITE_BACK`  
  - `MISS`  
- **Refill and update logic** for block loading and word writes  
- **Full testbench** simulating matrix multiplication memory patterns

This cache accurately handled hits, misses, dirty evictions, and refills.

---

## Skills Demonstrated

- Hardware design in **SystemVerilog**
- Building combinational + sequential circuits
- Microarchitectural design (datapath + control)
- Pipeline design + hazard resolution
- Cache hierarchy + memory interfacing
- Testbench writing and simulation analysis
- Debugging large hierarchical hardware systems

---

## Included Documentation

Both folders contain:

- Full SystemVerilog module implementations  
- Extensive testbenches  
- Machine code programs for CPU validation  
- Waveform outputs (VCD)  
- Reference schematics (if added to repo)  


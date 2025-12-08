# Final CPU Design – SystemVerilog Project

This project presents the design and implementation of a complete 32-bit CPU in SystemVerilog, developed as the culmination of a multi-part lab series. The CPU integrates a wide range of hardware design concepts, including combinational and sequential logic, control units, instruction and data memory, branching, subroutine support, and pipelining with hazard detection. The design was thoroughly tested using simulated CPU and memory models, and verified against a set of machine code programs and test vectors.

## Overview

The CPU is a fully functional model capable of executing arithmetic, logical, memory, and branch instructions. Its architecture is modular and hierarchical, consisting of:

- **ALU (Arithmetic Logic Unit)** for computation
- **Register File** for storing general-purpose registers
- **Control Unit** for instruction decoding and signal generation
- **Instruction Memory and Program Counter** for instruction fetching
- **Data Memory** for load and store operations
- **Pipeline Stages** including IF, ID, EX, MEM, and WB
- **Hazard Detection and Forwarding Unit** to manage data and control hazards

![CPU Schematic](misc/projects/images/CPU.png)

The CPU executes instructions using a 32-bit instruction format, supports immediate and register-based operations, and implements branching, subroutine calls, and returns.

## ALU

The ALU is a combinational module responsible for all arithmetic and logical operations. It supports:

- **Addition**
- **Subtraction**
- **AND**
- **OR**

The ALU outputs a 32-bit result along with status flags:

- **N (Negative)**: Set if the result is negative  
- **Z (Zero)**: Set if the result is zero  
- **C (Carry)**: Set for unsigned overflow on add/subtract  
- **V (Overflow)**: Set for signed overflow on add/subtract  

The ALU is fully combinational, with no internal storage, and was tested using supplied and custom test vectors to verify correct operation across positive, negative, and zero values.

## Register File

The register file stores the CPU’s general-purpose registers. Features include:

- Simultaneous read of two registers
- Conditional write on the falling edge of the clock
- Registers initialized with known values for testing, ensuring predictable behavior during arithmetic, logical, and memory operations

## Control Unit

The Control Unit decodes instructions and generates all necessary control signals for the CPU. It supports:

- Arithmetic and logical operations with immediate and register operands
- Load (LDR) and store (STR) instructions
- Branch instructions, including conditional branches (**BEQ**, **BNE**), unconditional branches (**B**), and subroutine calls/returns (**BL**, **BR**)
- Pipeline control, including stall and forwarding signals for hazard management

The control unit drives the ALU, register file, memory accesses, and multiplexer selections throughout the pipeline.

## Instruction and Data Memory

The CPU includes separate modules for instruction memory and data memory:

- **Instruction Memory**: Holds the program code, initialized from a machine code file. The Program Counter (PC) determines the instruction address, and a simple adder updates the PC sequentially.
- **Data Memory**: Supports word-aligned load (LDR) and store (STR) instructions. Memory writes are monitored in the testbench to ensure correctness.

## Pipeline Design

The CPU implements a five-stage pipeline:

1. **Instruction Fetch (IF)**
2. **Instruction Decode/Register Read (ID)**
3. **Execution (EX)**
4. **Memory Access (MEM)**
5. **Write Back (WB)**

Pipeline registers store intermediate signals between stages. Forwarding muxes are used in the EX stage to resolve data hazards, and the Hazard Detection Unit inserts stalls when necessary. A **NOP** instruction is included to safely handle pipeline delays.

## Branching and Subroutines

The CPU supports:

- Conditional and unconditional branches based on ALU flags
- Subroutine calls and returns via the **BL** and **BR** instructions
- Result and PC muxes to select between sequential execution, branch targets, and subroutine return addresses

This enables the CPU to execute loops, recursive functions, and subroutine calls, verified using programs such as array summation and Fibonacci number calculation.

## Testing and Verification

The CPU was extensively tested using a SystemVerilog testbench simulating both CPU and memory behavior. Testing included:

- ALU operations with various edge cases
- Arithmetic and logical instructions with immediate and register operands
- Memory operations using test programs that store and load array values
- Branching and subroutine calls, including loops and function calls
- Pipeline correctness with hazard detection and forwarding

All tests were verified using machine code programs and test vectors. The CPU was validated to produce correct results for all operations.

## Key Features

- Fully pipelined 32-bit CPU
- Combinational ALU with full status flags
- 32-bit register file with conditional write
- Control unit supporting immediate, register, load/store, branch, and subroutine instructions
- Instruction and data memory modules
- Branch and subroutine support
- Forwarding and hazard detection in the pipeline
- Tested using comprehensive machine code programs, including loops and Fibonacci number calculation

This final CPU project demonstrates proficiency in digital design, SystemVerilog coding, pipeline architecture, control logic, memory interfacing, and verification methodology, making it a strong centerpiece for a hardware design portfolio.

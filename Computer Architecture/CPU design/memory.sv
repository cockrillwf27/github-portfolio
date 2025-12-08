// Instruction memory -- a read-only memory
// Our memory is byte-addressable, but we assumne that all
// accesses are word (32-bit) aligned. In other words, we ignore
// the lower two bits of address.
module imem(input  logic [31:0] addr,
            output logic [31:0] data);
  
  // array to hold the words
  logic [31:0] mem[0:255];		// up to 256 words for now
  
  initial begin
    // load the initial program
    $readmemh("machineCode.txt", mem);
  end
  
  // read the data
  assign data = mem[addr[31:2]];	// ignore lower 2-bits of addr
  
  
endmodule


// Data memory -- a read/write memory
// Our memory is byte-addressable, but we assumne that all
// accesses are word (32-bit) aligned. In other words, we ignore
// the lower two bits of address.
module dmem(input  logic        clk,
            input  logic        we,
            input  logic [31:0] addr,
            input  logic [31:0] wd,
            output logic [31:0] rd
       );
  
 
  
  logic [31:0] mem[0:255];		// up to 256 words for now
  
  // write on rising edge of clock
  always_ff @(posedge clk) begin
    if (we) mem[addr[31:2]] <= wd;		// ignore lower 2-bits of addr
  end
  
  // read the data
  assign rd = mem[addr[31:2]];		// ignore lower 2-bits of addr
endmodule

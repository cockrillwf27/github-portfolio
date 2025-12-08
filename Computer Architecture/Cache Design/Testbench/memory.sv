// This module _simulates_ the memory and interface to the
// cache controller. Each memory access will cause a delay
// until the memory is again ready.
//
// Notice that we only simulate a 16K memory, focusing mostly
// on the timing (fixed delay when reading / writing). The memory
// is initially filled with values to represent 3 30x30 arrays of ints

module memory(input  logic         clk,
              input  logic         reset,
              input  logic         read,
              input  logic         write,
              input  logic [31:0]  addr,
              input  logic [127:0] writeBlock,
              output logic [127:0] readBlock,
              output logic         ready);
  
  localparam MEM_DELAY = 'd100;     // delay to access memory
  
  logic [127:0] mem[1023:0];		// 4K words = 16KB
  
  logic request;
  assign request = read | write;

  
  // look for a reset or new request -- delay until we're ready again
  always @(posedge reset or posedge request) begin
    if (reset) begin
      $readmemh("memContents.txt", mem);
      ready = 1'b1;
    end
    else begin
      // write?
      if (write) begin
        // delay and then write
        ready = 1'b0;
        #(MEM_DELAY-1);
        
        // map to 4 words in memory
        mem[addr[13:4]] <= writeBlock;
        #1;
        ready = 1'b1;
      end
      
      // read?
      else if (read) begin
        ready = 1'b0;
        #MEM_DELAY;
        ready = 1'b1;
      end
    end
  end
  
  // put data on the read output from 4 words  
  assign readBlock = mem[addr[13:4]];
endmodule

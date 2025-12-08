// Register file -- 16 regs, each 32 bits
module regfile(input  logic       clk,       // clock
               input  logic        reset,	 // reset
               input  logic        we3,      // write enable
               input  logic [3:0]  ra1,      // address 1
               input  logic [3:0]  ra2,      // address 2
               input  logic [3:0]  wa3,      // address 3
               input  logic [31:0] wd3,      // write data
               output logic [31:0] rd1,      // read value 1
               output logic [31:0] rd2       // read value 2
              );
  
  // register file
  logic [31:0] rf[15:0];
  
  // three ported register file.
  // Read two ports combinationally
  // and write third port on rising clock edge
   
  
  always_ff @(negedge clk) begin
    if (~reset) begin
      if (we3) rf[wa3] <= wd3;
    end
  end

  
  assign rd1 = rf[ra1];
  assign rd2 = rf[ra2];
  

  
endmodule

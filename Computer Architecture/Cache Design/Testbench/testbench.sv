// Testbench for cache controller
`include "memory.sv"

module cache_tb();
  
  logic clk;
  logic reset;
  
  // signals to connect to memory
  logic memReady;
  logic memRead;
  logic memWrite;
  logic [31:0] memAddr;
  logic [127:0] memReadBlock;
  logic [127:0] memWriteBlock;
  
  // signals to connect to CPU
  logic cpuRead;
  logic cpuWrite;
  logic [31:0] cpuAddr;
  logic [31:0] cpuWriteData;
  logic [31:0] cpuReadData;
  logic stall;
  
  // statistics
  integer numTests;
  integer misses;
  integer vectorNum = 0;
  logic [35:0] testVectors[9999:0];
  logic flag;
  
  // verify  memory accesses
  integer memErrors;
  logic [31:0] expectedCpuReadData;
  
  localparam INIT = 2'b0;
  localparam IDLE = 2'b01;
  localparam MEM_REQ = 2'b10;
  localparam STALLED = 2'b11;
  
  logic [1:0] state, nextState;
  
  // instantiate the simulated memory
  memory mem(.clk, .reset, .read(memRead), .write(memWrite),
             .addr(memAddr), .writeBlock(memWriteBlock),
             .readBlock(memReadBlock), .ready(memReady));
  
  // instantiate the cache
  cache uut(.clk, .reset, .cpuRead, .cpuWrite, .cpuAddr,
            .cpuWriteData, .cpuReadData, .stall,
            .memReady, .memRead, .memWrite, .memAddr,
            .memReadBlock, .memWriteBlock);
  
  // generate the clock
  always begin
    clk = 1; #5;
    clk = 0; #5;
  end
  
  // stop if we run for too long
  always begin
    #1_000_000;
    $display("ERROR -- Maximum clock cycles exceeded.");
    $display("%d of %d tests complete with %d cache misses",
                 vectorNum, numTests, misses);
    $finish;
  end
    
  // at start of test, load test vectors and pulse reset
  initial begin
    $dumpfile("dump.vcd");
    $dumpvars;
      
    $readmemh("memTrace.txt", testVectors);
    
    // figure out how many tests there are
    numTests = 0;
    while (testVectors[numTests][0] !== 1'bx)
      numTests++;
    
    misses = 0;
    flag = 1'b0;
    
    memErrors = 0;
    
    reset = 1; #22; reset = 0;
  end

  // update state on falling edge of clk
  always @(negedge clk) begin
    if (reset) begin
      state <= INIT;
    end
    else begin
      state <= nextState;
    end
  end
  
  // next-state logic -- updates after state changes on falling edge
  always_comb begin
    case (state)
      INIT:	nextState = MEM_REQ;
      IDLE: nextState = MEM_REQ;
      MEM_REQ: nextState = (stall) ? STALLED : IDLE;
      STALLED: nextState = (stall) ? STALLED : IDLE;
      default: nextState = INIT;
    endcase
  end
  
  // output logic -- updates after state changes on falling edge
  always_comb begin
    if ((state == INIT) || (state == IDLE)) begin
      cpuRead = 1'b0;
      cpuWrite = 1'b0;
      cpuAddr = 32'b0;
      cpuWriteData = 32'b0;
      expectedCpuReadData = 32'b0;
    end
    
    else begin
      cpuRead = ~testVectors[vectorNum][32];
      cpuWrite = testVectors[vectorNum][32];
      cpuAddr = {16'b0, testVectors[vectorNum][31:16]};
      cpuWriteData = {16'b0, testVectors[vectorNum][15:0]};
      expectedCpuReadData = {16'b0, testVectors[vectorNum][15:0]};
    end
  end    
  
  // on rising edge, check for stalls
  always @(posedge clk) begin
    if (~reset) begin
      #1;   // wait a little past edge
      
      // are we done?
      if (testVectors[vectorNum][0] === 1'bx) begin
        $display("%d of %d tests complete with %d cache misses", 
                 vectorNum, numTests, misses);
        
        $display("%d memory errors", memErrors);
        
        $finish;
      end
      
      else begin
        // for reads, check if we got the correct value
        if (cpuRead && ~stall) begin
          if (cpuReadData !== expectedCpuReadData) begin
            memErrors++;
            
            $display("Incorrect memory data from address %h -- %h (expected %h)",
                     cpuAddr, cpuReadData, expectedCpuReadData);
          end
        end
      end
    end
  end
  
  // on rising edge, increment miss count
  always @(posedge clk) begin
    if (~reset) begin
      if ((state == MEM_REQ) && (stall))
        misses++;
      else if (state == IDLE)
        vectorNum++;
    end
  end
endmodule
                          

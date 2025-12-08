// Code your design here
// Direct-mapped cache with write-back, 
// with sub-modules for controller and data array.

`include "util.sv"

module cache(input  logic         clk,
             input  logic         reset,
             input  logic         cpuRead,    // read request from CPU
             input  logic         cpuWrite,   // write request from CPU
             input  logic [31:0]  cpuAddr,    // request addr from CPU
             input  logic [31:0]  cpuWriteData, // data from CPU
             input  logic         memReady,   // if memory is ready
             input  logic [127:0] memReadBlock, // block read from memory
             output logic         stall,      // stall the CPU
             output logic [31:0]  cpuReadData, // data read from cache
             output logic [31:0]  memAddr,    // addr to memory
             output logic         memRead,    // read enable to memory
             output logic         memWrite,   // write enable to memory
             output logic [127:0] memWriteBlock // data to write to mem
            );
  
  logic refill;       // refill data array
  logic update;       // udpate word in data array
  
  cacheController cc(.clk, .reset, .addr(cpuAddr), .cpuRead, .cpuWrite,
                     .memReady, .stall, .memRead, .memWrite, .memAddr,
                     .refill, .update);
  
  dataArray da(.clk, .addr(cpuAddr), .cpuDataIn(cpuWriteData),
               .memDataIn(memReadBlock), .refill, .update,
               .cpuDataOut(cpuReadData), .memDataOut(memWriteBlock));
  
endmodule



// Cache controller operates as a FSM to coordinate transfers
// between memory and data array. The controller also manages
// the tags and the valid and dirty bits.

module cacheController(input  logic        clk,
                       input  logic        reset,
                       input  logic [31:0] addr,   // addr from CPU
                       input  logic        cpuRead, // read request
                       input  logic        cpuWrite, // write request
                       input  logic        memReady, // mem is avail
                       output logic        stall,    // stall CPU
                       output logic        memRead,  // read block
                       output logic        memWrite, // write block
                       output logic [31:0] memAddr,  // addr to memory
                       output logic        refill,   // to data array
                       output logic        update    // to data array
                      );
  
  
  

  logic [5:0] index;
  logic [21:0] tag;
  assign index = addr[9:4];
  assign tag = addr[31:10];
  logic [21:0] tagArray   [0:63];
  logic validArray [0:63];
  logic dirtyArray [0:63];
  logic [21:0] cacheTag;
  logic valid;
  logic dirty;
  logic hit;
  logic read_hit, write_hit;
  logic miss_dirty, miss_clean;

  
  
  always_comb begin
    cacheTag = tagArray[index];
    valid = validArray[index];
    dirty = dirtyArray[index];
    hit = valid && (cacheTag == tag);
    read_hit = cpuRead  && hit;
    write_hit = cpuWrite && hit;
    miss_dirty = (cpuRead || cpuWrite) && !hit && dirty;
    miss_clean = (cpuRead || cpuWrite) && !hit && !dirty;
  end

  
  
  
  typedef enum logic [1:0] {IDLE, WRITEBACK, MISS} stateType;
  
  stateType ThisState, NextState;
  

  always_ff @(posedge clk) begin
    if (reset) ThisState <= IDLE;
    else ThisState <= NextState;
  end

  

  always_comb begin
    stall = 1'b0;
    memRead = 1'b0;
    memWrite = 1'b0;
    refill = 1'b0;
    update = 1'b0;
    memAddr = 32'bx;
    NextState = ThisState;

    case (ThisState)
      IDLE: begin
        if (read_hit) begin
          stall = 1'b0;
        end
        else if (write_hit) begin
          stall = 1'b0;
          update = 1'b1;
        end
        else if (miss_dirty) begin
          stall = 1'b1;
          memWrite = 1'b1;
          memAddr = {cacheTag, index, 4'b0000};
          NextState = WRITEBACK;
        end
        else if (miss_clean) begin
          stall = 1'b1;
          memRead = 1'b1;
          memAddr = {tag, index, 4'b0000};
          NextState = MISS;
        end
      end

      WRITEBACK: begin
        stall = 1'b1;
        memWrite = 1'b1;
        memAddr = {cacheTag, index, 4'b0000};
        if (memReady) begin
          memRead = 1'b1;
          memAddr = {tag, index, 4'b0000};
          NextState = MISS;
        end
      end

      MISS: begin
        stall = 1'b1;
        memRead = 1'b1;
        memAddr = {tag, index, 4'b0000};
        if (memReady) begin
          refill = 1'b1;
          NextState = IDLE;
        end
      end

      default: begin
        NextState = IDLE;
      end
    endcase
  end

  

  always_ff @(posedge clk) begin
     if (reset) begin
      for (int i = 0; i < 64; i++) begin
        tagArray[i] <= 22'b0;
        validArray[i] <= 1'b0;
        dirtyArray[i] <= 1'b0;
      end
    end else begin 
      if (refill) begin
        tagArray[index] <= tag;
        validArray[index] <= 1'b1;
        dirtyArray[index] <= cpuWrite; 
      end else if (update) begin
        dirtyArray[index] <= 1'b1;     
      end
    end
  end

endmodule





// Data array holds array of 128-bit (4 word) values.
// Refill operation transfers entire 128-bit block from memory
// Update operation updates one word of block from CPU write
module dataArray(input logic clk,
                 input logic [31:0] addr, // addr from CPU
                 input logic [31:0] cpuDataIn, // write data from CPU
                 input logic [127:0] memDataIn, // block from memory
                 input logic refill, // from controller
                 input logic update, // from controller
                 output logic [31:0] cpuDataOut, // read data to CPU
                 output logic [127:0] memDataOut // block to memory
      			 );
  
  
  logic [127:0] dataBlocks [0:63] = '{default:128'b0};  
  logic [127:0] currentBlock, updateBlock;
  logic [5:0] index;
  logic [1:0] offset;
  logic [3:0] select;
  logic [31:0] mux1Result, mux2Result, mux3Result, mux4Result;

  assign index  = addr[9:4];
  assign offset = addr[3:2];

  assign currentBlock = dataBlocks[index];
  assign memDataOut = currentBlock;


  decoder2_4 dec(offset, select);
  mux2 #(32) Mux1(currentBlock[127:96], cpuDataIn, select[3], mux1Result);
  mux2 #(32) Mux2(currentBlock[95:64],  cpuDataIn, select[2], mux2Result);
  mux2 #(32) Mux3(currentBlock[63:32],  cpuDataIn, select[1], mux3Result);
  mux2 #(32) Mux4(currentBlock[31:0],   cpuDataIn, select[0], mux4Result);

  assign updateBlock = {mux1Result, mux2Result, mux3Result, mux4Result};


  mux4 #(32) MuxRead(currentBlock[31:0], currentBlock[63:32], currentBlock[95:64],							 currentBlock[127:96], offset, cpuDataOut);


  always_ff @(posedge clk) begin
    if (refill)
      dataBlocks[index] <= memDataIn;
    else if (update)
      dataBlocks[index] <= updateBlock;
  end

endmodule



  
  
  
  

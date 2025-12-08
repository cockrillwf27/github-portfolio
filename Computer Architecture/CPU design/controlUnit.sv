// Control unit for the VMI CPU
module controlUnit(input  logic [7:0] opcode,
                   output logic       regWrite,
                   output logic       aluSrcA,
                   output logic       aluSrcB,
                   output logic [1:0] aluControl,
                   output logic [1:0]      resultSRC,
                   output logic       MemWrite,
                   output logic       branch,
                   output logic       branchEQ, 
                   output logic       branchNE,
                   output logic       branchReg);
  

  localparam ADD_R = 8'h01;
  localparam ADD_I = 8'h02;
  localparam SUB_R = 8'h03;
  localparam SUB_I = 8'h04;
  localparam AND_R = 8'h05;
  localparam AND_I = 8'h06;
  localparam OR_R = 8'h07;
  localparam OR_I = 8'h08;
  localparam MOV_R = 8'h09;
  localparam MOV_I = 8'h0a;
  localparam LDR = 8'h0b;
  localparam STR = 8'h0c;
  localparam b = 8'h0d;
  localparam beq = 8'h0e;
  localparam bne = 8'h0f;
  localparam bl = 8'h10;
  localparam br = 8'h11;
  localparam NOP = 8'h00;
  

  
  // combined control signals
  logic [11:0] controls;
  
  assign {regWrite, aluSrcA, aluSrcB, aluControl, resultSRC, MemWrite, branch, branchEQ, branchNE, branchReg} = controls;
  
 
  always_comb
    case (opcode)
      ADD_R: controls = 12'b1_00_00_00_0_0000;
      ADD_I: controls = 12'b1_01_00_00_0_0000;
      SUB_R: controls = 12'b1_00_01_00_0_0000;
      SUB_I: controls = 12'b1_01_01_00_0_0000;
      AND_R: controls = 12'b1_00_10_00_0_0000;
      AND_I: controls = 12'b1_01_10_00_0_0000;
      OR_R: controls = 12'b1_00_11_00_0_0000;
      OR_I: controls = 12'b1_01_11_00_0_0000;
      MOV_R: controls = 12'b1_10_00_00_0_0000;
      MOV_I: controls = 12'b1_11_00_00_0_0000;
      LDR: controls = 12'b1_01_00_01_0_0000;
      STR: controls = 12'b0_01_00_00_1_0000;
      b: controls = 12'b0_0_0_00_00_0_1000;
      beq: controls = 12'b0_0_0_01_00_0_0100;
      bne: controls = 12'b0_0_0_01_00_0_0010;
      bl: controls = 12'b1_0_0_01_10_0_1000;
      br: controls = 12'b0_0_0_00_0_0_00001;
      NOP: controls = 12'b0_0_0_00_0_0_000;
      
      default: controls = 10'bx;

    endcase
endmodule

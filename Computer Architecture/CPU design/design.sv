`include "regfile.sv"
`include "alu.sv"
`include "extend.sv"
`include "util.sv"
`include "controlUnit.sv"
`include "memory.sv"
`include "hazardUnit.sv"

module cpu(input logic clk,
           input logic reset); 
  
  
 
  
  //instruction variables
  logic [31:0] instr_F;
  logic [31:0] instr_D;
                             
 //control unit variables   
  logic BranchReg_D;
  logic BranchReg_E;
  logic Branch_D;
  logic Branch_E;
  logic BranchEQ_D;
  logic BranchEQ_E;
  logic BranchNE_D;
  logic BranchNE_E;
  logic regWrite_D;
  logic regWrite_E;
  logic regWrite_M;
  logic regWrite_W;
  logic [1:0] resultSRC_D;
  logic [1:0] resultSRC_E;
  logic [1:0] resultSRC_M;
  logic [1:0] resultSRC_W;
  logic memWrite_D;
  logic memWrite_E;
  logic memWrite_M;
  logic [1:0]  aluControl_D;
  logic [1:0]  aluControl_E;   
  logic aluSrcA_D;
  logic aluSrcA_E;
  logic aluSrcB_D; 
  logic aluSrcB_E;                 
                                                     
 //regfile variables
  logic [31:0] regA_D;
  logic [31:0] regA_E;
  logic [31:0] regB_D;
  logic [31:0] regB_E;
  logic [31:0] regB_M;
  logic [3:0] rd_D;
  logic [3:0] rd_E;
  logic [3:0] rd_M;
  logic [3:0] rd_W;
  logic [3:0] rn_D;
  logic [3:0] rn_E;
  logic [3:0] rm_D;
  logic [3:0] rm_E;
  
  //extend unit variables
  logic [31:0] uimm32_D;
  logic [31:0] uimm32_E;
  logic [31:0] imm32_D;
  logic [31:0] imm32_E;
                    
  //alu variables 
  logic [3:0] flags;
  logic [31:0] srcA, srcB; 
  logic [31:0] aluResult_E;
  logic [31:0] aluResult_M;
  logic [31:0] aluResult_W;
  
  
  //mem variables
  logic [31:0] memResult_M;
  logic [31:0] memResult_W;
  
  logic [31:0] muxResult;
  
  
  //variables for pc reg
  logic [31:0] pcPlus4_F;
  logic [31:0] pcPlus4_D;
  logic [31:0] pcPlus4_E;
  logic [31:0] pcPlus4_M;
  logic [31:0] pcPlus4_W;
  logic [31:0] pc;
  logic [31:0] pcNext;
  logic [31:0] pcBranch;
  logic [1:0] PCsrc;
  
  //hazard variables 
  logic stall;
  logic [1:0] forwardA;
  logic [1:0] forwardB;
  logic branchTaken;
  
  logic [31:0] regA_fwd;
  logic [31:0] regB_fwd;
  
  
 
  
  
  mux4 mux4pc(.d0(pcPlus4_F), .d1(pcBranch), .d2(regA_fwd), .d3(),
              .s(PCsrc), .y(pcNext));
  
  
  register_ren register_r_inst(.clk(clk), .reset(reset), .en(~stall),
                               .d(pcNext), .q(pc));
   
  assign pcPlus4_F = pc + 32'h4;
                             
  imem im(.addr(pc), .data(instr_F));
  
                             
    
  //IFID register 
  register_ren#(64) IFID(.clk(clk), .reset(reset||branchTaken), .en(~stall), 
                .d({instr_F, pcPlus4_F}), 
                .q({instr_D, pcPlus4_D}));
  
  
  
  
                             
  controlUnit cU(.opcode(instr_D[31:24]), .regWrite(regWrite_D),
                 .aluSrcA(aluSrcA_D), .aluSrcB(aluSrcB_D),
                 .aluControl(aluControl_D),.resultSRC(resultSRC_D),
                 .MemWrite(memWrite_D), .branch(Branch_D),
                 .branchEQ(BranchEQ_D), .branchNE(BranchNE_D),
                 .branchReg(BranchReg_D));
  
  
  regfile rf(.clk(clk),.reset(reset),.we3(regWrite_W),.ra1(instr_D[19:16]),
             .ra2(instr_D[15:12]), .wa3(rd_W), .wd3(muxResult), .rd1(regA_D),
             .rd2(regB_D));
  
  
  extendUnit eU(.imm12(instr_D[11:0]), .uimm32(uimm32_D), .imm32(imm32_D));
  
  assign rd_D = instr_D[23:20];
  assign rn_D = instr_D[19:16];
  assign rm_D = instr_D[15:12];
  
  
  
   //IDEX register
  register_r#(184) IDEX(.clk(clk), .reset(reset||stall||branchTaken), 
                .d ({BranchReg_D, Branch_D, BranchEQ_D, BranchNE_D, regWrite_D,
                     resultSRC_D, memWrite_D, aluControl_D, aluSrcA_D,
                     aluSrcB_D, regA_D, regB_D, uimm32_D, imm32_D, pcPlus4_D,
                     rd_D, rn_D, rm_D}),
                .q({BranchReg_E, Branch_E, BranchEQ_E, BranchNE_E, regWrite_E,
                    resultSRC_E, memWrite_E, aluControl_E, aluSrcA_E, aluSrcB_E,
                    regA_E, regB_E, uimm32_E, imm32_E, pcPlus4_E, rd_E, rn_E, rm_E})); 
  
  
  assign PCsrc[0] = (Branch_E || (BranchEQ_E && flags[2]) ||
                     (BranchNE_E && ~(flags[2])));
  assign PCsrc[1] =  BranchReg_E;
  
  
   mux4 fwdA(.d0(regA_E), .d1(muxResult), .d2(aluResult_M), .d3(),
              .s(forwardA), .y(regA_fwd));
  
   mux4 fwdB(.d0(regB_E), .d1(muxResult), .d2(aluResult_M), .d3(),
             .s(forwardB), .y(regB_fwd));
  
  

  mux2 mux2One(.d0(regA_fwd), .d1(0), .s(aluSrcA_E), .y(srcA));
  
  mux2 mux2Two(.d0(regB_fwd), .d1(uimm32_E), .s(aluSrcB_E), .y(srcB));
 
  alu alu_inst(.a(srcA),.b(srcB),.f(aluControl_E),.result(aluResult_E),.flags(flags));
  
  assign pcBranch = pcPlus4_E + {imm32_E[29:0], 2'b00};
  
  
  //EXMEM register
  register_r#(104) EXMEM(.clk(clk), .reset(reset),
                         .d({regWrite_E, rd_E, memWrite_E, resultSRC_E, aluResult_E,
                    regB_fwd, pcPlus4_E}),
                         .q({regWrite_M, rd_M, memWrite_M, resultSRC_M, aluResult_M,
                    regB_M, pcPlus4_M}));
  
  
  dmem dm(.clk(clk), .we(memWrite_M), .addr(aluResult_M), .wd(regB_M),
          .rd(memResult_M));
 
  
  //MEMWB register 
  
  register_r #(103) MEMWB(.clk(clk), .reset(reset),
                          .d({regWrite_M, rd_M, resultSRC_M, aluResult_M, memResult_M,
                    pcPlus4_M}),
                          .q({regWrite_W, rd_W, resultSRC_W, aluResult_W, memResult_W,
                    pcPlus4_W}));
  
  mux4 mux4mem(.d0(aluResult_W), .d1(memResult_W), .d2(pcPlus4_W),
               .d3(), .s(resultSRC_W), .y(muxResult));
  
  
  
  //hazard 
   hazardUnit hu(.rn_D(rn_D), .rm_D(rm_D), .rn_E(rn_E), .rm_E(rm_E), .rd_E(rd_E),
                .rd_M(rd_M), .rd_W(rd_W), .pcSrc(PCsrc), .resultSrc_E(resultSRC_E),
                .regWrite_M(regWrite_M), .regWrite_W(regWrite_W), .forwardA(forwardA),
                .forwardB(forwardB), .stall(stall), .branchTaken(branchTaken));
  
  
  
endmodule 
  
  
  
  
  
  

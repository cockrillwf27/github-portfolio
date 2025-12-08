// Extend unit -- zero-extends 12-bit value to 32 bits
module extendUnit(input  logic [11:0] imm12,
                  output logic [31:0] uimm32,
                  output logic [31:0] imm32);

  assign uimm32[31:12] = 20'b00000000000000000000;
  assign uimm32[11:0] = imm12[11:0];
  

   always_comb
        begin
          if(imm12[11]==1'b0)
               imm32[31:12] = 20'b00000000000000000000;
            else
               imm32[31:12] = 20'b11111111111111111111;
        end

  assign imm32[11:0] = imm12[11:0];

  
endmodule

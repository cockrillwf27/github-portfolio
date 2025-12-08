module alu(input logic [31:0] a,
           input logic [31:0] b,
           input logic [1:0] f,
           output logic [31:0] result,
           output logic [3:0] flags);
  
  
  
  
   always_comb begin
     case (f)
      
      2'b00:{flags[1], result} = a + b; 
      2'b01:{flags[1], result} = a - b;
      2'b10:{flags[1], result} = {1'b0, a & b};
      2'b11:{flags[1], result} = {1'b0, a | b};
    
      default: {flags[1], result} = a + b; 
        
    endcase 
  end
  
  
  
  assign flags[0] = ( ~(f[0] ^ a[31] ^ b[31]) ) & (a[31] ^ result[31]) & (~(f[1]));
  assign flags[2] = (result == 32'b0);
  assign flags[3] = result[31];
  
endmodule 

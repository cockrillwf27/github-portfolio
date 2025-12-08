module hazardUnit(input logic [3:0] rn_D,
                  input logic [3:0] rm_D,
                  input logic [3:0] rn_E,
                  input logic [3:0] rm_E,
                  input logic [3:0] rd_E,
                  input logic [3:0] rd_M,
                  input logic [3:0] rd_W,
                  input logic [1:0] pcSrc,
                  input logic [1:0] resultSrc_E,
                  input logic regWrite_M,
                  input logic regWrite_W,
                  output logic [1:0] forwardA,
                  output logic [1:0] forwardB,
                  output logic stall,
                  output logic branchTaken);




assign stall = ((rn_D == rd_E) | (rm_D == rd_E)) & (resultSrc_E == 2'b01);

assign branchTaken = pcSrc!=2'b00;

  
  
assign forwardA = ((rd_M == rn_E) & regWrite_M) ? 2'b10
 				 : ((rd_W == rn_E) & regWrite_W) ? 2'b01 
				 : 2'b00;  

assign forwardB = ((rd_M == rm_E) & regWrite_M) ? 2'b10
 				 : ((rd_W == rm_E) & regWrite_W) ? 2'b01
				 : 2'b00;


endmodule 

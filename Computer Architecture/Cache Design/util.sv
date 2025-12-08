// Common building blocks

// 4:1 mux
module mux4 #(parameter WIDTH=32)
  (input logic [WIDTH-1:0] d0,
   input logic [WIDTH-1:0] d1,
   input logic [WIDTH-1:0] d2,
   input logic [WIDTH-1:0] d3,
   input logic [1:0] s,
   output logic [WIDTH-1:0] y
  );
  
  always_comb begin
    case (s)
      2'b00:	y = d0;
      2'b01:	y = d1;
      2'b10:	y = d2;
      2'b11:	y = d3;
      default:	y = 'bx;
    endcase
  end
endmodule


// 2:1 mux
module mux2 #(parameter WIDTH=32)
  (input  logic [WIDTH-1:0] d0,
   input  logic [WIDTH-1:0] d1,
   input  logic             s,
   output logic [WIDTH-1:0] y
  );
  
  assign y = s ? d1 : d0;
endmodule


// 2:4 decoder
module decoder2_4 (input  logic [1:0] s,
                   output logic [3:0] y);
  
  always_comb
    case (s)
      2'b00: y = 4'b0001;
      2'b01: y = 4'b0010;
      2'b10: y = 4'b0100;
      2'b11: y = 4'b1000;
      default: y = 4'bx;
    endcase
endmodule

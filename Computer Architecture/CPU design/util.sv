// Multiplexers

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



module mux2 #(parameter WIDTH=32)
  (input  logic [WIDTH-1:0] d0,
   input  logic [WIDTH-1:0] d1,
   input  logic             s,
   output logic [WIDTH-1:0] y
  );
  
  assign y = s ? d1 : d0;
endmodule


// Register with reset 
module register_r #(parameter WIDTH=32)
  (input  logic             clk,
   input  logic             reset,
   input  logic [WIDTH-1:0] d,
   output logic [WIDTH-1:0] q
  );
  
  always_ff @(posedge clk) begin
    if (reset) q <= 0;
    else q <= d;
  end
endmodule


// Register with reset and enable
module register_ren #(parameter WIDTH=32)
  (input  logic             clk,
   input  logic             reset,
   input  logic             en,
   input  logic [WIDTH-1:0] d,
   output logic [WIDTH-1:0] q
  );
  
  always_ff @(posedge clk) begin
    if (reset) q <= 0;
    else if (en) q <= d;
  end
endmodule

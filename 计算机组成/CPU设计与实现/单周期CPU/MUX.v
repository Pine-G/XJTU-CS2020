module MUX
    #(parameter WIDTH = 5)
    (input wire Sel,
    input wire [WIDTH-1:0] in1, in2,
    output wire [WIDTH-1:0] out);

    assign out = (Sel == 0)? in1: in2;

endmodule

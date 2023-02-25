module MUX41
    #(parameter WIDTH = 32)
    (input wire [1:0] Sel,
    input wire [WIDTH-1:0] in1, in2, in3, in4,
    output wire [WIDTH-1:0] out);

    assign out = (Sel == 2'b00)? in1:
                 (Sel == 2'b01)? in2:
                 (Sel == 2'b10)? in3: in4;

endmodule

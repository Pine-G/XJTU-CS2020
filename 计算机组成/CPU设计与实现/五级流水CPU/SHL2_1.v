module SHL2_1(
    input wire [25:0] in,
    output wire [27:0] out
);
    
    assign out = {in, 2'b00};

endmodule

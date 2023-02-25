module ZeroExt
    #(parameter INPUT_WIDTH = 16,
      parameter OUTPUT_WIDTH = 32)
    (input wire signed [INPUT_WIDTH-1:0] in,
     output wire signed [OUTPUT_WIDTH-1:0] out);

    assign out = {{(OUTPUT_WIDTH - INPUT_WIDTH){1'b0}}, in};

endmodule

module PC(
    input wire RST, CLK,
    input wire [31:0] D,
    output reg [31:0] Q
);

    always @(posedge CLK) begin
        Q <= (RST == 1)? 0: D;
    end

endmodule

module PC(
    input wire RST, CLK, PCWr,
    input wire [31:0] D,
    output reg [31:0] Q
);

    always @(posedge CLK) begin
        if(RST) Q <= 0;
        else if(PCWr) Q <= D;
    end

endmodule

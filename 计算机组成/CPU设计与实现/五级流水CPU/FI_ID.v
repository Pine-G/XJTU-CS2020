module FI_ID(
    input wire CLK, FI_ID_RegWr,
    input wire [31:0] FI_NPC1, FI_IR,
    output reg [31:0] NPC1, IR
);

    always @(posedge CLK) begin
        if(FI_ID_RegWr) begin
            NPC1 <= FI_NPC1;
            IR <= FI_IR;
        end
    end

endmodule

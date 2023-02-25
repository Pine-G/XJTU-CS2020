module MA_WB(
    input wire CLK,
    input wire MA_MemToReg, MA_JAL, MA_RegW, MA_RegDst, MA_OF,
    input wire [31:0] MA_NPC1, MA_ALUOut, MA_MemOut, MA_IR,
    output reg MemToReg, JAL, RegW, RegDst, OF,
    output reg [31:0] NPC1, ALUOut, MemOut, IR
);

    always @(posedge CLK) begin
        MemToReg <= MA_MemToReg;
        JAL <= MA_JAL;
        RegW <= MA_RegW;
        RegDst <= MA_RegDst;
        OF <= MA_OF;
        NPC1 <= MA_NPC1;
        ALUOut <= MA_ALUOut;
        MemOut <= MA_MemOut;
        IR <= MA_IR;
    end

endmodule

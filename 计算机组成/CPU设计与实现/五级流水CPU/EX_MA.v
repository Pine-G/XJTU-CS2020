module EX_MA(
    input wire CLK, Clear,
    input wire EX_BEQ, EX_BNE, EX_J, EX_MemToReg, EX_JAL, EX_RegW, EX_RegDst, EX_JR, EX_ZF, EX_OF,
    input wire [3:0] EX_MemRW,
    input wire [31:0] EX_NPC1, EX_NPC2, EX_NPC3, EX_NPC4, EX_ALUOut, EX_Rt, EX_IR,
    output reg BEQ, BNE, J, MemToReg, JAL, RegW, RegDst, JR, ZF, OF,
    output reg [3:0] MemRW,
    output reg [31:0] NPC1, NPC2, NPC3, NPC4, ALUOut, Rt, IR
);

    always @(posedge CLK) begin
        BEQ <= Clear? 1'b0: EX_BEQ;
        BNE <= Clear? 1'b0: EX_BNE;
        J <= Clear? 1'b0: EX_J;
        MemToReg <= Clear? 1'bx: EX_MemToReg;
        JAL <= Clear? 1'b0: EX_JAL;
        RegW <= Clear? 1'b0: EX_RegW;
        RegDst <= Clear? 1'bx: EX_RegDst;
        JR <= Clear? 1'b0: EX_JR;
        MemRW <= Clear? 4'b0xxx: EX_MemRW;
        
        ZF <= EX_ZF;
        OF <= EX_OF;
        NPC1 <= EX_NPC1;
        NPC2 <= EX_NPC2;
        NPC3 <= EX_NPC3;
        NPC4 <= EX_NPC4;
        ALUOut <= EX_ALUOut;
        Rt <= EX_Rt;
        IR <= EX_IR;
    end

endmodule

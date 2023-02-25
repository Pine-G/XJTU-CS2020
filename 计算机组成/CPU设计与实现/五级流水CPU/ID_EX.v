module ID_EX(
    input wire CLK, Clear,
    input wire ID_ALUSrc, ID_BEQ, ID_BNE, ID_J, ID_MemToReg, ID_JAL, ID_RegW, ID_RegDst,
    input wire [1:0] ID_ALUOp,
    input wire [3:0] ID_MemRW,
    input wire [31:0] ID_NPC1, ID_Rs, ID_Rt, ID_S_Imm32, ID_Z_Imm32, ID_IR,
    output reg ALUSrc, BEQ, BNE, J, MemToReg, JAL, RegW, RegDst,
    output reg [1:0] ALUOp,
    output reg [3:0] MemRW,
    output reg [31:0] NPC1, Rs, Rt, S_Imm32, Z_Imm32, IR
);

    always @(posedge CLK) begin
        ALUSrc <= Clear? 1'bx: ID_ALUSrc;
        BEQ <= Clear? 1'b0: ID_BEQ;
        BNE <= Clear? 1'b0: ID_BNE;
        J <= Clear? 1'b0: ID_J;
        MemToReg <= Clear? 1'bx: ID_MemToReg;
        JAL <= Clear? 1'b0: ID_JAL;
        RegW <= Clear? 1'b0: ID_RegW;
        RegDst <= Clear? 1'bx: ID_RegDst;
        ALUOp <= Clear? 2'bxx: ID_ALUOp;
        MemRW <= Clear? 4'b0xxx: ID_MemRW;
        
        NPC1 <= ID_NPC1;
        Rs <= ID_Rs;
        Rt <= ID_Rt;
        S_Imm32 <= ID_S_Imm32;
        Z_Imm32 <= ID_Z_Imm32;
        IR <= ID_IR;
    end

endmodule

module MCU(
    input wire [5:0] Op,
    output wire RegDst, RegW, ALUSrc, MemToReg, BEQ, BNE, J, JAL,
    output wire [3:0] MemRW,
    output wire [1:0] ALUOp
);

    reg [13:0] Controls;
    assign {RegDst, RegW, ALUSrc, MemToReg, MemRW, BEQ, BNE, J, JAL, ALUOp} = Controls;

    always @(*) begin
        casex(Op)
            6'b000000: Controls <= 14'b1100_0xxx_0000_10;       //R-R
            6'b001xxx: Controls <= 14'b0110_0xxx_0000_11;       //R-I
            
            6'b100000: Controls <= 14'b0111_1011_0000_00;       //LB
            6'b100001: Controls <= 14'b0111_1101_0000_00;       //LH
            6'b100011: Controls <= 14'b0111_1111_0000_00;       //LW
            6'b100100: Controls <= 14'b0111_1100_0000_00;       //LBU
            6'b100101: Controls <= 14'b0111_1110_0000_00;       //LHU
            
            6'b101000: Controls <= 14'bx01x_1000_0000_00;       //SB
            6'b101001: Controls <= 14'bx01x_1001_0000_00;       //SH
            6'b101011: Controls <= 14'bx01x_1010_0000_00;       //SW
            
            6'b000100: Controls <= 14'bx00x_0xxx_1000_01;       //BEQ
            6'b000101: Controls <= 14'bx00x_0xxx_0100_01;       //BNE
            
            6'b000010: Controls <= 14'bx0xx_0xxx_0010_xx;       //J
            6'b000011: Controls <= 14'bx0xx_0xxx_0011_xx;       //JAL
            
            default:   Controls <= 14'bxxxx_xxxx_xxxx_xx;
        endcase
    end

endmodule

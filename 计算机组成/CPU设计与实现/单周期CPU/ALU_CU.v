module ALU_CU(
    input wire [1:0] ALUOp,
    input wire [5:0] Funct,
    input wire [2:0] ImmOp,
    output reg shift, ZeroExt, JR,
    output reg [4:0] ALUControl
);

    always @(*) begin
        shift <= (ALUOp == 2'b10 && Funct[5:2] == 4'b0000)? 1: 0;
        ZeroExt <= (ALUOp == 2'b11 && (ImmOp == 3'b100 || ImmOp == 3'b101 || ImmOp == 3'b110))? 1: 0;
        JR <= (ALUOp == 2'b10 && Funct == 6'b001000)? 1: 0;
        case(ALUOp)
            2'b00: ALUControl <= 5'b00000;  //ADD
            2'b01: ALUControl <= 5'b00010;  //SUB
            2'b10:  case(Funct)
                        6'b100000: ALUControl <= 5'b00000;  //ADD
                        6'b100001: ALUControl <= 5'b00001;  //ADDU
                        6'b100010: ALUControl <= 5'b00010;  //SUB
                        6'b100011: ALUControl <= 5'b00011;  //SUBU
                        6'b011000: ALUControl <= 5'b00100;  //MULT
                        6'b011001: ALUControl <= 5'b00101;  //MULTU
                        6'b011010: ALUControl <= 5'b00110;  //DIV
                        6'b011011: ALUControl <= 5'b00111;  //DIVU
                        6'b101010: ALUControl <= 5'b01000;  //SLT
                        6'b101011: ALUControl <= 5'b01001;  //SLTU
                        6'b000000: ALUControl <= 5'b01010;  //SLL
                        6'b000010: ALUControl <= 5'b01011;  //SRL
                        6'b000011: ALUControl <= 5'b01100;  //SRA
                        6'b000100: ALUControl <= 5'b01010;  //SLLV
                        6'b000110: ALUControl <= 5'b01011;  //SRLV
                        6'b000111: ALUControl <= 5'b01100;  //SRAV
                        6'b100100: ALUControl <= 5'b01101;  //AND
                        6'b100101: ALUControl <= 5'b01110;  //OR
                        6'b100110: ALUControl <= 5'b01111;  //XOR
                        6'b100111: ALUControl <= 5'b10000;  //NOR
                        6'b010000: ALUControl <= 5'b10010;  //MFHI
                        6'b010010: ALUControl <= 5'b10011;  //MFLO
                        default:   ALUControl <= 5'bxxxxx;
                    endcase
            2'b11:  case(ImmOp)
                        3'b000:  ALUControl <= 5'b00000;    //ADDI
                        3'b001:  ALUControl <= 5'b00001;    //ADDIU
                        3'b100:  ALUControl <= 5'b01101;    //ANDI
                        3'b101:  ALUControl <= 5'b01110;    //ORI
                        3'b110:  ALUControl <= 5'b01111;    //XORI
                        3'b111:  ALUControl <= 5'b10001;    //LUI
                        3'b010:  ALUControl <= 5'b01000;    //SLTI
                        3'b011:  ALUControl <= 5'b01001;    //SLTIU
                        default: ALUControl <= 5'bxxxxx;
                    endcase
        endcase
    end

endmodule

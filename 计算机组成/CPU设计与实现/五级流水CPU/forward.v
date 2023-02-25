module forward(
    input wire [31:0] ID_EX_IR, EX_MA_IR, MA_WB_IR,
    input wire ID_EX_MemToReg, EX_MA_MemToReg, ID_EX_RegDst, EX_MA_RegDst, MA_WB_RegW, MA_WB_MemToReg,
    input wire [3:0] EX_MA_MemRW, ID_EX_MemRW,
    output reg [1:0] ALUSrc_A, ALUSrc_B,
    output reg MemSrc
);

    initial begin
        ALUSrc_A <= 2'b00;
        ALUSrc_B <= 2'b00;
        MemSrc <= 1'b0;
    end

    wire EX_MA_MemWr;
    assign EX_MA_MemWr = (EX_MA_MemRW == 4'b1000) | (EX_MA_MemRW == 4'b1001) | (EX_MA_MemRW == 4'b1010);
    
    wire ID_EX_MemWr;
    assign ID_EX_MemWr = (ID_EX_MemRW == 4'b1000) | (ID_EX_MemRW == 4'b1001) | (ID_EX_MemRW == 4'b1010);

    always @(*) begin
       //转发ALU输出至ALU输入
       if((ID_EX_RegDst & EX_MA_RegDst & (ID_EX_IR[25:21] == EX_MA_IR[15:11]) & (ID_EX_IR[25:21] != 5'b00000)) == 1'b1)
           ALUSrc_A[0] = 1'b1;
       else if((ID_EX_RegDst & (~EX_MA_RegDst) & (~EX_MA_MemToReg) & (ID_EX_IR[25:21] == EX_MA_IR[15:11]) & (ID_EX_IR[25:21] != 5'b00000)) == 1'b1)
           ALUSrc_A[0] = 1'b1;
       else if(((~ID_EX_RegDst) & (~ID_EX_MemToReg) & EX_MA_RegDst & (ID_EX_IR[25:21] == EX_MA_IR[20:16]) & (ID_EX_IR[25:21] != 5'b00000)) == 1'b1)
           ALUSrc_A[0] = 1'b1;
       else if(((~ID_EX_RegDst) & (~ID_EX_MemToReg) & (~EX_MA_RegDst) & (~EX_MA_MemToReg) & (ID_EX_IR[25:21] == EX_MA_IR[20:16]) & (ID_EX_IR[25:21] != 5'b00000)) == 1'b1)
           ALUSrc_A[0] = 1'b1;
       else ALUSrc_A[0] = 1'b0;
       if((ID_EX_RegDst & EX_MA_RegDst & (ID_EX_IR[20:16] == EX_MA_IR[15:11]) & (ID_EX_IR[20:16] != 5'b00000)) == 1'b1)
           ALUSrc_B[0] = 1'b1;
       else if(((~ID_EX_RegDst) & (~ID_EX_MemToReg) & EX_MA_RegDst & (ID_EX_IR[20:16] == EX_MA_IR[20:16]) & (ID_EX_IR[20:16] != 5'b00000)) == 1'b1)
           ALUSrc_B[0] = 1'b1;
       else ALUSrc_B[0] = 1'b0;

       //转发数据存储器输出至数据存储器输入
       if((MA_WB_RegW & EX_MA_MemWr & (MA_WB_IR[20:16] == EX_MA_IR[20:16])) == 1'b1)
           MemSrc = 1'b1;
       else MemSrc = 1'b0;
       
       //转发数据存储器输出至ALU输入
       if((MA_WB_MemToReg & (~ID_EX_MemWr) & (MA_WB_IR[20:16] == ID_EX_IR[25:21])) == 1'b1)
           ALUSrc_A[1] = 1'b1;
       else ALUSrc_A[1] = 1'b0;
       if((MA_WB_MemToReg & (~ID_EX_MemWr) & (MA_WB_IR[20:16] == ID_EX_IR[20:16])) == 1'b1)
           ALUSrc_B[1] = 1'b1;
       else ALUSrc_B[1] = 1'b0;
    end

endmodule

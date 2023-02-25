module hazard(
    input wire CLK,
    input wire [4:0] ID_EX_Rt, FI_ID_Rs, FI_ID_Rt,
    input wire [3:0] ID_EX_MemRW,
    input wire EX_MA_ZF, EX_MA_BEQ, EX_MA_BNE, ID_EX_J,
    output reg PCWr, FI_ID_RegWr, Clear0, Clear1
);

    initial begin
        PCWr <= 1;
        FI_ID_RegWr <= 1;
        Clear0 <= 0;
        Clear1 <= 0;
    end

    wire ID_EX_MemRd;
    assign ID_EX_MemRd = (ID_EX_MemRW == 4'b1011) | (ID_EX_MemRW == 4'b1100) | (ID_EX_MemRW == 4'b1101) | (ID_EX_MemRW == 4'b1110) | (ID_EX_MemRW == 4'b1111);

    integer i, k;
    always @(*) begin
        PCWr = 1;
        FI_ID_RegWr = 1;
        Clear0 = 0;
        Clear1 = 0;
        
        //EX阶段检测load-use,流水线停顿1个时钟周期
        if(ID_EX_MemRd & ((ID_EX_Rt == FI_ID_Rs) | (ID_EX_Rt == FI_ID_Rt))) begin
            Clear0 <= 1;
            PCWr <= 0;
            FI_ID_RegWr <= 0;
        end

        //EX阶段检测无条件转移指令,清除后面的无效指令
        if(ID_EX_J) begin
            k = 1;
            i = 5;
        end
        if(k == 1) begin
            if(i == 0) k = 0;
            else begin
                //置Clear0为“1”连续3个时钟周期
                Clear0 = 1;
                if(CLK) i = i - 1;
            end
        end

        //MA阶段检测分支指令,采用静态预测方式,预测失败则清除后面3条指令
        if((EX_MA_BEQ & EX_MA_ZF) | (EX_MA_BNE & (~EX_MA_ZF))) begin
            Clear0 <= 1;
            Clear1 <= 1;
            FI_ID_RegWr <= 0;
        end
    end

endmodule

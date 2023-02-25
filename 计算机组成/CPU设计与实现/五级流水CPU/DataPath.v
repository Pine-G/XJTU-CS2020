module DataPath(
    input wire CLK, RST
);

    wire [31:0] InstAddr, Inst;
    wire [31:0] add_sum1, add_sum2;
    wire [31:0] mux_2, mux_3, mux_4, mux_5, mux_6, mux_7, mux_8, mux_10, mux_11, mux_12, mux_13;
    wire [4:0] mux_1, mux_9;
    wire [31:0] ID_NPC1, ID_IR;
    wire [31:0] SigExt1, SigExt2, ZeroExt1;
    wire [31:0] R_data1, R_data2;
    wire RegDst, J, JAL, RegW, BEQ, BNE, MemToReg, ALUSrc;
    wire [3:0] MemRW;
    wire [1:0] ALUOp;
    wire EX_RegDst, EX_J, EX_JAL, EX_RegW, EX_BEQ, EX_BNE, EX_MemToReg, EX_ALUSrc;
    wire [3:0] EX_MemRW;
    wire [1:0] EX_ALUOp;
    wire [31:0] EX_NPC1, EX_Rs, EX_Rt, EX_S_Imm32, EX_Z_Imm32, EX_IR;
    wire shift, ZSExt, JR, ZF, OF, Error_DIV0;
    wire [4:0] ALUCtrl;
    wire [31:0] ALUResult;
    wire [27:0] shl1;
    wire [31:0] shl2;
    wire [31:0] JumpAddr;
    wire MA_BEQ, MA_BNE, MA_J, MA_MemToReg, MA_JAL, MA_RegW, MA_RegDst, MA_JR, MA_ZF, MA_OF;
    wire [3:0] MA_MemRW;
    wire [31:0] MA_NPC1, MA_NPC2, MA_NPC3, MA_NPC4, MA_ALUOut, MA_Rt, MA_IR;
    wire [31:0] R_data;
    wire Error_Addr;
    wire WB_MemToReg, WB_JAL, WB_RegW, WB_RegDst, WB_OF;
    wire [31:0] WB_NPC1, WB_ALUOut, WB_MemOut, WB_IR;
    wire dummy1, dummy2;
    wire PCSrc, Branch, WE;
    wire [1:0] ALUSrc_A, ALUSrc_B;
    wire MemSrc, Clear0, Clear1, PCWr, FI_ID_RegWr;
    reg PCSrc1;
    
    initial PCSrc1 = 1'b0;
    
    assign JumpAddr = {EX_NPC1[31:28], shl1};
    assign Branch = (MA_BEQ & MA_ZF) | (MA_BNE & (~MA_ZF));
    assign WE = (WB_RegW & (~WB_OF)) | WB_JAL;
    assign PCSrc = PCSrc1;
    
    integer i;
    always @(*) begin
       if(~RST) begin
           if(i == 0) begin
               //检测分支与转移指令
               if(Branch == 1'b1) PCSrc1 = 1'b1;
               else if(MA_J == 1'b1) PCSrc1 = 1'b1;
               else if(MA_JR == 1'b1) PCSrc1 = 1'b1;
               else PCSrc1 = 1'b0;
           end
           else begin
               //在复位后MA阶段暂无指令时负责从指令存储器中顺序取指
               PCSrc1 = 1'b0;
               if(CLK) i = i - 1;
           end
       end
       else begin
           //复位
           PCSrc1 = 0;
           i = 3;
       end
    end
    
    //取指
    PC pc(
        .PCWr(PCWr),
        .CLK(CLK),
        .RST(RST),
        .D(mux_4),
        .Q(InstAddr)
        );
    IMem imem(
        .Addr(InstAddr),
        .Inst(Inst)
        );
    Add add1(
        .A(4),
        .B(InstAddr),
        .S(add_sum1),
        .C(dummy1)
        );
    FI_ID fi_id(
        .FI_ID_RegWr(FI_ID_RegWr),
        .CLK(CLK),
        .FI_NPC1(add_sum1),
        .FI_IR(Inst),
        .NPC1(ID_NPC1),
        .IR(ID_IR)
        );
    
    //译码
    SigExt #(16,32) SigExt16_32(
        .in(ID_IR[15:0]),
        .out(SigExt1)
        );
    ZeroExt #(16,32) ZeroExt16_32(
        .in(ID_IR[15:0]),
        .out(ZeroExt1)
        );
    RF rf(
        .CLK(CLK),
        .W(WE),
        .W_data(mux_10),
        .W_Reg(mux_9), 
        .R_Reg1(ID_IR[25:21]),
        .R_Reg2(ID_IR[20:16]),
        .R_data1(R_data1),
        .R_data2(R_data2)
        );
    MCU mcu(
        .Op(ID_IR[31:26]),
        .RegDst(RegDst),
        .J(J),
        .JAL(JAL),
        .RegW(RegW),
        .BEQ(BEQ),
        .BNE(BNE),
        .MemToReg(MemToReg),
        .MemRW(MemRW),
        .ALUSrc(ALUSrc),
        .ALUOp(ALUOp)
        );
    ID_EX id_ex(
        .Clear(Clear0),
        .CLK(CLK),
        .ID_ALUSrc(ALUSrc),
        .ID_BEQ(BEQ),
        .ID_BNE(BNE),
        .ID_J(J),
        .ID_MemToReg(MemToReg),
        .ID_JAL(JAL),
        .ID_RegW(RegW),
        .ID_RegDst(RegDst),
        .ID_ALUOp(ALUOp),
        .ID_MemRW(MemRW),
        .ID_NPC1(ID_NPC1),
        .ID_Rs(R_data1),
        .ID_Rt(R_data2),
        .ID_S_Imm32(SigExt1),
        .ID_Z_Imm32(ZeroExt1),
        .ID_IR(ID_IR),
        .ALUSrc(EX_ALUSrc),
        .BEQ(EX_BEQ),
        .BNE(EX_BNE),
        .J(EX_J),
        .MemToReg(EX_MemToReg),
        .JAL(EX_JAL),
        .RegW(EX_RegW),
        .RegDst(EX_RegDst),
        .ALUOp(EX_ALUOp),
        .MemRW(EX_MemRW),
        .NPC1(EX_NPC1),
        .Rs(EX_Rs),
        .Rt(EX_Rt),
        .S_Imm32(EX_S_Imm32),
        .Z_Imm32(EX_Z_Imm32),
        .IR(EX_IR)
        );

    //执行
    SigExt #(5,32) SigExt5_32(
        .in(EX_IR[10:6]),
        .out(SigExt2)
        );
    MUX21 #(32) mux2(
        .Sel(EX_ALUSrc),
        .in1(EX_Rt),
        .in2(mux_7),
        .out(mux_2)
        );
    MUX21 #(32) mux6(
        .Sel(shift),
        .in1(EX_Rs),
        .in2(SigExt2),
        .out(mux_6)
        );
    MUX21 #(32) mux7(
        .Sel(ZSExt),
        .in1(EX_S_Imm32),
        .in2(EX_Z_Imm32),
        .out(mux_7)
        );
    SHL2_1 shl2_1(
        .in(EX_IR[25:0]),
        .out(shl1)
        );
    SHL2_2 shl2_2(
        .in(EX_S_Imm32),
        .out(shl2)
        );
    Add add2(
        .A(EX_NPC1),
        .B(shl2),
        .S(add_sum2),
        .C(dummy2)
        );
    ALU_CU alu_cu(
        .Funct(EX_IR[5:0]),
        .ImmOp(EX_IR[28:26]),
        .ALUOp(EX_ALUOp),
        .ALUControl(ALUCtrl),
        .shift(shift),
        .ZeroExt(ZSExt),
        .JR(JR)
        );
    ALU alu(
        .OP(ALUCtrl),
        .A(mux_11),
        .B(mux_12),
        .F(ALUResult),
        .ZF(ZF),
        .OF(OF),
        .Error_DIV0(Error_DIV0)
        );
    EX_MA ex_ma(
        .Clear(Clear1),
        .CLK(CLK),
        .EX_BEQ(EX_BEQ),
        .EX_BNE(EX_BNE),
        .EX_J(EX_J),
        .EX_MemToReg(EX_MemToReg),
        .EX_JAL(EX_JAL),
        .EX_RegW(EX_RegW),
        .EX_RegDst(EX_RegDst),
        .EX_JR(JR),
        .EX_ZF(ZF),
        .EX_OF(OF),
        .EX_MemRW(EX_MemRW),
        .EX_NPC1(EX_NPC1),
        .EX_NPC2(JumpAddr),
        .EX_NPC3(add_sum2),
        .EX_NPC4(EX_Rs),
        .EX_ALUOut(ALUResult),
        .EX_Rt(EX_Rt),
        .EX_IR(EX_IR),
        .BEQ(MA_BEQ),
        .BNE(MA_BNE),
        .J(MA_J),
        .MemToReg(MA_MemToReg),
        .JAL(MA_JAL),
        .RegW(MA_RegW),
        .RegDst(MA_RegDst),
        .JR(MA_JR),
        .ZF(MA_ZF),
        .OF(MA_OF),
        .MemRW(MA_MemRW),
        .NPC1(MA_NPC1),
        .NPC2(MA_NPC2),
        .NPC3(MA_NPC3),
        .NPC4(MA_NPC4),
        .ALUOut(MA_ALUOut),
        .Rt(MA_Rt),
        .IR(MA_IR)
        );
    
    //访存
    MUX41 #(32) mux3(
        .Sel({Branch, MA_J}),
        .in1(0),
        .in2(MA_NPC2),
        .in3(MA_NPC3),
        .in4(0),
        .out(mux_3)
        );
    MUX21 #(32) mux4(
        .Sel(PCSrc),
        .in1(add_sum1),
        .in2(mux_8),
        .out(mux_4)
        );
    MUX21 #(32) mux8(
        .Sel(MA_JR),
        .in1(mux_3),
        .in2(MA_NPC4),
        .out(mux_8)
        );
    DMem dmem(
        .Addr(MA_ALUOut),
        .R_data(R_data),
        .Mode(MA_MemRW),
        .W_data(mux_13),
        .Error_Addr(Error_Addr)
        );
    MA_WB ma_wb(
        .CLK(CLK),
        .MA_MemToReg(MA_MemToReg),
        .MA_JAL(MA_JAL),
        .MA_RegW(MA_RegW),
        .MA_RegDst(MA_RegDst),
        .MA_OF(MA_OF),
        .MA_NPC1(MA_NPC1),
        .MA_ALUOut(MA_ALUOut),
        .MA_MemOut(R_data),
        .MA_IR(MA_IR),
        .MemToReg(WB_MemToReg),
        .JAL(WB_JAL),
        .RegW(WB_RegW),
        .RegDst(WB_RegDst),
        .OF(WB_OF),
        .NPC1(WB_NPC1),
        .ALUOut(WB_ALUOut),
        .MemOut(WB_MemOut),
        .IR(WB_IR)
        );
    
    //写回
    MUX21 #(5) mux1(
        .Sel(WB_RegDst),
        .in1(WB_IR[20:16]),
        .in2(WB_IR[15:11]),
        .out(mux_1)
        );
    MUX21 #(32) mux5(
        .Sel(WB_MemToReg),
        .in1(WB_ALUOut),
        .in2(WB_MemOut),
        .out(mux_5)
        );
    MUX21 #(5) mux9(
        .Sel(WB_JAL),
        .in1(mux_1),
        .in2(5'b11111),
        .out(mux_9)
        );
    MUX21 #(32) mux10(
        .Sel(WB_JAL),
        .in1(mux_5),
        .in2(WB_NPC1),
        .out(mux_10)
        );
    
    //流水线冒险处理
    forward Forward(
        .ID_EX_IR(EX_IR),
        .EX_MA_IR(MA_IR),
        .MA_WB_IR(WB_IR),
        .ID_EX_MemToReg(EX_MemToReg),
        .EX_MA_MemToReg(MA_MemToReg),
        .MA_WB_MemToReg(WB_MemToReg),
        .ID_EX_RegDst(EX_RegDst),
        .EX_MA_RegDst(MA_RegDst),
        .MA_WB_RegW(WB_RegW),
        .EX_MA_MemRW(MA_MemRW),
        .ID_EX_MemRW(EX_MemRW),
        .MemSrc(MemSrc),
        .ALUSrc_A(ALUSrc_A),
        .ALUSrc_B(ALUSrc_B)
        );
    MUX41 #(32) mux11(
        .Sel(ALUSrc_A),
        .in1(mux_6),
        .in2(MA_ALUOut),
        .in3(WB_MemOut),
        .in4(0),
        .out(mux_11)
        );
    MUX41 #(32) mux12(
        .Sel(ALUSrc_B),
        .in1(mux_2),
        .in2(MA_ALUOut),
        .in3(WB_MemOut),
        .in4(0),
        .out(mux_12)
        );
    MUX21 #(32) mux13(
        .Sel(MemSrc),
        .in1(MA_Rt),
        .in2(WB_MemOut),
        .out(mux_13)
        );
    hazard Hazard(
        .CLK(CLK),
        .ID_EX_Rt(EX_IR[20:16]),
        .FI_ID_Rs(ID_IR[25:21]),
        .FI_ID_Rt(ID_IR[20:16]),
        .ID_EX_MemRW(EX_MemRW),
        .EX_MA_ZF(MA_ZF),
        .EX_MA_BEQ(MA_BEQ),
        .EX_MA_BNE(MA_BNE),
        .ID_EX_J(EX_J),
        .PCWr(PCWr),
        .FI_ID_RegWr(FI_ID_RegWr),
        .Clear0(Clear0),
        .Clear1(Clear1)
        );

endmodule

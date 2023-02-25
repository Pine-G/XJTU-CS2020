module DataPath(
    input wire CLK, RST
);

    wire [31:0] InstAddr, Inst;
    wire [31:0] R_data, R_data1, R_data2;
    wire [27:0] shl1;
    wire [31:0] shl2;
    wire [4:0] mux_1, mux_9;
    wire [31:0] mux_2, mux_3, mux_4, mux_5, mux_6, mux_7, mux_8, mux_10;
    wire [31:0] add_sum1, add_sum2;
    wire RegDst, RegW, ALUSrc, MemToReg, BEQ, BNE, J, JAL;
    wire ZF, OF, shift, ZSExt, JR, Branch, WE, Error_Addr, Error_DIV0;
    wire [3:0] MemRW;
    wire [1:0] ALUOp;
    wire [4:0] ALUCtrl;
    wire [31:0] ALUResult;
    wire [31:0] SigExt1, SigExt2, ZeroExt1;
    wire [31:0] JumpAddr;
    
    assign Branch = (BEQ & ZF) | (BNE & ~(ZF));
    assign JumpAddr = {add_sum1[31:28], shl1};
    assign WE = (RegW & (~OF)) | JAL;
    
    PC pc(.D(mux_8), .CLK(CLK), .RST(RST), .Q(InstAddr));
    
    Add add1(.A(4), .B(InstAddr), .C(add_sum1));
    Add add2(.A(add_sum1), .B(shl2), .C(add_sum2));
    
    SHL2_1 shl2_1(.in(Inst[25:0]), .out(shl1));
    SHL2_2 shl2_2(.in(SigExt1), .out(shl2));
    
    MUX #(5) mux1(.Sel(RegDst), .in1(Inst[20:16]), .in2(Inst[15:11]), .out(mux_1));
    MUX #(32) mux2(.Sel(ALUSrc), .in1(R_data2), .in2(mux_7), .out(mux_2));
    MUX #(32) mux3(.Sel(Branch), .in1(add_sum1), .in2(add_sum2), .out(mux_3));
    MUX #(32) mux4(.Sel(J), .in1(mux_3), .in2(JumpAddr), .out(mux_4));
    MUX #(32) mux5(.Sel(MemToReg), .in1(ALUResult), .in2(R_data), .out(mux_5));
    MUX #(32) mux6(.Sel(shift), .in1(R_data1), .in2(SigExt2), .out(mux_6));
    MUX #(32) mux7(.Sel(ZSExt), .in1(SigExt1), .in2(ZeroExt1), .out(mux_7));
    MUX #(32) mux8(.Sel(JR), .in1(mux_4), .in2(R_data1), .out(mux_8));
    MUX #(5) mux9(.Sel(JAL), .in1(mux_1), .in2(5'b11111), .out(mux_9));
    MUX #(32) mux10(.Sel(JAL), .in1(mux_5), .in2(add_sum1), .out(mux_10));
    
    SigExt #(16,32) SigExt16_32(.in(Inst[15:0]), .out(SigExt1));
    SigExt #(5,32) SigExt5_32(.in(Inst[10:6]), .out(SigExt2));
    ZeroExt #(16,32) ZeroExt16_32(.in(Inst[15:0]), .out(ZeroExt1));
    
    IMem imem(.Addr(InstAddr), .Inst(Inst));
    DMem dmem(.Addr(ALUResult), .R_data(R_data), .Mode(MemRW), .W_data(R_data2), .Error_Addr(Error_Addr));
    
    RF rf(.CLK(CLK), .W(WE), .W_data(mux_10), .R_Reg1(Inst[25:21]), .R_Reg2(Inst[20:16]), .W_Reg(mux_9), .R_data1(R_data1), .R_data2(R_data2));
    
    MCU mcu(.Op(Inst[31:26]), .RegDst(RegDst), .J(J), .JAL(JAL), .RegW(RegW), .BEQ(BEQ), .BNE(BNE), .MemToReg(MemToReg), .MemRW(MemRW), .ALUSrc(ALUSrc), .ALUOp(ALUOp));
    
    ALU_CU alu_cu(.Funct(Inst[5:0]), .ImmOp(Inst[28:26]), .ALUOp(ALUOp), .ALUControl(ALUCtrl), .shift(shift), .ZeroExt(ZSExt), .JR(JR));
    
    ALU alu(.OP(ALUCtrl), .A(mux_6), .B(mux_2), .F(ALUResult), .ZF(ZF), .OF(OF), .Error_DIV0(Error_DIV0));

endmodule

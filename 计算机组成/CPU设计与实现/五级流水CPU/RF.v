module RF(
    input wire CLK, W,
    input wire [31:0] W_data,
    input wire [4:0] R_Reg1, R_Reg2, W_Reg,
    output wire [31:0] R_data1, R_data2
);
    
    reg [31:0] rf[31:0];
    
    integer i;
    initial begin
        for(i = 0; i < 32; i = i + 1) rf[i] = 0;
    end
    
    always @(posedge CLK) begin
        if(W && (W_Reg != 0)) rf[W_Reg] <= W_data;
    end
    
    assign R_data1 = (R_Reg1 == 0)? 0:
                     (R_Reg1 === W_Reg)? W_data: rf[R_Reg1];
    assign R_data2 = (R_Reg2 == 0)? 0:
                     (R_Reg2 === W_Reg)? W_data: rf[R_Reg2];

endmodule

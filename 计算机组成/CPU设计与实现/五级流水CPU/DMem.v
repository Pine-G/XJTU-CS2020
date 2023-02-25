module DMem(
    input wire [3:0]Mode,
    input wire [31:0] Addr, W_data,
    output reg [31:0] R_data,
    output reg Error_Addr
);

    parameter DMEM_SIZE = 256;
    reg [7:0] RAM[DMEM_SIZE-1:0];
    
    initial $readmemb("D:/Desktop/data.txt", RAM);

    always @(*) begin
        Error_Addr = 0;
        case(Mode)
            4'b1000: RAM[Addr+0] <= W_data[7:0];        //SB
            4'b1001: if(Addr[0] == 1'b0) begin          //SH
                        RAM[Addr+0] <= W_data[7:0];
                        RAM[Addr+1] <= W_data[15:8];
                     end
                     else Error_Addr = 1;
            4'b1010: if(Addr[1:0] == 2'b00) begin       //SW
                        RAM[Addr+0] <= W_data[7:0];
                        RAM[Addr+1] <= W_data[15:8];
                        RAM[Addr+2] <= W_data[23:16];
                        RAM[Addr+3] <= W_data[31:24];
                     end
                     else Error_Addr = 1;
            4'b1011: R_data = {{(24){RAM[Addr+0][7]}}, RAM[Addr+0]};    //LB
            4'b1100: R_data = {24'h000000, RAM[Addr+0]};                //LBU
            4'b1101: if(Addr[0] == 1'b0)                //LH
                        R_data = {{(16){RAM[Addr+1][7]}}, RAM[Addr+1], RAM[Addr+0]};
                     else Error_Addr = 1;
            4'b1110: if(Addr[0] == 1'b0)                //LHU
                        R_data = {16'h0000, RAM[Addr+1], RAM[Addr+0]};
                     else Error_Addr = 1;
            4'b1111: if(Addr[1:0] == 2'b00)             //LW
                        R_data = {RAM[Addr+3], RAM[Addr+2], RAM[Addr+1], RAM[Addr+0]};
                     else Error_Addr = 1;
            default: R_data = 32'hzzzz_zzzz;
        endcase
    end

endmodule

module IMem(
    input wire [31:0] Addr,
    output wire [31:0] Inst
);

    parameter IMEM_SIZE = 64;
    reg [31:0] RAM[IMEM_SIZE-1:0];
    
    initial $readmemh("D:/Desktop/instruction.txt", RAM);

    assign Inst = RAM[Addr/4];

endmodule

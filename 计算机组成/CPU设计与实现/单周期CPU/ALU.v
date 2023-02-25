module ALU(
    input wire [4:0] OP,
    input wire [31:0] A, B,
    output reg [31:0] F,
    output reg ZF, OF, Error_DIV0
);

    reg [31:0] HI, LO;
    reg [32:0] temp;
    
    initial {HI, LO} = 64'h0000_0000_0000_0000;

    always @(*) begin
        OF = 0;
        Error_DIV0 = 0;
        #1
        case(OP)
            5'b00000: begin
                        temp = {A[31], A} + {B[31], B};
                        {F, OF} = (temp[32] ^ temp[31])? {32'hzzzz_zzzz, 1'b1}: {temp[31:0], 1'b0};
                      end
            5'b00001: F = A + B;
            5'b00010: begin
                        temp = {A[31], A} - {B[31], B};
                        {F, OF} = (temp[32] ^ temp[31])? {32'hzzzz_zzzz, 1'b1}: {temp[31:0], 1'b0};
                      end
            5'b00011: F = A - B;
            5'b00100: {HI, LO} = ($signed(A)) * ($signed(B));
            5'b00101: {HI, LO} = A * B;
            5'b00110: begin
                        if(B == 0) Error_DIV0 = 1;
                        else begin
                            HI = ($signed(A)) % ($signed(B));
                            LO = ($signed(A)) / ($signed(B));
                        end
                      end
            5'b00111: begin
                        if(B == 0) Error_DIV0 = 1;
                        else begin
                            HI = A % B;
                            LO = A / B;
                        end
                      end
            5'b01000: F = (($signed(A)) < ($signed(B)))? 1: 0;
            5'b01001: F = (A < B)? 1: 0;
            5'b01010: F = B << A[4:0];
            5'b01011: F = B >> A[4:0];
            5'b01100: F = ($signed(B)) >>> A[4:0];
            5'b01101: F = A & B;
            5'b01110: F = A | B;
            5'b01111: F = A ^ B;
            5'b10000: F = ~(A | B);
            5'b10001: F = {B[15:0], 16'h0000};
            5'b10010: F = HI;
            5'b10011: F = LO;
            default: F = 32'hzzzz_zzzz;
        endcase
        ZF = F == 0;
    end

endmodule

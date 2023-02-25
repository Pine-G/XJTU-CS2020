module addr_4bit(S, C3, A, B, Ci);
    input wire [3:0] A, B;
    input wire Ci;
    output wire [3:0] S;
    output wire C3;
    wire C0, C1, C2;
    
    fulladder FA0(S[0], C0, A[0], B[0], Ci),
              FA1(S[1], C1, A[1], B[1], C0),
              FA2(S[2], C2, A[2], B[2], C1),
              FA3(S[3], C3, A[3], B[3], C2);
endmodule

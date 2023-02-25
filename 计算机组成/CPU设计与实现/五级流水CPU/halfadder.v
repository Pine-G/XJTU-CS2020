module halfadder(S, C, A, B);
    input wire A, B;
    output wire S, C;
    xor (S, A, B);
    and (C, A, B);
endmodule

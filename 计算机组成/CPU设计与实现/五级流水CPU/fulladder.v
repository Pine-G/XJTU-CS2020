module fulladder(S, Co, A, B, Ci);
    input wire A, B, Ci;
    output wire S, Co;
    wire S1, D1, D2;
    halfadder HA1(S1, D1, A, B);
    halfadder HA2(S, D2, S1, Ci);
    or (Co, D2, D1);
endmodule

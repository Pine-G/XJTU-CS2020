module sim_cpu();
    reg CLK, RST;
    
    initial begin
        CLK = 0;
        RST = 1;
        fork
            forever #1 CLK = ~CLK;
            #2 RST = 0;
        join
    end
    
    DataPath DP(CLK, RST);
    
endmodule

module sim_cpu();
    reg CLK, RST;
    
    initial begin
        CLK = 0;
        RST = 1;
        fork
            forever #10 CLK = ~CLK;
            #20 RST = 0;
        join
    end
    
    DataPath DP(CLK, RST);
    
endmodule

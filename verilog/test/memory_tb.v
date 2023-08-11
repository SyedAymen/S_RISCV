module memory_tb;
    wire out_wire;
    reg in_reg;
    memory m(
        .clk(in_reg),
        .slow_clk(out_wire)
    );
    initial begin
    $dumpfile("build/memory_tb.vcd");
    $dumpvars(0,memory_tb);
       
       in_reg = 0;
       #5;
       in_reg = 1;
       #5;
       in_reg = 0;
       #5;
       in_reg = 1;
        $finish; 
    end
    
endmodule
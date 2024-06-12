`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 01/05/2023 07:16:14 PM
// Design Name: 
// Module Name: tb_RDataFeeder
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module tb_RDataFeeder;

    reg clk;
    reg rst;

    reg en;
    reg [32-1 : 0] addr;
    reg wr;
    reg [32-1 : 0] data_i;
    wire [32-1: 0] data_o;
    wire ready;

    RDataFeeder uut(
        .clk(clk),
        .rst(rst),

        .EN(en),
        .ADDR(addr[17:0]),
        .WR(wr),
        .DATAI(data_i),
        .DATAO(data_o),
        .READY(ready)
    );

    initial clk = 0;
    always #10 clk = ~clk;

    initial begin
        rst = 0;
        en = 0;
        addr = 0;
        wr = 0;
        data_i = 0;

        #20 rst = 1;
        @(posedge clk)
        @(posedge clk)
        @(posedge clk)
        #5
        rst = 0;

        //wirte nums
        @(posedge clk)
        en = 1;
        addr = 32'hC202_0000;
        wr = 1;
        data_i = 32'h0000_0004;
        @(posedge clk)
        #1
        en = 0;
        addr = 0;
        wr = 0;
        data_i = 0;

        @(posedge clk)
        #150
        @(posedge clk)

        //arrA[0]
        @(posedge clk)
        en = 1;
        addr = 32'hC200_0000;
        wr = 1;
        data_i = 32'hbe16_012c;
        @(posedge clk)
        #1
        en = 0;
        addr = 0;
        wr = 0;
        data_i = 0;

        @(posedge clk)
        #90
        @(posedge clk)

        @(posedge clk)
        en = 1;
        addr = 32'hC201_0000;
        wr = 1;
        data_i = 32'h41ab_a917;
        @(posedge clk)
        #1
        en = 0;
        addr = 0;
        wr = 0;
        data_i = 0;

        @(posedge clk)
        #200
        @(posedge clk)

        //arrA[0]
        @(posedge clk)
        en = 1;
        addr = 32'hC200_0004;
        wr = 1;
        data_i = 32'hc1af_b05f;
        @(posedge clk)
        #1
        en = 0;
        addr = 0;
        wr = 0;
        data_i = 0;

        @(posedge clk)
        #70
        @(posedge clk)

        @(posedge clk)
        en = 1;
        addr = 32'hC201_0004;
        wr = 1;
        data_i = 32'h428f_e82f;
        @(posedge clk)
        #1
        en = 0;
        addr = 0;
        wr = 0;
        data_i = 0;

        @(posedge clk)
        #240
        @(posedge clk)

        //arrA[0]
        @(posedge clk)
        en = 1;
        addr = 32'hC200_0008;
        wr = 1;
        data_i = 32'hc1e2_e945;
        @(posedge clk)
        #1
        en = 0;
        addr = 0;
        wr = 0;
        data_i = 0;

        @(posedge clk)
        #50
        @(posedge clk)

        @(posedge clk)
        en = 1;
        addr = 32'hC201_0008;
        wr = 1;
        data_i = 32'hc258_1870;
        @(posedge clk)
        #1
        en = 0;
        addr = 0;
        wr = 0;
        data_i = 0;

        @(posedge clk)
        #240
        @(posedge clk)

        //arrA[0]
        @(posedge clk)
        en = 1;
        addr = 32'hC200_000c;
        wr = 1;
        data_i = 32'h41dd_f47b;
        @(posedge clk)
        #1
        en = 0;
        addr = 0;
        wr = 0;
        data_i = 0;

        @(posedge clk)
        #50
        @(posedge clk)

        @(posedge clk)
        en = 1;
        addr = 32'hC201_000c;
        wr = 1;
        data_i = 32'h428f_748f;
        @(posedge clk)
        #1
        en = 0;
        addr = 0;
        wr = 0;
        data_i = 0;

        @(posedge clk)
        #500
        @(posedge clk)

        @(posedge clk)
        en = 1;
        addr = 32'hC203_0000;
        wr = 0;

        @(posedge clk) // read data here
        #1
        en = 0;
        addr = 0;
        wr = 0;
        data_i = 0;

        @(posedge clk)
        # 120
        @(posedge clk)

        //here starts the second round....

        //wirte nums
        @(posedge clk)
        en = 1;
        addr = 32'hC202_0000;
        wr = 1;
        data_i = 32'h0000_0004;
        @(posedge clk)
        #1
        en = 0;
        addr = 0;
        wr = 0;
        data_i = 0;

        @(posedge clk)
        #150
        @(posedge clk)

        //arrA[0]
        @(posedge clk)
        en = 1;
        addr = 32'hC200_0000;
        wr = 1;
        data_i = 32'hbe28_c151;
        @(posedge clk)
        #1
        en = 0;
        addr = 0;
        wr = 0;
        data_i = 0;

        @(posedge clk)
        #90
        @(posedge clk)

        @(posedge clk)
        en = 1;
        addr = 32'hC201_0000;
        wr = 1;
        data_i = 32'hc2a8_d411;
        @(posedge clk)
        #1
        en = 0;
        addr = 0;
        wr = 0;
        data_i = 0;

        @(posedge clk)
        #200
        @(posedge clk)

        //arrA[0]
        @(posedge clk)
        en = 1;
        addr = 32'hC200_0004;
        wr = 1;
        data_i = 32'h42af_222e;
        @(posedge clk)
        #1
        en = 0;
        addr = 0;
        wr = 0;
        data_i = 0;

        @(posedge clk)
        #70
        @(posedge clk)

        @(posedge clk)
        en = 1;
        addr = 32'hC201_0004;
        wr = 1;
        data_i = 32'hc2bfd6df;
        @(posedge clk)
        #1
        en = 0;
        addr = 0;
        wr = 0;
        data_i = 0;

        @(posedge clk)
        #240
        @(posedge clk)

        //arrA[0]
        @(posedge clk)
        en = 1;
        addr = 32'hC200_0008;
        wr = 1;
        data_i = 32'h42159a0b;
        @(posedge clk)
        #1
        en = 0;
        addr = 0;
        wr = 0;
        data_i = 0;

        @(posedge clk)
        #50
        @(posedge clk)

        @(posedge clk)
        en = 1;
        addr = 32'hC201_0008;
        wr = 1;
        data_i = 32'h42920b14;
        @(posedge clk)
        #1
        en = 0;
        addr = 0;
        wr = 0;
        data_i = 0;

        @(posedge clk)
        #240
        @(posedge clk)

        //arrA[0]
        @(posedge clk)
        en = 1;
        addr = 32'hC200_000c;
        wr = 1;
        data_i = 32'h4295_3aba;
        @(posedge clk)
        #1
        en = 0;
        addr = 0;
        wr = 0;
        data_i = 0;

        @(posedge clk)
        #50
        @(posedge clk)

        @(posedge clk)
        en = 1;
        addr = 32'hC201_000c;
        wr = 1;
        data_i = 32'h4257_72ce;
        @(posedge clk)
        #1
        en = 0;
        addr = 0;
        wr = 0;
        data_i = 0;

        @(posedge clk)
        #500
        @(posedge clk)

        @(posedge clk)
        en = 1;
        addr = 32'hC203_0000;
        wr = 0;

        @(posedge clk) // read data here
        #1
        en = 0;
        addr = 0;
        wr = 0;
        data_i = 0;



    end





endmodule

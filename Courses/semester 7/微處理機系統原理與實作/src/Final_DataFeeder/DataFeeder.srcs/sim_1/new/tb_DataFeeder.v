`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 01/02/2023 11:22:26 PM
// Design Name: 
// Module Name: tb_DataFeeder
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


module tb_DataFeeder;

    reg clk;
    reg rst;
    wire [32-1: 0] answer;
    wire done;

    DataFeeder uut (
        .clk(clk),
        .rst(rst),
        .answer_o(answer),
        .done_o(done)
    );

    initial begin
        clk = 0;
    end
    
    always #5 clk = ~clk;

    initial begin
        rst = 0;
        @(posedge clk)
        rst = 1;
        @(posedge clk)
        @(negedge clk)
        rst = 0;

    end


endmodule

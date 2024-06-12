`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 01/02/2023 10:45:07 PM
// Design Name: 
// Module Name: DataFeeder
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


module DataFeeder
    #(
        parameter X_LEN = 8
    )
    (
    input clk,
    input rst,
    output reg [32 - 1: 0] answer_o,
    output done_o
    );
    reg [$clog2(X_LEN) : 0] A_idx; 

    // a*b+c
    reg s_axis_a_tvalid, s_axis_b_tvalid, s_axis_c_tvalid;
    wire s_axis_a_tready, s_axis_b_tready, s_axis_c_tready;
    wire [32 - 1 : 0] s_axis_a_tdata, s_axis_b_tdata, s_axis_c_tdata;

    wire m_axis_result_tvalid;
    wire m_axis_result_tready;
    wire [32-1: 0] m_axis_result_tdata;

    floating_point_0 your_instance_name (
    .aclk(clk),                                  // input wire aclk
    .s_axis_a_tvalid(s_axis_a_tvalid),            // input wire s_axis_a_tvalid
    .s_axis_a_tready(s_axis_a_tready),            // output wire s_axis_a_tready
    .s_axis_a_tdata(s_axis_a_tdata),              // input wire [31 : 0] s_axis_a_tdata
    
    .s_axis_b_tvalid(s_axis_b_tvalid),            // input wire s_axis_b_tvalid
    .s_axis_b_tready(s_axis_b_tready),            // output wire s_axis_b_tready
    .s_axis_b_tdata(s_axis_b_tdata),              // input wire [31 : 0] s_axis_b_tdata
    
    .s_axis_c_tvalid(s_axis_c_tvalid),            // input wire s_axis_c_tvalid
    .s_axis_c_tready(s_axis_c_tready),            // output wire s_axis_c_tready
    .s_axis_c_tdata(s_axis_c_tdata),              // input wire [31 : 0] s_axis_c_tdata
    
    .m_axis_result_tvalid(m_axis_result_tvalid),  // output wire m_axis_result_tvalid
    .m_axis_result_tready(m_axis_result_tready),  // input wire m_axis_result_tready
    .m_axis_result_tdata(m_axis_result_tdata)     // output wire [31 : 0] m_axis_result_tdata
);

    reg [3: 0] state, next_state;
    localparam S_INIT = 0;
    localparam S_FILL = 1;
    localparam S_COMP = 2;
    localparam S_COMP_WAIT = 3;
    localparam S_COMP_PREP = 4;
    localparam S_DONE = 5;

    always @(posedge clk ) begin
        if(rst) state <= S_INIT;
        else state <= next_state;
    end

    always @(*)begin
        case (state)
            S_INIT: next_state = S_FILL;
            S_FILL: next_state = S_COMP;
            S_COMP: next_state = (A_idx >= X_LEN)? S_DONE : S_COMP_WAIT;
            S_COMP_WAIT: next_state = (m_axis_result_tvalid)? S_COMP_PREP: S_COMP_WAIT;
            S_COMP_PREP: next_state = S_COMP;
            S_DONE: next_state = S_DONE;
            default: next_state = S_INIT;
        endcase
    end

    integer idx;

    reg [32-1 : 0] buffer1 [X_LEN - 1: 0];
    reg [32-1 : 0] buffer2 [X_LEN - 1: 0];
    
    
    always @(posedge clk ) begin
        if(rst)begin
            for (idx = 0; idx < X_LEN; idx = idx + 1)begin
                buffer1[idx] <= 0;
                buffer2[idx] <= 0;
            end
        end
        else if(state == S_FILL)begin
            for (idx = 0; idx < X_LEN; idx = idx + 1)begin
                buffer1[idx] = $shortrealtobits($itor((idx+1)*(idx+1)));
                buffer2[idx] = $shortrealtobits(2.5-idx);
            end
        end
            
    end

    always @(posedge clk ) begin
        if(rst)begin
            A_idx <= 0;
        end
        else if(state == S_COMP_PREP)begin
            A_idx <= A_idx + 1;
        end
    end

    assign s_axis_a_tdata = buffer1[A_idx];
    assign s_axis_b_tdata = buffer2[A_idx];
    assign s_axis_c_tdata = answer_o;

    always @(posedge clk ) begin
        if(rst)begin
            s_axis_a_tvalid <= 0;
            s_axis_b_tvalid <= 0;
            s_axis_c_tvalid <= 0;
        end 
        else if(state == S_COMP)begin
            s_axis_a_tvalid <= 1;
            s_axis_b_tvalid <= 1;
            s_axis_c_tvalid <= 1;
        end else begin
            s_axis_a_tvalid <= 0;
            s_axis_b_tvalid <= 0;
            s_axis_c_tvalid <= 0;
        end
    end

    assign m_axis_result_tready = (state == S_COMP_PREP);

    always @(posedge clk ) begin
        if(rst) answer_o <= 0;
        else if(state == S_FILL) answer_o <= 0;
        else if(state == S_COMP_PREP) answer_o <= m_axis_result_tdata;
    end

    assign done_o = (state == S_DONE);

endmodule


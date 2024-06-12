`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 01/04/2023 10:56:27 AM
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


//       [1] 0xC200_0000 - 0xC2FF_FFFF : DSA device

// 0xc200_0000 ~ 0xc200_FFFF A array
// 0xc201_0000 ~ 0xc201_FFFF B array
// 0xc202_0000  number count
// 0xc203_0000 Answer

module RDataFeeder
#(
   parameter [15: 0] XLEN = 32,
   parameter [31: 0] BUF_LEN = 8 //buffers both a and b
) (
    input clk,   // clock
    input rst,   // reset

    input EN,
    input [17 : 0] ADDR,
    input WR, // 1: write 0:read
    input [XLEN -1 : 0] DATAI,
    output [XLEN -1 : 0] DATAO,
    output reg READY
    );


    reg [32 - 1: 0] target_num;
    reg [32 - 1: 0] answer_store;


    reg [3: 0] A_state;
    reg [3: 0] A_nextstate;
    localparam A_INIT = 0;
    localparam A_FILL = 1;
    localparam A_LOAD = 2;
    
    reg [3: 0] S_state;
    reg [3: 0] S_next_state;

    localparam S_INIT = 0;
    localparam S_COMP = 1;
    localparam S_COMP_WAIT = 2;
    localparam S_COMP_PREP = 3;
    localparam S_DONE = 4;
    


    always @(posedge clk ) begin
        if(rst) A_state <= A_INIT;
        else A_state <= A_nextstate;
    end

    always @(*) begin
        case (A_state)
            A_INIT: A_nextstate = (EN && (WR) && (ADDR == 18'h2_0000))? A_FILL : A_INIT;
            A_FILL: A_nextstate = (EN && (WR) && (ADDR == (18'h1_0000 + (target_num - 1)*4)))?A_LOAD: A_FILL;
            A_LOAD: A_nextstate = (EN && (~WR) && (ADDR == 18'h3_0000) && S_state == S_DONE)? A_INIT: A_LOAD;
            default: A_nextstate = A_INIT;
        endcase
    end



    always @(posedge clk ) begin
        if(rst) target_num <= 0;
        else if(EN && (WR) && (ADDR == 18'h2_0000)) target_num <= DATAI;
    end

    reg [$clog2(BUF_LEN): 0] bufread_addr;    //the address to read RAM to FPU
    reg [$clog2(BUF_LEN): 0] bufread_upbound; //the upper bound of read (num)
    
    wire [XLEN-1 :0] buff1data_o;
    wire [XLEN-1 :0] buff2data_o;

    wire writetoram_ok = EN && WR && (ADDR[15 :2] >= 0) && (ADDR[15: 2] < BUF_LEN);
    
    distri_ram #(.ENTRY_NUM(BUF_LEN), .XLEN(XLEN))
    buffer_1(
        .clk_i(clk),
        .we_i((ADDR[17:16] == 2'b00) && writetoram_ok), // Enabled when the instruction at Decode.
        .write_addr_i(ADDR[15: 2]),        // Write addr for the instruction at Decode.
        .read_addr_i(bufread_addr),         // Read addr for Fetch.
        .data_i(DATAI), // Valid at the next cycle (instr. at Execute).
        .data_o(buff1data_o)  // Combinational read data (same cycle at Fetch).
    );

    distri_ram #(.ENTRY_NUM(BUF_LEN), .XLEN(XLEN))
    buffer_2(
        .clk_i(clk),
        .we_i((ADDR[17:16] == 2'b01) && writetoram_ok), // Enabled when the instruction at Decode.
        .write_addr_i(ADDR[15: 2]),        // Write addr for the instruction at Decode.
        .read_addr_i(bufread_addr),         // Read addr for Fetch.
        .data_i(DATAI), // Valid at the next cycle (instr. at Execute).
        .data_o(buff2data_o)  // Combinational read data (same cycle at Fetch).
    );

    always @(posedge clk ) begin
        if(rst) bufread_upbound <= 0;
        else if(A_state == A_INIT) bufread_upbound <= 0;
        else if(A_state == A_FILL)begin
            if((ADDR[17:16] == 2'b01) && writetoram_ok) bufread_upbound <= ADDR[15: 2]+1;
        end
    end


    always @(posedge clk)begin
 
        if (EN && WR)
            READY <= 1;
        else if(EN && (~WR) && S_state == S_DONE)
            READY <= 1;
        else
            READY <= 0;
    end


    reg s_axis_a_tvalid, s_axis_b_tvalid, s_axis_c_tvalid;
    wire s_axis_a_tready, s_axis_b_tready, s_axis_c_tready;
    reg [32 - 1 : 0] s_axis_a_tdata, s_axis_b_tdata, s_axis_c_tdata;

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


    


    always @(posedge clk ) begin
        if(rst) S_state <= S_INIT;
        else S_state <= S_next_state;
    end
    
    always @(*)begin
        case (S_state)
            S_INIT: S_next_state = (bufread_upbound > 0)? S_COMP: S_INIT;
            S_COMP: S_next_state = S_COMP_WAIT;
            S_COMP_WAIT: S_next_state = (m_axis_result_tvalid && ~writetoram_ok)? S_COMP_PREP: S_COMP_WAIT;
            S_COMP_PREP: S_next_state = (bufread_addr == (target_num -1))? S_DONE:S_COMP;
            S_DONE: S_next_state = (A_state == A_INIT)? S_INIT:S_DONE;
            default: S_next_state = S_INIT;
        endcase
    end

    always @(posedge clk ) begin
        if(rst) bufread_addr <= 0;
        if(S_state == S_DONE) bufread_addr <= 0;
        else if((A_state == A_LOAD || A_state == A_FILL) && (S_state == S_COMP_PREP) && (bufread_addr < (bufread_upbound))) begin 
            bufread_addr <= bufread_addr  + 1;
        end
    end

    always @(posedge clk ) begin
        if(rst)begin
            s_axis_a_tdata <= 0;
            s_axis_b_tdata <= 0;
            s_axis_c_tdata <= 0;
        end
        else if(S_state == S_COMP)begin
            s_axis_a_tdata <= buff1data_o;
            s_axis_b_tdata <= buff2data_o;
            s_axis_c_tdata <= answer_store;
            
        end
    end


    always @(posedge clk ) begin
        if(rst)begin
            s_axis_a_tvalid <= 0;
            s_axis_b_tvalid <= 0;
            s_axis_c_tvalid <= 0;
        end 
        else if(S_state == S_COMP && (bufread_upbound > 0))begin
            s_axis_a_tvalid <= 1;
            s_axis_b_tvalid <= 1;
            s_axis_c_tvalid <= 1;
        end else begin
            s_axis_a_tvalid <= 0;
            s_axis_b_tvalid <= 0;
            s_axis_c_tvalid <= 0;
        end
    end

    assign m_axis_result_tready = (S_state == S_COMP_PREP);


    always @(posedge clk)begin
        if(rst) answer_store <= 0;
        if(A_state == A_INIT) answer_store <= 0;
        if(S_state == S_COMP_PREP) answer_store <= m_axis_result_tdata;
    end

    assign DATAO = answer_store;

endmodule

`timescale 1ns / 1ps
// =============================================================================
//  Program : soc_tb.v
//  Author  : Chun-Jen Tsai
//  Date    : Feb/24/2020
// -----------------------------------------------------------------------------
//  Description:
//  This is the top-level Aquila testbench.
// -----------------------------------------------------------------------------
//  Revision information:
//
//  None.
// -----------------------------------------------------------------------------
//  License information:
//
//  This software is released under the BSD-3-Clause Licence,
//  see https://opensource.org/licenses/BSD-3-Clause for details.
//  In the following license statements, "software" refers to the
//  "source code" of the complete hardware/software system.
//
//  Copyright 2019,
//                    Embedded Intelligent Systems Lab (EISL)
//                    Deparment of Computer Science
//                    National Chiao Tung Uniersity
//                    Hsinchu, Taiwan.
//
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//
//  1. Redistributions of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//  2. Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//  3. Neither the name of the copyright holder nor the names of its contributors
//     may be used to endorse or promote products derived from this software
//     without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
// =============================================================================
`include "aquila_config.vh"

module soc_tb #( parameter XLEN = 32, parameter CLSIZE = `CLP )();

reg  ui_clk = 0;
reg  ui_rst = 1;
reg  clk = 0, rst = 0;

// uart
wire                uart_rx = 1; /* When the UART rx line is idle, it carries '1'. */
wire                uart_tx;

// --------- External memory interface -----------------------------------------
// Instruction memory ports
wire                IMEM_strobe;
wire [XLEN-1 : 0]   IMEM_addr;
wire                IMEM_done = 0;
wire [CLSIZE-1 : 0] IMEM_data = {CLSIZE{1'b0}};

// Data memory ports
wire                DMEM_strobe;
wire [XLEN-1 : 0]   DMEM_addr;
wire                DMEM_rw;
wire [CLSIZE-1 : 0] DMEM_wt_data;
wire                DMEM_done = 0;
wire [CLSIZE-1 : 0] DMEM_rd_data = {CLSIZE{1'b0}};

// --------- I/O device interface ----------------------------------------------
// Device bus signals
wire                dev_strobe;
wire [XLEN-1 : 0]   dev_addr;
wire                dev_we;
wire [XLEN/8-1 : 0] dev_be;
wire [XLEN-1 : 0]   dev_din;
wire [XLEN-1 : 0]   dev_dout;
wire                dev_ready;

// Uart
wire                uart_sel;
wire [XLEN-1 : 0]   uart_dout;
wire                uart_ready;

// --------- System Clock Generator --------------------------------------------
// Generates a 100 MHz simulated MIG clock
always 
  #5 ui_clk <= ~ui_clk;

// Generates a 41.66667 MHz system clock
always 
  #12 clk <= ~clk;

// -----------------------------------------------------------------------------
// Synchronize the reset signal across the clock domains from the
//    MIG domain (ui_rst) to the Aquila SoC domain (rst).
//
// For the Aquila domain, the reset will lasts for 15 ui_clk cycles
// to make sure all pipeline registers are properly initialized.
//
reg [3 : 0] rst_count;
reg         usr_rst;

always @(posedge ui_clk)
begin
    if (ui_rst) begin
        rst_count <= 4'd15;
        usr_rst <= 0;
    end
    else if (rst_count != 3'b0) begin
        rst_count <= rst_count - 1;
        usr_rst <= 1;
    end
    else begin
        rst_count <= 0;
        usr_rst <= 0;
    end
end

always @(posedge clk)
    rst = usr_rst;

// -----------------------------------------------------------------------------
//  Aquila processor core.
//
aquila_top Aquila_SoC
(
    .clk_i(clk),
    .rst_i(rst),          // level-sensitive reset signal.
    .base_addr_i(32'b0),  // initial program counter.

    // External instruction memory ports.
    .M_IMEM_strobe_o(IMEM_strobe),
    .M_IMEM_addr_o(IMEM_addr),
    .M_IMEM_done_i(IMEM_done),
    .M_IMEM_data_i(IMEM_data),

    // External data memory ports.
    .M_DMEM_strobe_o(DMEM_strobe),
    .M_DMEM_addr_o(DMEM_addr),
    .M_DMEM_rw_o(DMEM_rw),
    .M_DMEM_data_o(DMEM_wt_data),
    .M_DMEM_done_i(DMEM_done),
    .M_DMEM_data_i(DMEM_rd_data),

    // I/O device ports (only connects to the UART device for now).
    .M_DEVICE_strobe_o(dev_strobe),
    .M_DEVICE_addr_o(dev_addr),
    .M_DEVICE_rw_o(dev_we),
    .M_DEVICE_byte_enable_o(dev_be),
    .M_DEVICE_data_o(dev_din),
    .M_DEVICE_data_ready_i(dev_ready),
    .M_DEVICE_data_i(dev_dout)
);

// -----------------------------------------------------------------------------
//  Device address decoder.
//
//       [0] 0xC000_0000 - 0xC0FF_FFFF : UART device
//       [1] 0xC200_0000 - 0xC2FF_FFFF : DCA device
assign uart_sel  = (dev_addr[XLEN-1:XLEN-8] == 8'hC0);
assign dev_dout  = (uart_sel)? uart_dout : {XLEN{1'b0}};
assign dev_ready = (uart_sel)? uart_ready : {XLEN{1'b0}};

// ----------------------------------------------------------------------------
//  UART Controller with a simple memory-mapped I/O interface.
//
`define BAUD_RATE	115200

wire simulation_finished;

uart #(.BAUD(`SOC_CLK/`BAUD_RATE))
UART(
    .clk(clk),
    .rst(rst),

    .EN(dev_strobe & uart_sel),
    .ADDR(dev_addr[3:2]),
    .WR(dev_we),
    .BE(dev_be),
    .DATAI(dev_din),
    .DATAO(uart_dout),
    .READY(uart_ready),

    .RXD(uart_rx),
    .TXD(uart_tx),

    .simulation_done(simulation_finished)
);

always @(posedge clk)
begin
    if (simulation_finished) begin
        $display();
        $display("Simulation finished.");
        $finish();
    end
end

// ----------------------------------------------------------------------------
//  Reset logic simulation.
//
event reset_trigger;
event reset_done_trigger;

initial begin
  forever begin
    @ (reset_trigger);
    ui_rst = 1;
    @ (posedge clk);
    ui_rst = 0;
    -> reset_done_trigger;
  end
end

initial
begin: TEST_CASE
    #12 -> reset_trigger;
end

endmodule

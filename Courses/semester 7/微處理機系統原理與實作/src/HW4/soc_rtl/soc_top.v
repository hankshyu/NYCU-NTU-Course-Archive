`timescale 1ns / 1ps
// =============================================================================
//  Program : soc_top.v
//  Author  : Chun-Jen Tsai
//  Date    : Feb/16/2020
// -----------------------------------------------------------------------------
//  Description:
//  This is the top-level Aquila IP wrapper for an AXI-based processor SoC.
// -----------------------------------------------------------------------------
//  Revision information:
//
//  This module is based on the soc_top.v module written by Jin-you Wu
//  on Feb/28/2019. The original module was a stand-alone top-level module
//  for an SoC. This rework makes it a module embedded inside an AXI IP.
//
//  Jan/12/2020, by Chun-Jen Tsai:
//    Added a on-chip Tightly-Coupled Memory (TCM) to the aquila SoC.
//
//  Sep/12/2022, by Chun-Jen Tsai:
//    Fix an issue of missing reset signal across clock domains.
//    Use the clock wizard to generate the Aquila clock on Arty.
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

module soc_top #( parameter XLEN = 32, parameter CLSIZE = `CLP )
(
    input           sysclk_i,
    input           resetn_i,

    // ddr3
    output [13 : 0] ddr3_addr,
    output [2 : 0]  ddr3_ba,
    output          ddr3_cas_n,
    output [0 : 0]  ddr3_ck_n,
    output [0 : 0]  ddr3_ck_p,
    output [0 : 0]  ddr3_cke,
    output          ddr3_ras_n,
    output          ddr3_reset_n,
    output          ddr3_we_n,
    inout  [`DRAMP-1 : 0] ddr3_dq,
    inout  [`DQSP-1 : 0]  ddr3_dqs_n,
    inout  [`DQSP-1 : 0]  ddr3_dqs_p,
    output [0 : 0]  ddr3_cs_n,
    output [`DQSP-1 : 0]  ddr3_dm,
    output [0 : 0]  ddr3_odt,

    // sdcard
    output          spi_mosi,
    input           spi_miso,
    output          spi_sck,
    output [0:0]    spi_ss,

    // uart
    input           uart_rx,
    output          uart_tx,

    // buttons & leds
    input  [0 : `USRP-1]  usr_btn,
    output [0 : `USRP-1]  usr_led
);

wire clk_166M, clk_200M;
wire ui_clk, ui_rst;
wire usr_reset;
wire clk, rst;

// --------- External memory interface -----------------------------------------
// Instruction memory ports
wire                IMEM_strobe;
wire [XLEN-1 : 0]   IMEM_addr;
wire                IMEM_done;
wire [CLSIZE-1 : 0] IMEM_data;

// Data memory ports
wire                DMEM_strobe;
wire [XLEN-1 : 0]   DMEM_addr;
wire                DMEM_rw;
wire [CLSIZE-1 : 0] DMEM_wt_data;
wire                DMEM_done;
wire [CLSIZE-1 : 0] DMEM_rd_data;

// --------- I/O device interface ----------------------------------------------
// Device bus signals
wire                dev_strobe;
wire [XLEN-1 : 0]   dev_addr;
wire                dev_we;
wire [XLEN/8-1 : 0] dev_be;
wire [XLEN-1 : 0]   dev_din;
wire [XLEN-1 : 0]   dev_dout;
wire                dev_ready;

// DSA device signals (Not used for HW#0 ~ HW#4)
wire                dsa_sel;
wire [XLEN-1 : 0]   dsa_dout;
wire                dsa_ready;

// Uart
wire                uart_sel;
wire [XLEN-1 : 0]   uart_dout;
wire                uart_ready;


// --------- cdc_sync ----------------------------------------------------------
// Instruction Memory
wire                IMEM_strobe_ui_clk;
wire [XLEN-1 : 0]   IMEM_addr_ui_clk;
wire                IMEM_done_ui_clk;
wire [CLSIZE-1 : 0] IMEM_data_ui_clk;

// Data Memory
wire                DMEM_strobe_ui_clk;
wire [XLEN-1 : 0]   DMEM_addr_ui_clk;
wire                DMEM_rw_ui_clk;
wire [CLSIZE-1 : 0] DMEM_wt_data_ui_clk;
wire                DMEM_done_ui_clk;
wire [CLSIZE-1 : 0] DMEM_rd_data_ui_clk;

// --------- Memory Controller Interface ---------------------------------------
// Xilinx MIG memory controller user-logic interface signals
wire [27:0]         MEM_addr;
wire [2:0]          MEM_cmd;
wire                MEM_en;
wire [`WDFP-1:0]    MEM_wdf_data;
wire                MEM_wdf_end;
wire [`WDFP/8-1:0]  MEM_wdf_mask;
wire                MEM_wdf_wren;
wire [`WDFP-1:0]    MEM_rd_data;
wire                MEM_rd_data_end;
wire                MEM_rd_data_valid;
wire                MEM_rdy;
wire                MEM_wdf_rdy;
wire                MEM_sr_req;
wire                MEM_ref_req;
wire                MEM_zq_req;
wire                MEM_sr_active;
wire                MEM_ref_ack;
wire                MEM_zq_ack;
wire                MEM_calib;

// SPI for SD Card
wire                spi_sel;
wire [XLEN-1 : 0]   spi_dout;
wire                spi_ready;

// --------- System Clock Generator --------------------------------------------
// Generates a 41.66667 MHz system clock from the 100MHz oscillator on the PCB.
assign usr_reset = ~resetn_i;

clk_wiz_0 Clock_Generator(
    .clk_in1(sysclk_i),  // Board oscillator clock
    .clk_out1(clk),      // System clock for the Aquila SoC
    .clk_out2(clk_166M), // Clock input to the MIG Memory controller
    .clk_out3(clk_200M)  // DRAM Reference clock for MIG
);

// -----------------------------------------------------------------------------
// Synchronize the system reset signal (usr_reset) across the clock domains
//   to the Aquila SoC domains (rst).
//
// For the Aquila Core, the reset (rst) should lasts for at least 5 cycles
//   to initialize all the pipeline registers.
//
localparam SR_N = 8;
reg [SR_N-1:0] sync_reset = {SR_N{1'b1}};
assign rst = sync_reset[SR_N-1];

always @(posedge clk) begin
    if (usr_reset)
        sync_reset <= {SR_N{1'b1}};
    else
        sync_reset <= {sync_reset[SR_N-2 : 0], 1'b0};
end

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

    // I/O device ports.
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
//       [1] 0xC200_0000 - 0xC2FF_FFFF : DSA device
assign uart_sel  = (dev_addr[XLEN-1:XLEN-8] == 8'hC0);
assign dsa_sel   = (dev_addr[XLEN-1:XLEN-8] == 8'hC2);
assign dev_dout  = (uart_sel)? uart_dout : (dsa_sel)? dsa_dout : {XLEN{1'b0}};
assign dev_ready = (uart_sel)? uart_ready : (dsa_sel)? dsa_ready : {XLEN{1'b0}};

// ----------------------------------------------------------------------------
//  UART Controller with a simple memory-mapped I/O interface.
//
`define BAUD_RATE	115200

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
    .TXD(uart_tx)
);

// ----------------------------------------------------------------------------
//  cdc_sync.
//
cdc_sync synchronizer
(
    .clk_core(clk),
    .clk_memc(ui_clk),
    .rst_i(rst),

    .IMEM_strobe_i(IMEM_strobe),
    .IMEM_addr_i(IMEM_addr),
    .IMEM_done_o(IMEM_done),
    .IMEM_data_o(IMEM_data),

    .DMEM_strobe_i(DMEM_strobe),
    .DMEM_addr_i(DMEM_addr),
    .DMEM_rw_i(DMEM_rw),
    .DMEM_wt_data_i(DMEM_wt_data),
    .DMEM_done_o(DMEM_done),
    .DMEM_rd_data_o(DMEM_rd_data),

    .IMEM_strobe_o(IMEM_strobe_ui_clk),
    .IMEM_addr_o(IMEM_addr_ui_clk),
    .IMEM_done_i(IMEM_done_ui_clk),
    .IMEM_data_i(IMEM_data_ui_clk),

    .DMEM_strobe_o(DMEM_strobe_ui_clk),
    .DMEM_addr_o(DMEM_addr_ui_clk),
    .DMEM_rw_o(DMEM_rw_ui_clk),
    .DMEM_wt_data_o(DMEM_wt_data_ui_clk),
    .DMEM_done_i(DMEM_done_ui_clk),
    .DMEM_rd_data_i(DMEM_rd_data_ui_clk)
);

// ----------------------------------------------------------------------------
//  mem_arbiter.
//
mem_arbiter Memory_Arbiter
(
    // System signals
    .clk_i(ui_clk),
    .rst_i(rst),

    // Aquila M_ICACHE master port interface signals
    .S_IMEM_strobe_i(IMEM_strobe_ui_clk),
    .S_IMEM_addr_i(IMEM_addr_ui_clk),
    .S_IMEM_done_o(IMEM_done_ui_clk),
    .S_IMEM_data_o(IMEM_data_ui_clk),

    // Aquila M_DCACHE master port interface signals
    .S_DMEM_strobe_i(DMEM_strobe_ui_clk),
    .S_DMEM_addr_i(DMEM_addr_ui_clk),
    .S_DMEM_rw_i(DMEM_rw_ui_clk),
    .S_DMEM_data_i(DMEM_wt_data_ui_clk),
    .S_DMEM_done_o(DMEM_done_ui_clk),
    .S_DMEM_data_o(DMEM_rd_data_ui_clk),
    
    // memory user interface signals
    .M_MEM_addr_o(MEM_addr),
    .M_MEM_cmd_o(MEM_cmd),
    .M_MEM_en_o(MEM_en),
    .M_MEM_wdf_data_o(MEM_wdf_data),
    .M_MEM_wdf_end_o(MEM_wdf_end),
    .M_MEM_wdf_mask_o(MEM_wdf_mask),
    .M_MEM_wdf_wren_o(MEM_wdf_wren),
    .M_MEM_rd_data_i(MEM_rd_data),
    .M_MEM_rd_data_valid_i(MEM_rd_data_valid),
    .M_MEM_rdy_i(MEM_rdy),
    .M_MEM_wdf_rdy_i(MEM_wdf_rdy),
    .M_MEM_sr_req_o(MEM_sr_req),
    .M_MEM_ref_req_o(MEM_ref_req),
    .M_MEM_zq_req_o(MEM_zq_req)
);

// ----------------------------------------------------------------------------
//  MIG.
//
mig_7series_0 MIG(

    // Memory interface ports
    .ddr3_addr(ddr3_addr),                  // output [13:0]  ddr3_addr
    .ddr3_ba(ddr3_ba),                      // output [2:0]   ddr3_ba
    .ddr3_cas_n(ddr3_cas_n),                // output         ddr3_cas_n
    .ddr3_ck_n(ddr3_ck_n),                  // output [0:0]   ddr3_ck_n
    .ddr3_ck_p(ddr3_ck_p),                  // output [0:0]   ddr3_ck_p
    .ddr3_cke(ddr3_cke),                    // output [0:0]   ddr3_cke
    .ddr3_ras_n(ddr3_ras_n),                // output         ddr3_ras_n
    .ddr3_reset_n(ddr3_reset_n),            // output         ddr3_reset_n
    .ddr3_we_n(ddr3_we_n),                  // output         ddr3_we_n
    .ddr3_dq(ddr3_dq),                      // inout [15:0]   ddr3_dq
    .ddr3_dqs_n(ddr3_dqs_n),                // inout [1:0]    ddr3_dqs_n
    .ddr3_dqs_p(ddr3_dqs_p),                // inout [1:0]    ddr3_dqs_p
    .init_calib_complete(MEM_calib),        // output         init_calib_complete

    .ddr3_cs_n(ddr3_cs_n),                  // output [0:0]   ddr3_cs_n
    .ddr3_dm(ddr3_dm),                      // output [1:0]   ddr3_dm
    .ddr3_odt(ddr3_odt),                    // output [0:0]   ddr3_odt

    // Application interface ports
    .app_addr(MEM_addr),                    // input [27:0]   app_addr
    .app_cmd(MEM_cmd),                      // input [2:0]    app_cmd
    .app_en(MEM_en),                        // input          app_en
    .app_wdf_data(MEM_wdf_data),            // input [127:0]  app_wdf_data
    .app_wdf_end(MEM_wdf_end),              // input          app_wdf_end
    .app_wdf_mask(MEM_wdf_mask),            // input [15:0]   app_wdf_mask
    .app_wdf_wren(MEM_wdf_wren),            // input          app_wdf_wren
    .app_rd_data(MEM_rd_data),              // output [127:0] app_rd_data
    .app_rd_data_end(MEM_rd_data_end),      // output         app_rd_data_end
    .app_rd_data_valid(MEM_rd_data_valid),  // output         app_rd_data_valid
    .app_rdy(MEM_rdy),                      // output         app_rdy
    .app_wdf_rdy(MEM_wdf_rdy),              // output         app_wdf_rdy
    .app_sr_req(MEM_sr_req),                // input          app_sr_req
    .app_ref_req(MEM_ref_req),              // input          app_ref_req
    .app_zq_req(MEM_zq_req),                // input          app_zq_req
    .app_sr_active(MEM_sr_active),          // output         app_sr_active
    .app_ref_ack(MEM_ref_ack),              // output         app_ref_ack
    .app_zq_ack(MEM_zq_ack),                // output         app_zq_ack

    // System Clock & Reset Ports
    .sys_clk_i(clk_166M),                   // input          memory controller ref. clock
    .sys_rst(resetn_i),                     // input          sys_rst

    // 200MHz Reference Clock Ports (only needed when the ui_clk is not 200MHz)
    .clk_ref_i(clk_200M),

    // User logic reference clock and reset.
    .ui_clk(ui_clk),                        // output         ui_clk
    .ui_clk_sync_rst(ui_rst)                // output         ui_clk_sync_rst
);

endmodule

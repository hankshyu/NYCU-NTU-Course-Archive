################################################################################
# Constraint file for the Xilinx KC705 development board
################################################################################
## Clock Signal using MIG XDC
# set_property -dict {PACKAGE_PIN AD12 IOSTANDARD LVDS} [get_ports sysclk_p]
# create_clock -period 5.000 -name clk_p -waveform {0.000 2.500} -add [get_ports sysclk_p]
# set_property -dict {PACKAGE_PIN AD11 IOSTANDARD LVDS} [get_ports sysclk_n]
# create_clock -period 5.000 -name clk_n -waveform {2.500 5.000} -add [get_ports sysclk_n]

################################################################################

set_property DCI_CASCADE {32 34} [get_iobanks 33]

################################################################################
## Reset buttons
set_property -dict {PACKAGE_PIN AB7 IOSTANDARD LVCMOS15} [get_ports reset_i]

## User buttons: Up, Down, Left, Right, and Center respectively.
set_property -dict {PACKAGE_PIN AA12 IOSTANDARD LVCMOS15} [get_ports {usr_btn[0]}]
set_property -dict {PACKAGE_PIN AB12 IOSTANDARD LVCMOS15} [get_ports {usr_btn[1]}]
set_property -dict {PACKAGE_PIN AC6  IOSTANDARD LVCMOS15} [get_ports {usr_btn[2]}]
set_property -dict {PACKAGE_PIN AG5  IOSTANDARD LVCMOS15} [get_ports {usr_btn[3]}]
set_property -dict {PACKAGE_PIN  G12 IOSTANDARD LVCMOS25} [get_ports {usr_btn[4]}]

## UART
set_property -dict {PACKAGE_PIN K24 IOSTANDARD LVCMOS25} [get_ports uart_tx]
set_property -dict {PACKAGE_PIN M19 IOSTANDARD LVCMOS25} [get_ports uart_rx]

## LEDs
set_property -dict {PACKAGE_PIN AB8 IOSTANDARD LVCMOS15} [get_ports {usr_led[0]}]
set_property -dict {PACKAGE_PIN AA8 IOSTANDARD LVCMOS15} [get_ports {usr_led[1]}]
set_property -dict {PACKAGE_PIN AC9 IOSTANDARD LVCMOS15} [get_ports {usr_led[2]}]
set_property -dict {PACKAGE_PIN AB9 IOSTANDARD LVCMOS15} [get_ports {usr_led[3]}]
set_property -dict {PACKAGE_PIN G19 IOSTANDARD LVCMOS25} [get_ports {usr_led[4]}]

## Switches
#set_property -dict {PACKAGE_PIN Y29  IOSTANDARD LVCMOS25} [get_ports {usr_sw[0]}]
#set_property -dict {PACKAGE_PIN W29  IOSTANDARD LVCMOS25} [get_ports {usr_sw[1]}]
#set_property -dict {PACKAGE_PIN AA28 IOSTANDARD LVCMOS25} [get_ports {usr_sw[2]}]
#set_property -dict {PACKAGE_PIN Y28  IOSTANDARD LVCMOS25} [get_ports {usr_sw[3]}]

## 1620 LCD
#set_property -dict {PACKAGE_PIN AB10 IOSTANDARD LVCMOS15} [get_ports { LCD_E }];
#set_property -dict {PACKAGE_PIN AB13 IOSTANDARD LVCMOS15} [get_ports { LCD_RW }];
#set_property -dict {PACKAGE_PIN Y11  IOSTANDARD LVCMOS15} [get_ports { LCD_RS }];
#set_property -dict {PACKAGE_PIN Y10  IOSTANDARD LVCMOS15} [get_ports { LCD_D[3] }];
#set_property -dict {PACKAGE_PIN AA11 IOSTANDARD LVCMOS15} [get_ports { LCD_D[2] }];
#set_property -dict {PACKAGE_PIN AA10 IOSTANDARD LVCMOS15} [get_ports { LCD_D[1] }];
#set_property -dict {PACKAGE_PIN AA13 IOSTANDARD LVCMOS15} [get_ports { LCD_D[0] }];

## SDHC (Bank 12)
#Net sd_clk LOC=AB23 | IOSTANDARD=LVCMOS25;
#Net sd_dat<0> LOC=AC20 | IOSTANDARD=LVCMOS25;
#Net sd_dat<1> LOC=AA23 | IOSTANDARD=LVCMOS25;
#Net sd_dat<2> LOC=AA22 | IOSTANDARD=LVCMOS25;
#Net sd_dat<3> LOC=AC21 | IOSTANDARD=LVCMOS25;
#Net sd_cmd    LOC=AB22 | IOSTANDARD=LVCMOS25;
#Net sd_cd_n   LOC=AA21 | IOSTANDARD=LVCMOS25;
#Net sd_wp     LOC=Y21  | IOSTANDARD=LVCMOS25;
set_property -dict { PACKAGE_PIN AC20  IOSTANDARD LVCMOS25 } [get_ports { spi_miso }]; #IO_L17N_T2_35 Sch=ck_miso
set_property -dict { PACKAGE_PIN AB22  IOSTANDARD LVCMOS25 } [get_ports { spi_mosi }]; #IO_L17P_T2_35 Sch=ck_mosi
set_property -dict { PACKAGE_PIN AB23  IOSTANDARD LVCMOS25 } [get_ports { spi_sck }]; #IO_L18P_T2_35 Sch=ck_sck
set_property -dict { PACKAGE_PIN AC21  IOSTANDARD LVCMOS25 } [get_ports { spi_ss }]; #IO_L16N_T2_35 Sch=ck_ss

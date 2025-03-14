-- Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
-- --------------------------------------------------------------------------------
-- Tool Version: Vivado v.2022.1 (lin64) Build 3526262 Mon Apr 18 15:47:01 MDT 2022
-- Date        : Thu Mar 13 08:28:53 2025
-- Host        : gpu2 running 64-bit Ubuntu 20.04.6 LTS
-- Command     : write_vhdl -force -mode synth_stub -rename_top decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix -prefix
--               decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_ bd_85ad_hbm_inst_0_stub.vhdl
-- Design      : bd_85ad_hbm_inst_0
-- Purpose     : Stub declaration of top-level module interface
-- Device      : xcu55c-fsvh2892-2L-e
-- --------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix is
  Port ( 
    HBM_REF_CLK_0 : in STD_LOGIC;
    HBM_REF_CLK_1 : in STD_LOGIC;
    AXI_12_ACLK : in STD_LOGIC;
    AXI_12_ARESET_N : in STD_LOGIC;
    AXI_12_ARADDR : in STD_LOGIC_VECTOR ( 33 downto 0 );
    AXI_12_ARBURST : in STD_LOGIC_VECTOR ( 1 downto 0 );
    AXI_12_ARID : in STD_LOGIC_VECTOR ( 5 downto 0 );
    AXI_12_ARLEN : in STD_LOGIC_VECTOR ( 3 downto 0 );
    AXI_12_ARSIZE : in STD_LOGIC_VECTOR ( 2 downto 0 );
    AXI_12_ARVALID : in STD_LOGIC;
    AXI_12_AWADDR : in STD_LOGIC_VECTOR ( 33 downto 0 );
    AXI_12_AWBURST : in STD_LOGIC_VECTOR ( 1 downto 0 );
    AXI_12_AWID : in STD_LOGIC_VECTOR ( 5 downto 0 );
    AXI_12_AWLEN : in STD_LOGIC_VECTOR ( 3 downto 0 );
    AXI_12_AWSIZE : in STD_LOGIC_VECTOR ( 2 downto 0 );
    AXI_12_AWVALID : in STD_LOGIC;
    AXI_12_RREADY : in STD_LOGIC;
    AXI_12_BREADY : in STD_LOGIC;
    AXI_12_WDATA : in STD_LOGIC_VECTOR ( 255 downto 0 );
    AXI_12_WLAST : in STD_LOGIC;
    AXI_12_WSTRB : in STD_LOGIC_VECTOR ( 31 downto 0 );
    AXI_12_WDATA_PARITY : in STD_LOGIC_VECTOR ( 31 downto 0 );
    AXI_12_WVALID : in STD_LOGIC;
    AXI_14_ACLK : in STD_LOGIC;
    AXI_14_ARESET_N : in STD_LOGIC;
    AXI_14_ARADDR : in STD_LOGIC_VECTOR ( 33 downto 0 );
    AXI_14_ARBURST : in STD_LOGIC_VECTOR ( 1 downto 0 );
    AXI_14_ARID : in STD_LOGIC_VECTOR ( 5 downto 0 );
    AXI_14_ARLEN : in STD_LOGIC_VECTOR ( 3 downto 0 );
    AXI_14_ARSIZE : in STD_LOGIC_VECTOR ( 2 downto 0 );
    AXI_14_ARVALID : in STD_LOGIC;
    AXI_14_AWADDR : in STD_LOGIC_VECTOR ( 33 downto 0 );
    AXI_14_AWBURST : in STD_LOGIC_VECTOR ( 1 downto 0 );
    AXI_14_AWID : in STD_LOGIC_VECTOR ( 5 downto 0 );
    AXI_14_AWLEN : in STD_LOGIC_VECTOR ( 3 downto 0 );
    AXI_14_AWSIZE : in STD_LOGIC_VECTOR ( 2 downto 0 );
    AXI_14_AWVALID : in STD_LOGIC;
    AXI_14_RREADY : in STD_LOGIC;
    AXI_14_BREADY : in STD_LOGIC;
    AXI_14_WDATA : in STD_LOGIC_VECTOR ( 255 downto 0 );
    AXI_14_WLAST : in STD_LOGIC;
    AXI_14_WSTRB : in STD_LOGIC_VECTOR ( 31 downto 0 );
    AXI_14_WDATA_PARITY : in STD_LOGIC_VECTOR ( 31 downto 0 );
    AXI_14_WVALID : in STD_LOGIC;
    AXI_16_ACLK : in STD_LOGIC;
    AXI_16_ARESET_N : in STD_LOGIC;
    AXI_16_ARADDR : in STD_LOGIC_VECTOR ( 33 downto 0 );
    AXI_16_ARBURST : in STD_LOGIC_VECTOR ( 1 downto 0 );
    AXI_16_ARID : in STD_LOGIC_VECTOR ( 5 downto 0 );
    AXI_16_ARLEN : in STD_LOGIC_VECTOR ( 3 downto 0 );
    AXI_16_ARSIZE : in STD_LOGIC_VECTOR ( 2 downto 0 );
    AXI_16_ARVALID : in STD_LOGIC;
    AXI_16_AWADDR : in STD_LOGIC_VECTOR ( 33 downto 0 );
    AXI_16_AWBURST : in STD_LOGIC_VECTOR ( 1 downto 0 );
    AXI_16_AWID : in STD_LOGIC_VECTOR ( 5 downto 0 );
    AXI_16_AWLEN : in STD_LOGIC_VECTOR ( 3 downto 0 );
    AXI_16_AWSIZE : in STD_LOGIC_VECTOR ( 2 downto 0 );
    AXI_16_AWVALID : in STD_LOGIC;
    AXI_16_RREADY : in STD_LOGIC;
    AXI_16_BREADY : in STD_LOGIC;
    AXI_16_WDATA : in STD_LOGIC_VECTOR ( 255 downto 0 );
    AXI_16_WLAST : in STD_LOGIC;
    AXI_16_WSTRB : in STD_LOGIC_VECTOR ( 31 downto 0 );
    AXI_16_WDATA_PARITY : in STD_LOGIC_VECTOR ( 31 downto 0 );
    AXI_16_WVALID : in STD_LOGIC;
    APB_0_PWDATA : in STD_LOGIC_VECTOR ( 31 downto 0 );
    APB_0_PADDR : in STD_LOGIC_VECTOR ( 21 downto 0 );
    APB_0_PCLK : in STD_LOGIC;
    APB_0_PENABLE : in STD_LOGIC;
    APB_0_PRESET_N : in STD_LOGIC;
    APB_0_PSEL : in STD_LOGIC;
    APB_0_PWRITE : in STD_LOGIC;
    APB_1_PWDATA : in STD_LOGIC_VECTOR ( 31 downto 0 );
    APB_1_PADDR : in STD_LOGIC_VECTOR ( 21 downto 0 );
    APB_1_PCLK : in STD_LOGIC;
    APB_1_PENABLE : in STD_LOGIC;
    APB_1_PRESET_N : in STD_LOGIC;
    APB_1_PSEL : in STD_LOGIC;
    APB_1_PWRITE : in STD_LOGIC;
    AXI_12_ARREADY : out STD_LOGIC;
    AXI_12_AWREADY : out STD_LOGIC;
    AXI_12_RDATA_PARITY : out STD_LOGIC_VECTOR ( 31 downto 0 );
    AXI_12_RDATA : out STD_LOGIC_VECTOR ( 255 downto 0 );
    AXI_12_RID : out STD_LOGIC_VECTOR ( 5 downto 0 );
    AXI_12_RLAST : out STD_LOGIC;
    AXI_12_RRESP : out STD_LOGIC_VECTOR ( 1 downto 0 );
    AXI_12_RVALID : out STD_LOGIC;
    AXI_12_WREADY : out STD_LOGIC;
    AXI_12_BID : out STD_LOGIC_VECTOR ( 5 downto 0 );
    AXI_12_BRESP : out STD_LOGIC_VECTOR ( 1 downto 0 );
    AXI_12_BVALID : out STD_LOGIC;
    AXI_14_ARREADY : out STD_LOGIC;
    AXI_14_AWREADY : out STD_LOGIC;
    AXI_14_RDATA_PARITY : out STD_LOGIC_VECTOR ( 31 downto 0 );
    AXI_14_RDATA : out STD_LOGIC_VECTOR ( 255 downto 0 );
    AXI_14_RID : out STD_LOGIC_VECTOR ( 5 downto 0 );
    AXI_14_RLAST : out STD_LOGIC;
    AXI_14_RRESP : out STD_LOGIC_VECTOR ( 1 downto 0 );
    AXI_14_RVALID : out STD_LOGIC;
    AXI_14_WREADY : out STD_LOGIC;
    AXI_14_BID : out STD_LOGIC_VECTOR ( 5 downto 0 );
    AXI_14_BRESP : out STD_LOGIC_VECTOR ( 1 downto 0 );
    AXI_14_BVALID : out STD_LOGIC;
    AXI_16_ARREADY : out STD_LOGIC;
    AXI_16_AWREADY : out STD_LOGIC;
    AXI_16_RDATA_PARITY : out STD_LOGIC_VECTOR ( 31 downto 0 );
    AXI_16_RDATA : out STD_LOGIC_VECTOR ( 255 downto 0 );
    AXI_16_RID : out STD_LOGIC_VECTOR ( 5 downto 0 );
    AXI_16_RLAST : out STD_LOGIC;
    AXI_16_RRESP : out STD_LOGIC_VECTOR ( 1 downto 0 );
    AXI_16_RVALID : out STD_LOGIC;
    AXI_16_WREADY : out STD_LOGIC;
    AXI_16_BID : out STD_LOGIC_VECTOR ( 5 downto 0 );
    AXI_16_BRESP : out STD_LOGIC_VECTOR ( 1 downto 0 );
    AXI_16_BVALID : out STD_LOGIC;
    APB_0_PRDATA : out STD_LOGIC_VECTOR ( 31 downto 0 );
    APB_0_PREADY : out STD_LOGIC;
    APB_0_PSLVERR : out STD_LOGIC;
    APB_1_PRDATA : out STD_LOGIC_VECTOR ( 31 downto 0 );
    APB_1_PREADY : out STD_LOGIC;
    APB_1_PSLVERR : out STD_LOGIC;
    apb_complete_0 : out STD_LOGIC;
    apb_complete_1 : out STD_LOGIC;
    DRAM_0_STAT_CATTRIP : out STD_LOGIC;
    DRAM_0_STAT_TEMP : out STD_LOGIC_VECTOR ( 6 downto 0 );
    DRAM_1_STAT_CATTRIP : out STD_LOGIC;
    DRAM_1_STAT_TEMP : out STD_LOGIC_VECTOR ( 6 downto 0 )
  );

end decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix;

architecture stub of decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix is
attribute syn_black_box : boolean;
attribute black_box_pad_pin : string;
attribute syn_black_box of stub : architecture is true;
attribute black_box_pad_pin of stub : architecture is "HBM_REF_CLK_0,HBM_REF_CLK_1,AXI_12_ACLK,AXI_12_ARESET_N,AXI_12_ARADDR[33:0],AXI_12_ARBURST[1:0],AXI_12_ARID[5:0],AXI_12_ARLEN[3:0],AXI_12_ARSIZE[2:0],AXI_12_ARVALID,AXI_12_AWADDR[33:0],AXI_12_AWBURST[1:0],AXI_12_AWID[5:0],AXI_12_AWLEN[3:0],AXI_12_AWSIZE[2:0],AXI_12_AWVALID,AXI_12_RREADY,AXI_12_BREADY,AXI_12_WDATA[255:0],AXI_12_WLAST,AXI_12_WSTRB[31:0],AXI_12_WDATA_PARITY[31:0],AXI_12_WVALID,AXI_14_ACLK,AXI_14_ARESET_N,AXI_14_ARADDR[33:0],AXI_14_ARBURST[1:0],AXI_14_ARID[5:0],AXI_14_ARLEN[3:0],AXI_14_ARSIZE[2:0],AXI_14_ARVALID,AXI_14_AWADDR[33:0],AXI_14_AWBURST[1:0],AXI_14_AWID[5:0],AXI_14_AWLEN[3:0],AXI_14_AWSIZE[2:0],AXI_14_AWVALID,AXI_14_RREADY,AXI_14_BREADY,AXI_14_WDATA[255:0],AXI_14_WLAST,AXI_14_WSTRB[31:0],AXI_14_WDATA_PARITY[31:0],AXI_14_WVALID,AXI_16_ACLK,AXI_16_ARESET_N,AXI_16_ARADDR[33:0],AXI_16_ARBURST[1:0],AXI_16_ARID[5:0],AXI_16_ARLEN[3:0],AXI_16_ARSIZE[2:0],AXI_16_ARVALID,AXI_16_AWADDR[33:0],AXI_16_AWBURST[1:0],AXI_16_AWID[5:0],AXI_16_AWLEN[3:0],AXI_16_AWSIZE[2:0],AXI_16_AWVALID,AXI_16_RREADY,AXI_16_BREADY,AXI_16_WDATA[255:0],AXI_16_WLAST,AXI_16_WSTRB[31:0],AXI_16_WDATA_PARITY[31:0],AXI_16_WVALID,APB_0_PWDATA[31:0],APB_0_PADDR[21:0],APB_0_PCLK,APB_0_PENABLE,APB_0_PRESET_N,APB_0_PSEL,APB_0_PWRITE,APB_1_PWDATA[31:0],APB_1_PADDR[21:0],APB_1_PCLK,APB_1_PENABLE,APB_1_PRESET_N,APB_1_PSEL,APB_1_PWRITE,AXI_12_ARREADY,AXI_12_AWREADY,AXI_12_RDATA_PARITY[31:0],AXI_12_RDATA[255:0],AXI_12_RID[5:0],AXI_12_RLAST,AXI_12_RRESP[1:0],AXI_12_RVALID,AXI_12_WREADY,AXI_12_BID[5:0],AXI_12_BRESP[1:0],AXI_12_BVALID,AXI_14_ARREADY,AXI_14_AWREADY,AXI_14_RDATA_PARITY[31:0],AXI_14_RDATA[255:0],AXI_14_RID[5:0],AXI_14_RLAST,AXI_14_RRESP[1:0],AXI_14_RVALID,AXI_14_WREADY,AXI_14_BID[5:0],AXI_14_BRESP[1:0],AXI_14_BVALID,AXI_16_ARREADY,AXI_16_AWREADY,AXI_16_RDATA_PARITY[31:0],AXI_16_RDATA[255:0],AXI_16_RID[5:0],AXI_16_RLAST,AXI_16_RRESP[1:0],AXI_16_RVALID,AXI_16_WREADY,AXI_16_BID[5:0],AXI_16_BRESP[1:0],AXI_16_BVALID,APB_0_PRDATA[31:0],APB_0_PREADY,APB_0_PSLVERR,APB_1_PRDATA[31:0],APB_1_PREADY,APB_1_PSLVERR,apb_complete_0,apb_complete_1,DRAM_0_STAT_CATTRIP,DRAM_0_STAT_TEMP[6:0],DRAM_1_STAT_CATTRIP,DRAM_1_STAT_TEMP[6:0]";
attribute X_CORE_INFO : string;
attribute X_CORE_INFO of stub : architecture is "hbm_v1_0_12,Vivado 2022.1";
begin
end;

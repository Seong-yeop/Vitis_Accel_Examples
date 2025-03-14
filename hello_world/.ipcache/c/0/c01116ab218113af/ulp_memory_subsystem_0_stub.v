// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// --------------------------------------------------------------------------------
// Tool Version: Vivado v.2022.1 (lin64) Build 3526262 Mon Apr 18 15:47:01 MDT 2022
// Date        : Wed Mar 12 02:01:38 2025
// Host        : gpu2 running 64-bit Ubuntu 20.04.6 LTS
// Command     : write_verilog -force -mode synth_stub -rename_top decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix -prefix
//               decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_ ulp_memory_subsystem_0_stub.v
// Design      : ulp_memory_subsystem_0
// Purpose     : Stub declaration of top-level module interface
// Device      : xcu55c-fsvh2892-2L-e
// --------------------------------------------------------------------------------

// This empty module with port declaration file causes synthesis tools to infer a black box for IP.
// The synthesis directives are for Synopsys Synplify support to prevent IO buffer insertion.
// Please paste the declaration into a Verilog source file or add the file as an additional source.
(* X_CORE_INFO = "bd_b35e,Vivado 2022.1" *)
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix(aclk, aclk1, aclk2, aresetn, S00_AXI_awid, 
  S00_AXI_awaddr, S00_AXI_awlen, S00_AXI_awsize, S00_AXI_awburst, S00_AXI_awlock, 
  S00_AXI_awcache, S00_AXI_awprot, S00_AXI_awregion, S00_AXI_awqos, S00_AXI_awvalid, 
  S00_AXI_awready, S00_AXI_wdata, S00_AXI_wstrb, S00_AXI_wlast, S00_AXI_wvalid, 
  S00_AXI_wready, S00_AXI_bid, S00_AXI_bresp, S00_AXI_bvalid, S00_AXI_bready, S00_AXI_arid, 
  S00_AXI_araddr, S00_AXI_arlen, S00_AXI_arsize, S00_AXI_arburst, S00_AXI_arlock, 
  S00_AXI_arcache, S00_AXI_arprot, S00_AXI_arregion, S00_AXI_arqos, S00_AXI_arvalid, 
  S00_AXI_arready, S00_AXI_rid, S00_AXI_rdata, S00_AXI_rresp, S00_AXI_rlast, S00_AXI_rvalid, 
  S00_AXI_rready, S01_AXI_awid, S01_AXI_awaddr, S01_AXI_awlen, S01_AXI_awsize, 
  S01_AXI_awburst, S01_AXI_awlock, S01_AXI_awcache, S01_AXI_awprot, S01_AXI_awregion, 
  S01_AXI_awqos, S01_AXI_awvalid, S01_AXI_awready, S01_AXI_wdata, S01_AXI_wstrb, 
  S01_AXI_wlast, S01_AXI_wvalid, S01_AXI_wready, S01_AXI_bid, S01_AXI_bresp, S01_AXI_bvalid, 
  S01_AXI_bready, S01_AXI_arid, S01_AXI_araddr, S01_AXI_arlen, S01_AXI_arsize, 
  S01_AXI_arburst, S01_AXI_arlock, S01_AXI_arcache, S01_AXI_arprot, S01_AXI_arregion, 
  S01_AXI_arqos, S01_AXI_arvalid, S01_AXI_arready, S01_AXI_rid, S01_AXI_rdata, S01_AXI_rresp, 
  S01_AXI_rlast, S01_AXI_rvalid, S01_AXI_rready, S02_AXI_awaddr, S02_AXI_awlen, 
  S02_AXI_awburst, S02_AXI_awlock, S02_AXI_awcache, S02_AXI_awprot, S02_AXI_awregion, 
  S02_AXI_awqos, S02_AXI_awvalid, S02_AXI_awready, S02_AXI_wdata, S02_AXI_wstrb, 
  S02_AXI_wlast, S02_AXI_wvalid, S02_AXI_wready, S02_AXI_bresp, S02_AXI_bvalid, 
  S02_AXI_bready, S02_AXI_araddr, S02_AXI_arlen, S02_AXI_arburst, S02_AXI_arlock, 
  S02_AXI_arcache, S02_AXI_arprot, S02_AXI_arregion, S02_AXI_arqos, S02_AXI_arvalid, 
  S02_AXI_arready, S02_AXI_rdata, S02_AXI_rresp, S02_AXI_rlast, S02_AXI_rvalid, 
  S02_AXI_rready, S03_AXI_araddr, S03_AXI_arlen, S03_AXI_arcache, S03_AXI_arprot, 
  S03_AXI_arregion, S03_AXI_arqos, S03_AXI_arvalid, S03_AXI_arready, S03_AXI_rdata, 
  S03_AXI_rresp, S03_AXI_rlast, S03_AXI_rvalid, S03_AXI_rready, S04_AXI_awaddr, 
  S04_AXI_awlen, S04_AXI_awcache, S04_AXI_awprot, S04_AXI_awregion, S04_AXI_awqos, 
  S04_AXI_awvalid, S04_AXI_awready, S04_AXI_wdata, S04_AXI_wstrb, S04_AXI_wlast, 
  S04_AXI_wvalid, S04_AXI_wready, S04_AXI_bresp, S04_AXI_bvalid, S04_AXI_bready, 
  S05_AXI_awaddr, S05_AXI_awlen, S05_AXI_awcache, S05_AXI_awprot, S05_AXI_awregion, 
  S05_AXI_awqos, S05_AXI_awvalid, S05_AXI_awready, S05_AXI_wdata, S05_AXI_wstrb, 
  S05_AXI_wlast, S05_AXI_wvalid, S05_AXI_wready, S05_AXI_bresp, S05_AXI_bvalid, 
  S05_AXI_bready, S06_AXI_araddr, S06_AXI_arlen, S06_AXI_arcache, S06_AXI_arprot, 
  S06_AXI_arregion, S06_AXI_arqos, S06_AXI_arvalid, S06_AXI_arready, S06_AXI_rdata, 
  S06_AXI_rresp, S06_AXI_rlast, S06_AXI_rvalid, S06_AXI_rready, S07_AXI_awaddr, 
  S07_AXI_awlen, S07_AXI_awcache, S07_AXI_awprot, S07_AXI_awregion, S07_AXI_awqos, 
  S07_AXI_awvalid, S07_AXI_awready, S07_AXI_wdata, S07_AXI_wstrb, S07_AXI_wlast, 
  S07_AXI_wvalid, S07_AXI_wready, S07_AXI_bresp, S07_AXI_bvalid, S07_AXI_bready, 
  M00_AXI_awid, M00_AXI_awaddr, M00_AXI_awlen, M00_AXI_awburst, M00_AXI_awvalid, 
  M00_AXI_awready, M00_AXI_wdata, M00_AXI_wstrb, M00_AXI_wlast, M00_AXI_wvalid, 
  M00_AXI_wready, M00_AXI_bid, M00_AXI_bresp, M00_AXI_bvalid, M00_AXI_bready, M00_AXI_arid, 
  M00_AXI_araddr, M00_AXI_arlen, M00_AXI_arburst, M00_AXI_arvalid, M00_AXI_arready, 
  M00_AXI_rid, M00_AXI_rdata, M00_AXI_rresp, M00_AXI_rlast, M00_AXI_rvalid, M00_AXI_rready, 
  S02_AXI_arid, S02_AXI_awid, S02_AXI_bid, S02_AXI_rid)
/* synthesis syn_black_box black_box_pad_pin="aclk,aclk1,aclk2,aresetn,S00_AXI_awid[3:0],S00_AXI_awaddr[38:0],S00_AXI_awlen[7:0],S00_AXI_awsize[2:0],S00_AXI_awburst[1:0],S00_AXI_awlock[0:0],S00_AXI_awcache[3:0],S00_AXI_awprot[2:0],S00_AXI_awregion[3:0],S00_AXI_awqos[3:0],S00_AXI_awvalid,S00_AXI_awready,S00_AXI_wdata[511:0],S00_AXI_wstrb[63:0],S00_AXI_wlast,S00_AXI_wvalid,S00_AXI_wready,S00_AXI_bid[3:0],S00_AXI_bresp[1:0],S00_AXI_bvalid,S00_AXI_bready,S00_AXI_arid[3:0],S00_AXI_araddr[38:0],S00_AXI_arlen[7:0],S00_AXI_arsize[2:0],S00_AXI_arburst[1:0],S00_AXI_arlock[0:0],S00_AXI_arcache[3:0],S00_AXI_arprot[2:0],S00_AXI_arregion[3:0],S00_AXI_arqos[3:0],S00_AXI_arvalid,S00_AXI_arready,S00_AXI_rid[3:0],S00_AXI_rdata[511:0],S00_AXI_rresp[1:0],S00_AXI_rlast,S00_AXI_rvalid,S00_AXI_rready,S01_AXI_awid[3:0],S01_AXI_awaddr[38:0],S01_AXI_awlen[7:0],S01_AXI_awsize[2:0],S01_AXI_awburst[1:0],S01_AXI_awlock[0:0],S01_AXI_awcache[3:0],S01_AXI_awprot[2:0],S01_AXI_awregion[3:0],S01_AXI_awqos[3:0],S01_AXI_awvalid,S01_AXI_awready,S01_AXI_wdata[511:0],S01_AXI_wstrb[63:0],S01_AXI_wlast,S01_AXI_wvalid,S01_AXI_wready,S01_AXI_bid[3:0],S01_AXI_bresp[1:0],S01_AXI_bvalid,S01_AXI_bready,S01_AXI_arid[3:0],S01_AXI_araddr[38:0],S01_AXI_arlen[7:0],S01_AXI_arsize[2:0],S01_AXI_arburst[1:0],S01_AXI_arlock[0:0],S01_AXI_arcache[3:0],S01_AXI_arprot[2:0],S01_AXI_arregion[3:0],S01_AXI_arqos[3:0],S01_AXI_arvalid,S01_AXI_arready,S01_AXI_rid[3:0],S01_AXI_rdata[511:0],S01_AXI_rresp[1:0],S01_AXI_rlast,S01_AXI_rvalid,S01_AXI_rready,S02_AXI_awaddr[37:0],S02_AXI_awlen[7:0],S02_AXI_awburst[1:0],S02_AXI_awlock[0:0],S02_AXI_awcache[3:0],S02_AXI_awprot[2:0],S02_AXI_awregion[3:0],S02_AXI_awqos[3:0],S02_AXI_awvalid,S02_AXI_awready,S02_AXI_wdata[511:0],S02_AXI_wstrb[63:0],S02_AXI_wlast,S02_AXI_wvalid,S02_AXI_wready,S02_AXI_bresp[1:0],S02_AXI_bvalid,S02_AXI_bready,S02_AXI_araddr[37:0],S02_AXI_arlen[7:0],S02_AXI_arburst[1:0],S02_AXI_arlock[0:0],S02_AXI_arcache[3:0],S02_AXI_arprot[2:0],S02_AXI_arregion[3:0],S02_AXI_arqos[3:0],S02_AXI_arvalid,S02_AXI_arready,S02_AXI_rdata[511:0],S02_AXI_rresp[1:0],S02_AXI_rlast,S02_AXI_rvalid,S02_AXI_rready,S03_AXI_araddr[37:0],S03_AXI_arlen[7:0],S03_AXI_arcache[3:0],S03_AXI_arprot[2:0],S03_AXI_arregion[3:0],S03_AXI_arqos[3:0],S03_AXI_arvalid,S03_AXI_arready,S03_AXI_rdata[127:0],S03_AXI_rresp[1:0],S03_AXI_rlast,S03_AXI_rvalid,S03_AXI_rready,S04_AXI_awaddr[37:0],S04_AXI_awlen[7:0],S04_AXI_awcache[3:0],S04_AXI_awprot[2:0],S04_AXI_awregion[3:0],S04_AXI_awqos[3:0],S04_AXI_awvalid,S04_AXI_awready,S04_AXI_wdata[31:0],S04_AXI_wstrb[3:0],S04_AXI_wlast,S04_AXI_wvalid,S04_AXI_wready,S04_AXI_bresp[1:0],S04_AXI_bvalid,S04_AXI_bready,S05_AXI_awaddr[37:0],S05_AXI_awlen[7:0],S05_AXI_awcache[3:0],S05_AXI_awprot[2:0],S05_AXI_awregion[3:0],S05_AXI_awqos[3:0],S05_AXI_awvalid,S05_AXI_awready,S05_AXI_wdata[31:0],S05_AXI_wstrb[3:0],S05_AXI_wlast,S05_AXI_wvalid,S05_AXI_wready,S05_AXI_bresp[1:0],S05_AXI_bvalid,S05_AXI_bready,S06_AXI_araddr[37:0],S06_AXI_arlen[7:0],S06_AXI_arcache[3:0],S06_AXI_arprot[2:0],S06_AXI_arregion[3:0],S06_AXI_arqos[3:0],S06_AXI_arvalid,S06_AXI_arready,S06_AXI_rdata[63:0],S06_AXI_rresp[1:0],S06_AXI_rlast,S06_AXI_rvalid,S06_AXI_rready,S07_AXI_awaddr[37:0],S07_AXI_awlen[7:0],S07_AXI_awcache[3:0],S07_AXI_awprot[2:0],S07_AXI_awregion[3:0],S07_AXI_awqos[3:0],S07_AXI_awvalid,S07_AXI_awready,S07_AXI_wdata[63:0],S07_AXI_wstrb[7:0],S07_AXI_wlast,S07_AXI_wvalid,S07_AXI_wready,S07_AXI_bresp[1:0],S07_AXI_bvalid,S07_AXI_bready,M00_AXI_awid[1:0],M00_AXI_awaddr[38:0],M00_AXI_awlen[7:0],M00_AXI_awburst[1:0],M00_AXI_awvalid,M00_AXI_awready,M00_AXI_wdata[511:0],M00_AXI_wstrb[63:0],M00_AXI_wlast,M00_AXI_wvalid,M00_AXI_wready,M00_AXI_bid[1:0],M00_AXI_bresp[1:0],M00_AXI_bvalid,M00_AXI_bready,M00_AXI_arid[1:0],M00_AXI_araddr[38:0],M00_AXI_arlen[7:0],M00_AXI_arburst[1:0],M00_AXI_arvalid,M00_AXI_arready,M00_AXI_rid[1:0],M00_AXI_rdata[511:0],M00_AXI_rresp[1:0],M00_AXI_rlast,M00_AXI_rvalid,M00_AXI_rready,S02_AXI_arid[3:0],S02_AXI_awid[3:0],S02_AXI_bid[3:0],S02_AXI_rid[3:0]" */;
  input aclk;
  input aclk1;
  input aclk2;
  input aresetn;
  input [3:0]S00_AXI_awid;
  input [38:0]S00_AXI_awaddr;
  input [7:0]S00_AXI_awlen;
  input [2:0]S00_AXI_awsize;
  input [1:0]S00_AXI_awburst;
  input [0:0]S00_AXI_awlock;
  input [3:0]S00_AXI_awcache;
  input [2:0]S00_AXI_awprot;
  input [3:0]S00_AXI_awregion;
  input [3:0]S00_AXI_awqos;
  input S00_AXI_awvalid;
  output S00_AXI_awready;
  input [511:0]S00_AXI_wdata;
  input [63:0]S00_AXI_wstrb;
  input S00_AXI_wlast;
  input S00_AXI_wvalid;
  output S00_AXI_wready;
  output [3:0]S00_AXI_bid;
  output [1:0]S00_AXI_bresp;
  output S00_AXI_bvalid;
  input S00_AXI_bready;
  input [3:0]S00_AXI_arid;
  input [38:0]S00_AXI_araddr;
  input [7:0]S00_AXI_arlen;
  input [2:0]S00_AXI_arsize;
  input [1:0]S00_AXI_arburst;
  input [0:0]S00_AXI_arlock;
  input [3:0]S00_AXI_arcache;
  input [2:0]S00_AXI_arprot;
  input [3:0]S00_AXI_arregion;
  input [3:0]S00_AXI_arqos;
  input S00_AXI_arvalid;
  output S00_AXI_arready;
  output [3:0]S00_AXI_rid;
  output [511:0]S00_AXI_rdata;
  output [1:0]S00_AXI_rresp;
  output S00_AXI_rlast;
  output S00_AXI_rvalid;
  input S00_AXI_rready;
  input [3:0]S01_AXI_awid;
  input [38:0]S01_AXI_awaddr;
  input [7:0]S01_AXI_awlen;
  input [2:0]S01_AXI_awsize;
  input [1:0]S01_AXI_awburst;
  input [0:0]S01_AXI_awlock;
  input [3:0]S01_AXI_awcache;
  input [2:0]S01_AXI_awprot;
  input [3:0]S01_AXI_awregion;
  input [3:0]S01_AXI_awqos;
  input S01_AXI_awvalid;
  output S01_AXI_awready;
  input [511:0]S01_AXI_wdata;
  input [63:0]S01_AXI_wstrb;
  input S01_AXI_wlast;
  input S01_AXI_wvalid;
  output S01_AXI_wready;
  output [3:0]S01_AXI_bid;
  output [1:0]S01_AXI_bresp;
  output S01_AXI_bvalid;
  input S01_AXI_bready;
  input [3:0]S01_AXI_arid;
  input [38:0]S01_AXI_araddr;
  input [7:0]S01_AXI_arlen;
  input [2:0]S01_AXI_arsize;
  input [1:0]S01_AXI_arburst;
  input [0:0]S01_AXI_arlock;
  input [3:0]S01_AXI_arcache;
  input [2:0]S01_AXI_arprot;
  input [3:0]S01_AXI_arregion;
  input [3:0]S01_AXI_arqos;
  input S01_AXI_arvalid;
  output S01_AXI_arready;
  output [3:0]S01_AXI_rid;
  output [511:0]S01_AXI_rdata;
  output [1:0]S01_AXI_rresp;
  output S01_AXI_rlast;
  output S01_AXI_rvalid;
  input S01_AXI_rready;
  input [37:0]S02_AXI_awaddr;
  input [7:0]S02_AXI_awlen;
  input [1:0]S02_AXI_awburst;
  input [0:0]S02_AXI_awlock;
  input [3:0]S02_AXI_awcache;
  input [2:0]S02_AXI_awprot;
  input [3:0]S02_AXI_awregion;
  input [3:0]S02_AXI_awqos;
  input S02_AXI_awvalid;
  output S02_AXI_awready;
  input [511:0]S02_AXI_wdata;
  input [63:0]S02_AXI_wstrb;
  input S02_AXI_wlast;
  input S02_AXI_wvalid;
  output S02_AXI_wready;
  output [1:0]S02_AXI_bresp;
  output S02_AXI_bvalid;
  input S02_AXI_bready;
  input [37:0]S02_AXI_araddr;
  input [7:0]S02_AXI_arlen;
  input [1:0]S02_AXI_arburst;
  input [0:0]S02_AXI_arlock;
  input [3:0]S02_AXI_arcache;
  input [2:0]S02_AXI_arprot;
  input [3:0]S02_AXI_arregion;
  input [3:0]S02_AXI_arqos;
  input S02_AXI_arvalid;
  output S02_AXI_arready;
  output [511:0]S02_AXI_rdata;
  output [1:0]S02_AXI_rresp;
  output S02_AXI_rlast;
  output S02_AXI_rvalid;
  input S02_AXI_rready;
  input [37:0]S03_AXI_araddr;
  input [7:0]S03_AXI_arlen;
  input [3:0]S03_AXI_arcache;
  input [2:0]S03_AXI_arprot;
  input [3:0]S03_AXI_arregion;
  input [3:0]S03_AXI_arqos;
  input S03_AXI_arvalid;
  output S03_AXI_arready;
  output [127:0]S03_AXI_rdata;
  output [1:0]S03_AXI_rresp;
  output S03_AXI_rlast;
  output S03_AXI_rvalid;
  input S03_AXI_rready;
  input [37:0]S04_AXI_awaddr;
  input [7:0]S04_AXI_awlen;
  input [3:0]S04_AXI_awcache;
  input [2:0]S04_AXI_awprot;
  input [3:0]S04_AXI_awregion;
  input [3:0]S04_AXI_awqos;
  input S04_AXI_awvalid;
  output S04_AXI_awready;
  input [31:0]S04_AXI_wdata;
  input [3:0]S04_AXI_wstrb;
  input S04_AXI_wlast;
  input S04_AXI_wvalid;
  output S04_AXI_wready;
  output [1:0]S04_AXI_bresp;
  output S04_AXI_bvalid;
  input S04_AXI_bready;
  input [37:0]S05_AXI_awaddr;
  input [7:0]S05_AXI_awlen;
  input [3:0]S05_AXI_awcache;
  input [2:0]S05_AXI_awprot;
  input [3:0]S05_AXI_awregion;
  input [3:0]S05_AXI_awqos;
  input S05_AXI_awvalid;
  output S05_AXI_awready;
  input [31:0]S05_AXI_wdata;
  input [3:0]S05_AXI_wstrb;
  input S05_AXI_wlast;
  input S05_AXI_wvalid;
  output S05_AXI_wready;
  output [1:0]S05_AXI_bresp;
  output S05_AXI_bvalid;
  input S05_AXI_bready;
  input [37:0]S06_AXI_araddr;
  input [7:0]S06_AXI_arlen;
  input [3:0]S06_AXI_arcache;
  input [2:0]S06_AXI_arprot;
  input [3:0]S06_AXI_arregion;
  input [3:0]S06_AXI_arqos;
  input S06_AXI_arvalid;
  output S06_AXI_arready;
  output [63:0]S06_AXI_rdata;
  output [1:0]S06_AXI_rresp;
  output S06_AXI_rlast;
  output S06_AXI_rvalid;
  input S06_AXI_rready;
  input [37:0]S07_AXI_awaddr;
  input [7:0]S07_AXI_awlen;
  input [3:0]S07_AXI_awcache;
  input [2:0]S07_AXI_awprot;
  input [3:0]S07_AXI_awregion;
  input [3:0]S07_AXI_awqos;
  input S07_AXI_awvalid;
  output S07_AXI_awready;
  input [63:0]S07_AXI_wdata;
  input [7:0]S07_AXI_wstrb;
  input S07_AXI_wlast;
  input S07_AXI_wvalid;
  output S07_AXI_wready;
  output [1:0]S07_AXI_bresp;
  output S07_AXI_bvalid;
  input S07_AXI_bready;
  output [1:0]M00_AXI_awid;
  output [38:0]M00_AXI_awaddr;
  output [7:0]M00_AXI_awlen;
  output [1:0]M00_AXI_awburst;
  output M00_AXI_awvalid;
  input M00_AXI_awready;
  output [511:0]M00_AXI_wdata;
  output [63:0]M00_AXI_wstrb;
  output M00_AXI_wlast;
  output M00_AXI_wvalid;
  input M00_AXI_wready;
  input [1:0]M00_AXI_bid;
  input [1:0]M00_AXI_bresp;
  input M00_AXI_bvalid;
  output M00_AXI_bready;
  output [1:0]M00_AXI_arid;
  output [38:0]M00_AXI_araddr;
  output [7:0]M00_AXI_arlen;
  output [1:0]M00_AXI_arburst;
  output M00_AXI_arvalid;
  input M00_AXI_arready;
  input [1:0]M00_AXI_rid;
  input [511:0]M00_AXI_rdata;
  input [1:0]M00_AXI_rresp;
  input M00_AXI_rlast;
  input M00_AXI_rvalid;
  output M00_AXI_rready;
  input [3:0]S02_AXI_arid;
  input [3:0]S02_AXI_awid;
  output [3:0]S02_AXI_bid;
  output [3:0]S02_AXI_rid;
endmodule

// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// --------------------------------------------------------------------------------
// Tool Version: Vivado v.2022.1 (lin64) Build 3526262 Mon Apr 18 15:47:01 MDT 2022
// Date        : Wed Mar 12 02:03:59 2025
// Host        : gpu2 running 64-bit Ubuntu 20.04.6 LTS
// Command     : write_verilog -force -mode funcsim -rename_top decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix -prefix
//               decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_ bd_22c0_gapping_demand_toggle_0_sim_netlist.v
// Design      : bd_22c0_gapping_demand_toggle_0
// Purpose     : This verilog netlist is a functional simulation representation of the design and should not be modified
//               or synthesized. This netlist cannot be used for SDF annotated simulation.
// Device      : xcu55c-fsvh2892-2L-e
// --------------------------------------------------------------------------------
`timescale 1 ps / 1 ps

(* CHECK_LICENSE_TYPE = "bd_22c0_gapping_demand_toggle_0,c_counter_binary_v12_0_15,{}" *) (* downgradeipidentifiedwarnings = "yes" *) (* x_core_info = "c_counter_binary_v12_0_15,Vivado 2022.1" *) 
(* NotValidForBitStream *)
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix
   (CLK,
    CE,
    Q);
  (* x_interface_info = "xilinx.com:signal:clock:1.0 clk_intf CLK" *) (* x_interface_parameter = "XIL_INTERFACENAME clk_intf, ASSOCIATED_BUSIF q_intf:thresh0_intf:l_intf:load_intf:up_intf:sinit_intf:sset_intf, ASSOCIATED_RESET SCLR, ASSOCIATED_CLKEN CE, FREQ_HZ 50000000, FREQ_TOLERANCE_HZ 0, PHASE 0, CLK_DOMAIN cd_ctrl_00, INSERT_VIP 0" *) input CLK;
  (* x_interface_info = "xilinx.com:signal:clockenable:1.0 ce_intf CE" *) (* x_interface_parameter = "XIL_INTERFACENAME ce_intf, POLARITY ACTIVE_HIGH" *) input CE;
  (* x_interface_info = "xilinx.com:signal:data:1.0 q_intf DATA" *) (* x_interface_parameter = "XIL_INTERFACENAME q_intf, LAYERED_METADATA xilinx.com:interface:datatypes:1.0 {DATA {datatype {name {attribs {resolve_type immediate dependency {} format string minimum {} maximum {}} value data} bitwidth {attribs {resolve_type generated dependency bitwidth format long minimum {} maximum {}} value 1} bitoffset {attribs {resolve_type immediate dependency {} format long minimum {} maximum {}} value 0} integer {signed {attribs {resolve_type immediate dependency {} format bool minimum {} maximum {}} value false}}}} DATA_WIDTH 1}" *) output [0:0]Q;

  wire CE;
  wire CLK;
  wire [0:0]Q;
  wire NLW_U0_THRESH0_UNCONNECTED;

  (* C_AINIT_VAL = "0" *) 
  (* C_CE_OVERRIDES_SYNC = "0" *) 
  (* C_COUNT_BY = "1" *) 
  (* C_COUNT_MODE = "0" *) 
  (* C_COUNT_TO = "1" *) 
  (* C_FB_LATENCY = "0" *) 
  (* C_HAS_CE = "1" *) 
  (* C_HAS_LOAD = "0" *) 
  (* C_HAS_SCLR = "0" *) 
  (* C_HAS_SINIT = "0" *) 
  (* C_HAS_SSET = "0" *) 
  (* C_HAS_THRESH0 = "0" *) 
  (* C_IMPLEMENTATION = "0" *) 
  (* C_LATENCY = "1" *) 
  (* C_LOAD_LOW = "0" *) 
  (* C_RESTRICT_COUNT = "0" *) 
  (* C_SCLR_OVERRIDES_SSET = "1" *) 
  (* C_SINIT_VAL = "0" *) 
  (* C_THRESH0_VALUE = "1" *) 
  (* C_VERBOSITY = "0" *) 
  (* C_WIDTH = "1" *) 
  (* C_XDEVICEFAMILY = "virtexuplusHBM" *) 
  (* downgradeipidentifiedwarnings = "yes" *) 
  (* is_du_within_envelope = "true" *) 
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_c_counter_binary_v12_0_15 U0
       (.CE(CE),
        .CLK(CLK),
        .L(1'b0),
        .LOAD(1'b0),
        .Q(Q),
        .SCLR(1'b0),
        .SINIT(1'b0),
        .SSET(1'b0),
        .THRESH0(NLW_U0_THRESH0_UNCONNECTED),
        .UP(1'b1));
endmodule
`pragma protect begin_protected
`pragma protect version = 1
`pragma protect encrypt_agent = "XILINX"
`pragma protect encrypt_agent_info = "Xilinx Encryption Tool 2022.1"
`pragma protect key_keyowner="Synopsys", key_keyname="SNPS-VCS-RSA-2", key_method="rsa"
`pragma protect encoding = (enctype="BASE64", line_length=76, bytes=128)
`pragma protect key_block
etX3SrY3g8TAXqvd6Iw5SvKCZZW0eO22ydOZWWDKv0XmkGG+9M8FsJMD1dG+u6RHY7gdvUOfoViN
wYkRFkkBiOKUODn3KY+yorf/HZ9bJsxi4Q2Ru+Zt4ivE2MW+zxqBWPhCkRfS/ucHGuO95naXnjgL
lb9i6CyTczDpW4UI/Ik=

`pragma protect key_keyowner="Aldec", key_keyname="ALDEC15_001", key_method="rsa"
`pragma protect encoding = (enctype="BASE64", line_length=76, bytes=256)
`pragma protect key_block
ocAlwZgQjNXrZcvxNs8IW79dzfyE9fvD0AHIqJ8SeTEjMRupN9qSyJOEgFaNpPxUIuu0svukJu0R
IqatA+wGaribd/22wO76Mj0aoD9bGE3VsaN0pR3+Y8YoU+IGyyVvjDG9mNbukvYTSJ5ZEMHsUOyi
imiBAjKNItKHyfLmbgwiEIR1oZvFzOU1gKu//rXEuLNx3KotU0MiuqklmmKAGGJ254g/V2yZiNe4
HQ5tbfx8TDma7/NxoBfjg3Bvq2/eHzXnOj06HGJUgjWu2IDATi1Tf48miMjPHWhgHLGKzWaqgjyq
g35QHtqKIkgMv0ig+cxF8SaXK1SIsQAmTNFhtw==

`pragma protect key_keyowner="Mentor Graphics Corporation", key_keyname="MGC-VELOCE-RSA", key_method="rsa"
`pragma protect encoding = (enctype="BASE64", line_length=76, bytes=128)
`pragma protect key_block
WIYuLymN2Nv/rvoxxWXQJUzJMgD17q8uGZGaBtC6iR4pcjbP2LoAhSDAWPDJ8U0aLYG9J5uhEBJr
/OOxx4nmKJUgsWFJzpj1iAmBvs57SxHV6jPVhnslDdyPjeSb8osDZ9b6tft6h9988MculRQRvGgI
HSSSC/8vkkw0e/Kc6U8=

`pragma protect key_keyowner="Mentor Graphics Corporation", key_keyname="MGC-VERIF-SIM-RSA-2", key_method="rsa"
`pragma protect encoding = (enctype="BASE64", line_length=76, bytes=256)
`pragma protect key_block
T7BkeDa870ApjbtvZ7/2khjAHJx69MvPtsnIFkvlpIJh1CZzfvHg0ZYY8clsVgB/Qtk+a1wUS8wP
HoB0f6b47CZuLyVfYbdve2ifMZVtreXVYOjP81VEm/bz/Y0YCGUToVzTEgzkTUAMwsVq461ePNjA
1OIMAhX2UTCZemmI+t1xc7UUPsprpRtQFsD2AycglI1VCpO2g9OrLgGhEOq4Rs+qIncV13whpF+6
ow3SfiEWSC5yWeJWQVSILtpitCj/uLGV7JhZOtyk+GNqgtNN/kUEXukyTmH4gtr3R0Weq0Ta/7UA
jEeryu6BSvjECQ/4CXkcsLZx21gswFcMgdy/+w==

`pragma protect key_keyowner="Real Intent", key_keyname="RI-RSA-KEY-1", key_method="rsa"
`pragma protect encoding = (enctype="BASE64", line_length=76, bytes=256)
`pragma protect key_block
0K8CiEosnZGPGhb4bfIxrv7D6G0rwpTFuE0sxW9JWE7hZOw6ejqpy+L7rBIjbtQWBlwBxjJyyBOr
DN7TKWiVYDmvUwUJk5GOYGGm5leq/LSFNBRH/isZvo0qJMN6vy+9FVe9M5YSMEnFs3rXqyLGptPM
gknJThr/VhNLOJvbCnPFMl8hUL88Z45aFQY6pQypIxO9aSVD6FNrIjSWvSKBsp47joDxHbjgngCK
aEyWu5FCOvZink3ultoCC36AMpVuoei/rz5cE2lS/JnA0EHecKtNxb5UyEX0k1RkI7Y573ErJHDK
672KED4gm/zneBtNCYGxcXUo/DgfrPXjUWRXUw==

`pragma protect key_keyowner="Xilinx", key_keyname="xilinxt_2021_07", key_method="rsa"
`pragma protect encoding = (enctype="BASE64", line_length=76, bytes=256)
`pragma protect key_block
ngjVz6+RURV0NXze0ScoKCqO+Sw1oGm0rynAKadmo2FAqjTc2+/XedUU3UcU7YuXAAIBHrw6La+O
HsUGGhJmLx8cMtddX+L3oymDlcoxkaqSJlhDigmTawK44PlNtYqssNQN+pco7blJLk9/oqmMyuuI
0Wv6/OqwjGRH5z/FOF0exK65OVlKVFKwibJXjsGH/Df3LafmiyeS7qcveJ91nS4q4IjFSTY9WZUX
IOIJ4vGlDtY4jnS3eRHUIX4X59Zj8yId5U0Hs/FrKzCYV07oUy2rTvltPwkFj5vg9G4CgEunNPc3
3wcZb74KDE6Jse+vxtK+i7H3Sy6iUu5AjmD7lQ==

`pragma protect key_keyowner="Metrics Technologies Inc.", key_keyname="DSim", key_method="rsa"
`pragma protect encoding = (enctype="BASE64", line_length=76, bytes=256)
`pragma protect key_block
f1eoK6dk/UeyS6WKTzZq+16+eooDXoHX+8BhhaCu54Rj9FahkQz9QhJf/wF2+AXgmFOom5ukrVr/
v3BFE8gnMHUnQOWBDrNewnalpxoqLTj0jO8w/Dle08RaL0nJ37F9JYfwvY5BrUNuYrd9jm4Q+gB0
u6pWiPYTtwxvIy8UrGz4Lw3INaGxrFOeuFuh0YLyAPcmIH4Zx3g0P1uoDBRXeFh0lrPfeb/VHh5N
MbBMMd1ZBfBORTEAEfeCKHodK2Ag6VELMOcNvRsngPNUprRlwyoznZgx4EGP5uknUWJ28szVxQKF
hNPRhe1ZYPmHS/A+jmSlcs4nFOlLq5lMa8WNJg==

`pragma protect key_keyowner="Atrenta", key_keyname="ATR-SG-RSA-1", key_method="rsa"
`pragma protect encoding = (enctype="BASE64", line_length=76, bytes=384)
`pragma protect key_block
D2RwxcSch3LPUkN/XFzw4CqPI9z++PJYhbEiPcZO87hBRRGQz6upuyWq6R370Oarp7pn7/dYPDqt
BkvR5v+SmGaDOxyTErt8Co4qFR+t2eYDF4qA7aL/JJhR4GWjSGL2n1wte4fAvKHAWxVPnbeXY72h
ZguOZdrKfH9i/zlBQEOOb0+u7d/zavZwXNleLCIWGHlwdEPZ2IzMpccItAx9AfOcRFrSvGee8a5U
f5T+IFSfqdIKLE2xvfpShFqr0PAS0UyIztiDz2s2L/3PCTeZb6PAUq6+nRC1d+a4Zudo8eU3vQrp
XRQH5XfY7HDEFp0FwSdVwpTbpOICVYutMg+WqWNsOilITHjrqhtdJ1eakkJh42/iXuGHm74DygNx
YIudsp166RXGfy6PRpADb15n+U2Zs3i2bs2PvhiXdfa6XgrRZheG6hqE/gGZ8jzRMOrN6U73Um3f
nRmIBNssHc+NUR19lvdaQcG8t3iv4mFgPCPzhwuPMfEJ4K9vXupKZRZC

`pragma protect key_keyowner="Cadence Design Systems.", key_keyname="CDS_RSA_KEY_VER_1", key_method="rsa"
`pragma protect encoding = (enctype="BASE64", line_length=76, bytes=256)
`pragma protect key_block
GiBTRr15aQ7ZFVD9DA3YCzxBU25RUBqeLM6/zaVyX1C2MTwaDcj7eUsmHPTuLrbz8wqCpKE70+mi
MFXYoP3RM/M7Qvdjz8Wr99Xd2NrlOmB9eNpCWyr2JEhHBVFXtK8Pg+eYu1T83oiQfLd5qKA0acct
+ZviXiAOaPDluQKhBNJuucJJsMIT/pXw0sQ5H7Vwf/PJKdX4bVALFvpra39UXluHDWce+mK6eI5N
olI4dHLhpo6pN6FFWESTD+2KoftBx3MDImLjocOGF7pOhGSNxB81JS3xtKYXvyKUKVXinrL/Rt7H
VT02aL+3dCLG1Vx5Jt8tUOXwye0BQsDmf8/vvg==

`pragma protect key_keyowner="Mentor Graphics Corporation", key_keyname="MGC-PREC-RSA", key_method="rsa"
`pragma protect encoding = (enctype="BASE64", line_length=76, bytes=256)
`pragma protect key_block
PR9SefeB0ObYmaAC5FfYaQwF8HfztKKpYxqd2VQ5zA1tHljZqEwUX+bpunsebT0uHuztgCZd0bxz
678GMjJ4I9672mnlLFrIDGsRK1Fm4pvDEkbx3JglO9fkjhOuRarxWexiEY2cS3EMVC/s1KOBP5tB
S84kcHAbYGpwqqrZu2eyTDAm6PtXhvk79etIxUmwWU/Jh1I7sTdqb+rvTzIq7+8e15xE+/keYRlL
3S3TQXPH9Ik+e38LVZtzYkeeZFEYmjsKat+iKEu4WX/xh9SYb1fGD9Rbq1mjkJ+P8LdU0WaESzor
xhipL4VoSpVLX2mI9MENnjKlUu27jW9ECXotAw==

`pragma protect key_keyowner="Synplicity", key_keyname="SYNP15_1", key_method="rsa"
`pragma protect encoding = (enctype="BASE64", line_length=76, bytes=256)
`pragma protect key_block
QESwJXr9r1dNMpiXe5rVC6MHkDdeqtun670EBPgILaUhJoG2pd2p6hCPL1lNlHNyoOmTG7W21Xo+
CliVPLeB8pcHkhehI3ZV5bkVE6pLAcwtOzoUdmJ5fNH6P2kzeeZolksvJhgejENaDlH1GS7O2HHi
mhMmfNrSADVF/rXVZ74/cFhB+I5+MZU0D7S+0hTMh3ObaUVhxFnobT74apIafGIdX1aKxFIPo+M2
uvTbRu2LeUt/iAEzV9hPTW48il1pymgA0b/Y+G8NuXb4Ka+oYdYdEOHQzenROxsPegdVdOrPGcu+
X2U2YEPg5R6T42WTSSC3va+ebTkzIie4Pf7+Qw==

`pragma protect data_method = "AES128-CBC"
`pragma protect encoding = (enctype = "BASE64", line_length = 76, bytes = 5168)
`pragma protect data_block
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
`pragma protect end_protected
`ifndef GLBL
`define GLBL
`timescale  1 ps / 1 ps

module glbl ();

    parameter ROC_WIDTH = 100000;
    parameter TOC_WIDTH = 0;
    parameter GRES_WIDTH = 10000;
    parameter GRES_START = 10000;

//--------   STARTUP Globals --------------
    wire GSR;
    wire GTS;
    wire GWE;
    wire PRLD;
    wire GRESTORE;
    tri1 p_up_tmp;
    tri (weak1, strong0) PLL_LOCKG = p_up_tmp;

    wire PROGB_GLBL;
    wire CCLKO_GLBL;
    wire FCSBO_GLBL;
    wire [3:0] DO_GLBL;
    wire [3:0] DI_GLBL;
   
    reg GSR_int;
    reg GTS_int;
    reg PRLD_int;
    reg GRESTORE_int;

//--------   JTAG Globals --------------
    wire JTAG_TDO_GLBL;
    wire JTAG_TCK_GLBL;
    wire JTAG_TDI_GLBL;
    wire JTAG_TMS_GLBL;
    wire JTAG_TRST_GLBL;

    reg JTAG_CAPTURE_GLBL;
    reg JTAG_RESET_GLBL;
    reg JTAG_SHIFT_GLBL;
    reg JTAG_UPDATE_GLBL;
    reg JTAG_RUNTEST_GLBL;

    reg JTAG_SEL1_GLBL = 0;
    reg JTAG_SEL2_GLBL = 0 ;
    reg JTAG_SEL3_GLBL = 0;
    reg JTAG_SEL4_GLBL = 0;

    reg JTAG_USER_TDO1_GLBL = 1'bz;
    reg JTAG_USER_TDO2_GLBL = 1'bz;
    reg JTAG_USER_TDO3_GLBL = 1'bz;
    reg JTAG_USER_TDO4_GLBL = 1'bz;

    assign (strong1, weak0) GSR = GSR_int;
    assign (strong1, weak0) GTS = GTS_int;
    assign (weak1, weak0) PRLD = PRLD_int;
    assign (strong1, weak0) GRESTORE = GRESTORE_int;

    initial begin
	GSR_int = 1'b1;
	PRLD_int = 1'b1;
	#(ROC_WIDTH)
	GSR_int = 1'b0;
	PRLD_int = 1'b0;
    end

    initial begin
	GTS_int = 1'b1;
	#(TOC_WIDTH)
	GTS_int = 1'b0;
    end

    initial begin 
	GRESTORE_int = 1'b0;
	#(GRES_START);
	GRESTORE_int = 1'b1;
	#(GRES_WIDTH);
	GRESTORE_int = 1'b0;
    end

endmodule
`endif

// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// --------------------------------------------------------------------------------
// Tool Version: Vivado v.2022.1 (lin64) Build 3526262 Mon Apr 18 15:47:01 MDT 2022
// Date        : Wed Mar 12 01:53:33 2025
// Host        : gpu2 running 64-bit Ubuntu 20.04.6 LTS
// Command     : write_verilog -force -mode funcsim -rename_top decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix -prefix
//               decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_ bd_58f6_lut_buffer_0_sim_netlist.v
// Design      : bd_58f6_lut_buffer_0
// Purpose     : This verilog netlist is a functional simulation representation of the design and should not be modified
//               or synthesized. This netlist cannot be used for SDF annotated simulation.
// Device      : xcu55c-fsvh2892-2L-e
// --------------------------------------------------------------------------------
`timescale 1 ps / 1 ps

(* CHECK_LICENSE_TYPE = "bd_58f6_lut_buffer_0,lut_buffer_v2_0_0_lut_buffer,{}" *) (* DowngradeIPIdentifiedWarnings = "yes" *) (* X_CORE_INFO = "lut_buffer_v2_0_0_lut_buffer,Vivado 2022.1" *) 
(* NotValidForBitStream *)
module decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix
   (tdi_i,
    tms_i,
    tck_i,
    drck_i,
    sel_i,
    shift_i,
    update_i,
    capture_i,
    runtest_i,
    reset_i,
    bscanid_en_i,
    tdo_o,
    tdi_o,
    tms_o,
    tck_o,
    drck_o,
    sel_o,
    shift_o,
    update_o,
    capture_o,
    runtest_o,
    reset_o,
    bscanid_en_o,
    tdo_i);
  (* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 s_bscan TDI" *) input tdi_i;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 s_bscan TMS" *) input tms_i;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 s_bscan TCK" *) input tck_i;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 s_bscan DRCK" *) input drck_i;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 s_bscan SEL" *) input sel_i;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 s_bscan SHIFT" *) input shift_i;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 s_bscan UPDATE" *) input update_i;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 s_bscan CAPTURE" *) input capture_i;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 s_bscan RUNTEST" *) input runtest_i;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 s_bscan RESET" *) input reset_i;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 s_bscan BSCANID_EN" *) input bscanid_en_i;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 s_bscan TDO" *) output tdo_o;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 m_bscan TDI" *) output tdi_o;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 m_bscan TMS" *) output tms_o;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 m_bscan TCK" *) output tck_o;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 m_bscan DRCK" *) output drck_o;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 m_bscan SEL" *) output sel_o;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 m_bscan SHIFT" *) output shift_o;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 m_bscan UPDATE" *) output update_o;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 m_bscan CAPTURE" *) output capture_o;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 m_bscan RUNTEST" *) output runtest_o;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 m_bscan RESET" *) output reset_o;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 m_bscan BSCANID_EN" *) output bscanid_en_o;
  (* X_INTERFACE_INFO = "xilinx.com:interface:bscan:1.0 m_bscan TDO" *) input tdo_i;

  wire bscanid_en_i;
  wire bscanid_en_o;
  wire capture_i;
  wire capture_o;
  wire drck_i;
  wire drck_o;
  wire reset_i;
  wire reset_o;
  wire runtest_i;
  wire runtest_o;
  wire sel_i;
  wire sel_o;
  wire shift_i;
  wire shift_o;
  wire tck_i;
  wire tck_o;
  wire tdi_i;
  wire tdi_o;
  wire tdo_i;
  wire tdo_o;
  wire tms_i;
  wire tms_o;
  wire update_i;
  wire update_o;
  wire [31:0]NLW_inst_bscanid_o_UNCONNECTED;

  (* C_EN_BSCANID_VEC = "0" *) 
  (* DONT_TOUCH *) 
  (* KEEP_HIERARCHY = "soft" *) 
  (* is_du_within_envelope = "true" *) 
  decalper_eb_ot_sdeen_pot_pi_dehcac_xnilix_lut_buffer_v2_0_0_lut_buffer inst
       (.bscanid_en_i(bscanid_en_i),
        .bscanid_en_o(bscanid_en_o),
        .bscanid_i({1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0,1'b0}),
        .bscanid_o(NLW_inst_bscanid_o_UNCONNECTED[31:0]),
        .capture_i(capture_i),
        .capture_o(capture_o),
        .drck_i(drck_i),
        .drck_o(drck_o),
        .reset_i(reset_i),
        .reset_o(reset_o),
        .runtest_i(runtest_i),
        .runtest_o(runtest_o),
        .sel_i(sel_i),
        .sel_o(sel_o),
        .shift_i(shift_i),
        .shift_o(shift_o),
        .tck_i(tck_i),
        .tck_o(tck_o),
        .tdi_i(tdi_i),
        .tdi_o(tdi_o),
        .tdo_i(tdo_i),
        .tdo_o(tdo_o),
        .tms_i(tms_i),
        .tms_o(tms_o),
        .update_i(update_i),
        .update_o(update_o));
endmodule
`pragma protect begin_protected
`pragma protect version = 1
`pragma protect encrypt_agent = "XILINX"
`pragma protect encrypt_agent_info = "Xilinx Encryption Tool 2022.1"
`pragma protect key_keyowner="Synopsys", key_keyname="SNPS-VCS-RSA-2", key_method="rsa"
`pragma protect encoding = (enctype="BASE64", line_length=76, bytes=128)
`pragma protect key_block
f1j0yiOflyV2kLMBe+hsL0MouEX0Wv67RiMU1n4QYTAQ1VnAgpLBrFb17v/9GYVLUPddIDNOGPRZ
u2WV424zInW/Vmf6dhRXe1SsADWvbW7FiGPDw4jSrHnTNCEJCn+cfjSxhjMmAkuHG1fir3l+gxGz
L/X05RxFmZ2QL5UIwRk=

`pragma protect key_keyowner="Aldec", key_keyname="ALDEC15_001", key_method="rsa"
`pragma protect encoding = (enctype="BASE64", line_length=76, bytes=256)
`pragma protect key_block
0i7KENkA2NQO3GXaWrbdCJnWHApRZRcJ0tLb3D4baBRVZkAd2BJDMxoSUwnRKToSUEazPqGR4ywK
WqTfdpMetoN7s7CYXDjMoygLqtE11hWxTqG3rxD4MHY+oxhHIbr0ICQdK58tH2gBIeD4Y8rjtc1M
L01bGVrzL348HIiDA0SPDZYJeem4+87SeBTvZP3vuJPPdCuxDmudnIAXnqy6CGab6C9RgMyi/LlF
L0I7jQB9ONBLqrj8KcUrPrduGmiyShXt/NwzRFGoTEY3E0lYXHgJkdReoCCCmYa5+FNOs18Qwno+
YkWOQhWFcPvWa4g6Y/ouRdk7jPEz/jq+GzkkPg==

`pragma protect key_keyowner="Mentor Graphics Corporation", key_keyname="MGC-VELOCE-RSA", key_method="rsa"
`pragma protect encoding = (enctype="BASE64", line_length=76, bytes=128)
`pragma protect key_block
KNOVCEjQhCCxVJzUYfNJKAf2haBXTHL4u8VRnkMGr2AZ4CjhCD2LNR3uxAto+kgqlGvllYk6mDuX
ehYphfcWZHnD/uMO9Mlgm2xYmmqRUjk15bM5mfH+RhWUKFD6VRSygM9P06naXp5l6yAxCjsaJoia
sn/MOoxmT2wVf3n+Xdg=

`pragma protect key_keyowner="Mentor Graphics Corporation", key_keyname="MGC-VERIF-SIM-RSA-2", key_method="rsa"
`pragma protect encoding = (enctype="BASE64", line_length=76, bytes=256)
`pragma protect key_block
V2yPa0MI9WR304IOhrhuSQ9Ldvhw0yC0csbY2Yup99Z9agkgpsDjW+77nBHVwkwZmgUyfGSo0TK2
5+kz3fJgn/RD2oSYkSaBVXzvKItV8acak2gfA2kdbaCCDklOwfdWWqRGAWts/1Mg5clfcdwoNOZ1
m3ZVTEFXXRdnaVotagNm2zuupNsX/J509sWqJwxqY/tkapVzbizogJQlBDpHRmeI8Wy9MHgl+ez8
ErgBkS1v84iUGQ0VVXOdzfpn7Rr+JkKzknuKR21+imEMhw/nro+d3J6z9gL9sw/hot77/E45Wlv0
J6rMye1iHGBgTJ/ongJJ/bF2sU0UIep3jo3Ycw==

`pragma protect key_keyowner="Real Intent", key_keyname="RI-RSA-KEY-1", key_method="rsa"
`pragma protect encoding = (enctype="BASE64", line_length=76, bytes=256)
`pragma protect key_block
Gsa5eR6EJ+H0y0tfeQrSF62h+z9EjOrg9Nx7r8uw/ePJ78O1ebL7fqko1w5NXOXLbrE6GH0vB5PG
/sp4Ez/hK7CAozkTRRLKNpEiEjYs2xf/XWbCy8m7MjJL3tGVSrP4bCa5emQEsIcEi8ZXNnVw4khq
FTPzKuxstMIRAptl0GnPBlQC7elSuw6Li8Olj1CTQU7VlBFtuNyAt5Onc/FWt3H78x5rf4h+wZAC
rCL/7OKx01xqTpSd7s35OB0kE6C75KSwa0lxluUz3o07qQoZ1FuaWMdqLjSuwyBAqL97DAkcrtNs
SGDdpgTIT+dK7ICRArioBCEKt/kMjmL/uID0Hg==

`pragma protect key_keyowner="Metrics Technologies Inc.", key_keyname="DSim", key_method="rsa"
`pragma protect encoding = (enctype="BASE64", line_length=76, bytes=256)
`pragma protect key_block
h+kjW5M8Dem541WFtDf09jjHpUR1/+CL7YNUyFmXM5qvgxmmSz2XXkwnCuEbvknqy0Yh9fpgPEtw
P8UmTkW5JrTQVdJEaqXXg5foMh4iNGQJirjH4SbGQ/YuI2eV97IrrEygr9Cs4lTCJJxvBYMzkZ8O
7k00rA51JoBJVpXt/5T5upbqQiFhJ6MU6avXSC4vezl93MB5UrxRfDP7E7q9m0W+xAcLwxYEwl4B
1E52gJSYNRV3KH5tXZTsXURVzpZ4rgpGuhnTqaRIbzdHDFBO5R2HLAM6qPpwH3iXqmCPpwhcuL/C
jBAzhuIZ3U12mvg6St28AxyWYwWi2IAz4uzfgg==

`pragma protect key_keyowner="Xilinx", key_keyname="xilinxt_2021_07", key_method="rsa"
`pragma protect encoding = (enctype="BASE64", line_length=76, bytes=256)
`pragma protect key_block
dtmv56Ko4bFw0rY2VV0lIolvnvL8ZqJOHXaQX9ROldne6mUD/6S3uCZJeduFtssQ5PyvwzGhKGxi
7um7D/9xiCOrGpJTZ8BXDmmL9x6GCuNjAfNCfBGJsQEf6celtP5Wb+/KyqzKylMTmOn4jYDpwKnv
7aGYgDcY7bLQPstQJ1lqpnIzpsXokk2yfTXesH1TE5QAebLuUzMJkhjk+/0Png8FOhnM2GWK/nNN
tYDjJVOBtj/HBwhT/lFXucnPSnqCy/bh0ObsNNSD5sUoHhUk00KBNeDEdIGHT7MgOiRMEgRxIVBr
fE2b4uHkwE3B9zjQ5ayuPIdobQKIH9uajxa1Cw==

`pragma protect key_keyowner="Atrenta", key_keyname="ATR-SG-RSA-1", key_method="rsa"
`pragma protect encoding = (enctype="BASE64", line_length=76, bytes=384)
`pragma protect key_block
nP7sH9m1/BWzEDOejD42H89YXpl36cWXtZO9mIisNlEpVUC6G6NkFQXuEvVhxWDlr/ANE2zhdLuk
pUjX5zCIatBQEjjG7DxiP93OzjAUtlcezRl2EdP8WONFhAXwbtW+FjwvaygqnYLOFuX1NySRVtIm
JnNXiZV1065+1Z3tuINtTEoj7vWpyiY6keja/lhk6y7N1vDfbpJnxwC8XF6qagRF2f3BSidj4gRD
kLvmVWetDWRwL3rbqsFqAryiMwdmn+bzBSuKJX0DvUZ9Mf5pfVjh6oeaUiV+N8VsCoGG4AYJMW6a
BDdl5Vq35R9zL+M25sLYEKaG07cYl0Exhxk6oVzhqw/+PO//HHdYa8S6U3zet2RHV0BUpC+IxVj6
r/daUxvI5ChIZKtIsApJvvJjybQUYIZz/lMNvxQY//moq88zjpwQ13CeVbxZA4cEo3LTdka/RIet
ScU9pHox1Y+im5MusVfpMJQNihowibveX0/XnfK1an+3HLXxA2daxGiD

`pragma protect key_keyowner="Cadence Design Systems.", key_keyname="CDS_RSA_KEY_VER_1", key_method="rsa"
`pragma protect encoding = (enctype="BASE64", line_length=76, bytes=256)
`pragma protect key_block
gZo52SHmX+/9ltS9SXeTGQ5BAPUpW+A6ImhJo+12gKxivRKWEgEcZyBRgM7Hj6mtw9eEPmcH7UuO
9vD90A9QvItgrbujaZxLVL7i94ZAamx40/J5wjA0HPDTV75t68r0vy1qwPQ3sZeu6XwipquHvOWj
ujFI8+tIW+YWMlunpQwGZ3rGMizIBhMobkp1BGodxvf99yEznW3Oi9Y/TJINC1QCqR6JK9Z4AscZ
EZf7XVSb7dPw0bFtiw6ZaZQmyvhRQYrO8FiaZJj2P7cxrUIuDAEpK0DY1MdJ5NKVuNFFKweNyC3/
3oyciyDOZSr6a1Ow3A2imATmKbSnAKV6h9qoMg==

`pragma protect data_method = "AES128-CBC"
`pragma protect encoding = (enctype = "BASE64", line_length = 76, bytes = 2960)
`pragma protect data_block
BZmF/qB2Nu+0Vbalz6EXwhKBfprO3DuxYHdzNtf7nfdPlRQsl7tuAHRxeCzNLD1S1NaqXTDwdd58
S189wjJZLtVP5QQtXT5rKfFdiAYJLg6ixR+u++hTW1bXNy1fGEAxMjQgkFHdmn7T6Agy6rZed1J+
nD07fPO4hkwpKyfrTE/aEac9Ffzr8A6oShb0iY6mLS8uQ4MgYbURWRzrJR87Sv34nVTK2t8F0wu+
C6eLVe9/E1WqWi0sf20dJcx+JyxNk0KwfC0Pi5dFF4CZ7Z188RNfq2jPRvIQqfSFgcrw8/jwS4i6
SHtxuHt5ERk3Ei+sTOdjyfFnmRLcigO2sMg8yyk0X8v9XESBLh10OvYXOxX0L249LU22i5EyoTKa
t05hdXRoByqJZW452XMOcp/idssr7V6emQfl+oVfP+veATluOuV2iZEmSafKxV13cSW6xJ4hvwFp
Nj0WQ14/NOVwQQ+gTbnw90plEgw3qwnyIXh5+MKjYqakQM5UODDp0ofIWDJxglNAPGdS1vSyPsDy
omlkaNJoHZmalRPuTEzrtMLq0iREBfVMMbTXI4R0Lxr7LfKsCaQvPRQRxiYArLyv3RI9mkj7r5B8
DgE05XX/DmFLXWWrTVsFPzogmt7oyl3oGZiWcj4tDQie8qBHEnQbPzP5mhijqPqdW0fIiWmvG29G
hSotaMqxCg0MGIFqqctafjkU/jZzFrI6U58v8vHiSJZCD00xRQbZRQj5/S2bt5g480aUj3a1UTRO
y7DyH02yIiWOvN+/OimHML8Cy//Jza1D21pcSIWETgR+X49hIfGsLJVmqDhSw6BlGLvmQlifTGEe
35n/ZTsodyHqoDfZJDlbTH535bBeOPg5qb2kVFzhNSt+nyWD8pCZ/Hgsl1I/EpFQ1vxx5PcVv+G+
ZiKHuPfjO5gEFmAIHUlmlZ2g5lhrWIHWO+9+bqeAskTTMY9ZOQFF4xUohqdQssSH6X42hahLv0Bj
BucbNcxmo9yyJfRRzDKhnjTTQycYDsVRxWfK3mQn9WVK4MuJGNTL2L1jN+bctmWVZzFOK30Anjan
6mob5NrOdSiB2KGS+nWotePjX77Ey8LHdnAg9R9MpMKEAvQXwr8/fbzT+T8yCDvXVhT0Lw3rxPM3
s/2FPcb80LwNROQjfvaX95/sn7WjtXKm44kXMvz7xMVhAVV+saaquXCbya5LrWuR/OXirtOchgcV
g3AoeJXARKwXLdC7KPXka5PvHCnlncpd5U5VX3JHhK6hM/vRJrwzFAF4IR6o5JbN9dUd5Msnn5nJ
ukvMK5v1pv6pvRr14dO46Xm/gPringN2iapcNw+IqmGeC1z1/Zdis7WeoyCDAikN1wLhJe8yhP4L
Kx7YdM5vICXTQaGsEy9o41mLlYFfs0qZeuYYDomJ+gOe7BGESXtl73uf6rPnOg4cKB1QeCF8nRG9
JkeKPAVgkZOtzupYE67Xo5RXHW1kf9Dx2qb3kTxhYFLQIRxtLvuV8ODGe3kvVMyCEuslSnl+JIQx
TW7KYVuJqDDsQH4WuXRMkTSqLh0xG83xALqUcoek9PhlauCRfmLgg0XZkhr9W7LcgHB6/YE1/DTO
JwQ2JVxvSnNxG/koL+78bX0xdlH396a/Q8lDDfZ1pZn2MLmjKL884I3ATo8r5WMaFxNp/UQFNfxx
y+5zLyu6WYKNUa9UMvokjljehNqVkoouMOF0NOsORTXh/E23KcaEsIxe4AVHOg0p5QbAEfSCg/4h
XijhI87l7X2u4geqinnQy0fMDdf65xkI0TG5e4boARP7dm/B3jz6XdfEfNasjFjOQHaZvSXCoOig
FWBmdTFCk3ahityGkG20EaO2IBPkZ2waEQlN4Cupey95DmByS044vuOZ4FU9PhIx38cNQnIyx13O
2gQ4x5G5IBz5PzJa64Wd//YKolVsrTY0ytX3uRrsOMlpZWBsbvAVNiul2ebGvfewaLSQi8gFXVJP
9UoaLriR+lWobp+Qff5M9XR1O9CHAGMjibpR3FkO4K7XTlzdkmq0r+sVjSFi9+OEbf++wqQUGCfY
XvT9QFBYAQOokSmgbOMT25xA5GA+oE53YW/HIOKGygVQF/ldLp+s+QRLrIJU6RzPCKm7Ydxt6jXg
bn6Vg0BlM9YSS4r1afILusJeA0GwrZg+/rELrckROsutej/Z3h6ysp27fhPnf6AALHSVdkZf32jJ
S5AdHb75Wuc54I9obdQ1z4F6fkSlsg1U8rQwZYO0cL4FLm6esrEJRwAcvXD8GwJiWZDtckjdDS2p
DimS2ICtpUjPirXV8uze70EMWhpSRp7hbM25ytvGQziJAoX3BGG92ry+gNTP5SlA2kFB2zF7P/go
1PnIebA/14/CUSH7SVsCg/lMc/IkkWJd3qwUcRXEDW43BpLjoASjCMwaDKzZaShWjNfNXUrq0bAj
5qUbNdNYMs0dmimbwNHL1PvwfyXSPBjjMIA8Vx3z4Kl4Qz+8On0501+ezd/1GXXuLNZaXpooFdkx
n0lhtDZi71Ys97xSMdrokAP1oGsLRRms96kOZQdOqhPUpbPTbyLr32c5nssuyW6ie+ZAOFD+E3QW
SZlqZ+4Ff0Kr4lnxxl5iH/CmRYcv7Thl7Q6x1Y6hSDBkmbqWkb2ytyHj8vUzC8MH8KdQtZVgrH+J
I+XQHnUN+jnB9sCAF5hOMtjqWbiCa9t8apNf4TkTCEvmKInxSq0OY+Lajbq348cwoiCI6aOmkNsP
H9HYPxWBsFmk6RO+vaWtsdzVZfmRrreA1tevSyDMBJNbrUst0JaZD/yr/GjwMcWRcfD+Zr5E+pf2
0/qjSsBYwbAYu9MAUYzyCVFoAq2kzFJracnDW42WLQChprQkWSDl81njzMhdfE0PuvsbiZBvm4ih
eZt9z/lBZ/MIW4+4s79oxZ8e8Z5VBQnr1yxoiIwmIkeuN5MvJdpqvb5Raej92nFqaYuytqJhRzJl
cNcljr9gkhv+5RdkXp6hOaO3n6SCh0CXZddpiURV9NzR4ItzC7TGlxUikbh8wOCHzIBbLD7Ckc5Z
qzkYNN//tzyVjjXqhTdQDMJWeAfXomgGQLKonjc+InYBnRjVa+3SvfZuOWxSD6yDWZtjUzWZ/w6+
Wy+1/AtPakYLkGjnJv7cBDxoiOlNltvNAmkK6WAiZkWQgqnVHp+BMZiG7ZPIw/zbXrzFEyeGpIXK
ckvRvodiBZZCgpCjWOWjH5Wd93ccVfTSLiUDoRsP2J/ZoYmkqVIS6G4fm5z36QOOyonQGncxOotR
pYjGjYspYk+plTMA+a/tATHo3mygd3M7ba0hgF+lZE3g+uF+LmQrsjFGNQYFw9oL3sO6de49VlHr
s5lPiId2Ts5aF8U7e81qrJw6JUFuNwyoI4MUGrbiTiV+8kL35kz3/dQZZU/zBcSzN2J4i8r+ECgx
4s7F2NWQI6+c9QfEU2FlJ9Qhx6qPuY0ljT4iz7Oxoc+1eBd6hmmuiWKnYrA7w8HjjaoKvJrB6t+I
x+07emxK3aZcas8dmvI7pYPL5PsxHaueffgobcbkvwTtPxQ3mfId0QM0BB7GozEWQypt9MzuV9Dh
PQRl4dgv4WME38z839zCzXgQqvdEx0uOrmEfi4u2zNa9wUtJpTj60Q/4058m0Vnu5/Hn6hz8unCf
jNFAEN1yTQC3Rv0box6OuwX63YG5K+MnLpi0SZhWLNrh5zjjU9ChLJlJLIakM+xr2yvT/55DrlSa
MSqjZE4YZqcRWkNWpTcGR4EW+ReTDg+e/3J7SUFqHffa40zEruCbx2kugELtReQ2mwV2iNgFdpKG
DG7fl6yE9AFbUQhubVXway2ojE9PQDPLlL1wu/8I3TEFaEQXugnUHdk0AZi3UCj0nwiJ1wX6aEG2
BZMwjlxnIEl+dWN5bROiLrl8vibObzr7vGvRQVT546lCa8TcMasDQDZ5q66L0Hl8eQR53M0=
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

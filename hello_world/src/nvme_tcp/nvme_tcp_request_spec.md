 Figure 26: Initialize Connection Request PDU (ICReq)
 ┌────────────────────────────────────────────────────────────────────────────────┐
 │ Bytes  | Field/Section                                │ Description            │
 ├────────────────────────────────────────────────────────────────────────────────┤
 │ 00     | CH: PDU Type (PDU-Type)                      │ 00h                    │
 ├────────────────────────────────────────────────────────────────────────────────┤
 │ 01     | Flags (FLAGS):                               │                        │
 │        │   Bit 7: Kickstart Discovery Connection      │                      │
 │        │          (KDCONN): If '1' → kickstart discovery    NVMe/TCP           │
 │        │          connection (see Kickstart Discovery Pull      Registration   │
 │        │          Requests in NVM Express Base Spec, section 3.1).             │
 │        │          If '0' → non-kickstart discovery connection.                  │
 │        │   Bits 6:0: Reserved                           │                      │
 ├─────────────────────────────────────────────────────────────────────────────┤
 │ 02     | Header Length (HLEN)                         │ Fixed 128 bytes (80h)│
 ├─────────────────────────────────────────────────────────────────────────────┤
 │ 03     | PDU Data Offset (PDO)                        │ Reserved             │
 ├─────────────────────────────────────────────────────────────────────────────┤
 │ 07:04  | PDU Length (PLEN)                            │ Fixed 128 bytes (80h)│
 ├─────────────────────────────────────────────────────────────────────────────┤
 │ 09:08  | PSH: PDU Format Version (PFV)                │ Format version; set  │
 │        |                                              │ to 0h                │
 ├─────────────────────────────────────────────────────────────────────────────┤
 │ 10     | Host PDU Data Alignment (HPDA)               │ Alignment for PDUs   │
 │        |                                              │ from controller to   │
 │        |                                              │ host; expressed as a │
 │        |                                              │ 0’s based value in   │
 │        |                                              │ dwords (e.g., 0 → 4 bytes, 1 → 8 bytes, 2 → 12 bytes) │
 ├─────────────────────────────────────────────────────────────────────────────┤
 │ 11     | Digest (DGST):                             │ Host PDU header &    │
 │        |   Bits 7:2: Reserved                         │ data digest enable   │
 │        |   Bit 1: PDU Data Digest Enable (DDGST_ENABLE)│ option               │
 │        |           - '1': Data digest requested         │
 │        |           - '0': Data digest not used          │
 │        |   Bit 0: PDU Header Digest Enable (HDGST_ENABLE)│ option              │
 │        |           - '1': Header digest requested       │
 │        |           - '0': Header digest not used        │
 ├─────────────────────────────────────────────────────────────────────────────┤
 │ 15:12  | Maximum Number of Outstanding R2T (MAXR2T)   │ 0’s based value; max │
 │        |                                              │ outstanding R2T PDUs for a command │
 ├─────────────────────────────────────────────────────────────────────────────┤
 │ 127:16 | Reserved                                     │                      │
 └─────────────────────────────────────────────────────────────────────────────┘
 
 /*
 * Figure 30: Command Capsule PDU (CapsuleCmd)
 *
 * ┌─────────────────────────────────────────────────────────────────────────────────────────────┐
 * │ Byte(s)    │ Field / Section                      │ Description                              │
 * ├─────────────────────────────────────────────────────────────────────────────────────────────┤
 * │ 00         │ CH: PDU Type (PDU-Type)              │ 04h                                      │
 * ├─────────────────────────────────────────────────────────────────────────────────────────────┤
 * │ 01         │ Flags (FLAGS)                        │                                          │
 * │            │   Bits 7:2                         │ Reserved                                 │
 * │            │   Bit 1: PDU Data Digest Flag        │ 1: DDGST field follows PDU Data and      │
 * │            │          (DDGSTF)                    │    contains a valid value                │
 * │            │                                      │ 0: DDGST field is not present            │
 * │            │   Bit 0: PDU Header Digest Flag      │ 1: HDGST field follows PDU Header and      │
 * │            │          (HDGSTF)                    │    contains a valid value                │
 * │            │                                      │ 0: HDGST field is not present            │
 * ├─────────────────────────────────────────────────────────────────────────────────────────────┤
 * │ 02         │ Header Length (HLEN)                 │ Fixed length of 72 bytes (48h)           │
 * ├─────────────────────────────────────────────────────────────────────────────────────────────┤
 * │ 03         │ PDU Data Offset (PDO)                │ Offset from byte 0 to the CCICD field    │
 * │            │                                      │ (value = N); must be a multiple of the   │
 * │            │                                      │ data alignment specified by the CPDA      │
 * │            │                                      │ field in the ICResp PDU                  │
 * ├─────────────────────────────────────────────────────────────────────────────────────────────┤
 * │ 07:04      │ PDU Length (PLEN)                    │ Total length of the PDU in bytes         │
 * │            │                                      │ (includes CH, PSH, HDGST, PAD, DATA, DDGST)│
 * ├─────────────────────────────────────────────────────────────────────────────────────────────┤
 * │ 71:08      │ PSH: NVMe-oF Command Capsule SQE     │ Command Capsule SQE (CCSQE) field        │
 * ├─────────────────────────────────────────────────────────────────────────────────────────────┤
 * │ 75:72      │ PDU Header Digest (HDGST)            │ When HDGSTF (bit0 of FLAGS) is 1: valid    │
 * │            │                                      │ header digest per section 3.3.1.1;         │
 * │            │                                      │ when HDGSTF is 0: this field is not        │
 * │            │                                      │ present                                  │
 * ├─────────────────────────────────────────────────────────────────────────────────────────────┤
 * │ N - 1:76   │ PAD                                  │ Padding bytes to achieve the alignment     │
 * │            │                                      │ specified by the CPDA field (if in-capsule  │
 * │            │                                      │ data is present)                         │
 * │ N - 1:72   │ PAD                                  │ Alternate range if HDGSTF is 0             │
 * ├─────────────────────────────────────────────────────────────────────────────────────────────┤
 * │ M - 1:N    │ DATA: NVMe-oF In-Capsule Data (CCICD)  │ Contains the in-capsule data, if any       │
 * ├─────────────────────────────────────────────────────────────────────────────────────────────┤
 * │ M + 3:M    │ PDU Data Digest (DDGST)              │ When DDGSTF (bit1 of FLAGS) is 1 and CCICD   │
 * │            │                                      │ field is present: contains data digest     │
 * │            │                                      │ (per section 3.3.1.1); otherwise, not         │
 * │            │                                      │ present                                  │
 * └─────────────────────────────────────────────────────────────────────────────────────────────┘
 */
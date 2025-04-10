#include <hls_stream.h>
#include <ap_int.h>
#include <stdint.h>


typedef struct _nvme_cqe_t
{
    // -- DW0 --
    uint32_t dw0; // Command Specific

    // -- DW1 --
    uint32_t dw1; // Reserved 

    // -- DW2 --
    uint16_t sq_head;                 
    uint16_t sq_identifier;               

    // -- DW3 --
    uint16_t cid;       // Command Identifier (CID)
    uint32_t phase_tag  :1;     // Phase Tag (P)
    uint32_t status_code:8;     // Status Code
    uint32_t sct        :3;     // Status Code Type (SCT)
    uint32_t reserved   :4;     // Reserved

} nvme_cqe_t;

typedef struct _nvme_sqe_t
{
    // common
    uint8_t  opc;         ///< opcode (0x01=Write, 0x02=Read, etc.)
    uint8_t  fuse : 2;    ///< fuse
    uint8_t  rsvd : 6;    ///< reserved
    uint16_t cid;         ///< Command ID
    uint32_t nsid;        ///< Namespace ID
    uint32_t cdw2;        ///< reserved or optional
    uint32_t cdw3;        ///< reserved or optional
    uint64_t mptr;        ///< metadata pointer
    uint64_t prp1;        ///< PRP entry 1
    uint64_t prp2;        ///< PRP entry 2
    uint32_t cdw10;

    // rsvd
    uint32_t cdw11;
    uint32_t cdw12;
    uint32_t cdw13;
    uint32_t cdw14;
    uint32_t cdw15;

} nvme_sqe_t;

struct request_packet {
    union {
        struct {
            uint16_t id;         ///< command ID
            uint32_t dev : 4;    ///< device
            uint32_t sqid : 4;   ///< SQ ID
            uint32_t flags : 4;  ///< flags
            uint32_t opc : 4;     ///< opcode (0x01=Write, 0x02=Read, etc.)
        };
        uint32_t raw;
    } dw0;

    uint32_t dw1; /// slba[31:0]
    
    union {
        struct {
            uint32_t nlb : 16;    ///< Number of Logical Blocks
            uint32_t slba : 16;   /// slba[47:32]
        };
        uint32_t raw;
    } dw2;

    uint64_t dw3; /// buffer address
};

struct completion_packet {
    union {
        struct {
            uint16_t id;         ///< command ID
            uint32_t reserved1 : 4;  
            uint32_t sct : 4;  ///< Status Code Type
            uint32_t sc : 8;     ///< Status Code
        };
        uint32_t raw;
    } dw0;
};
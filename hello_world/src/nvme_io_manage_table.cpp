
#include <hls_stream.h>
#include <ap_int.h>
#include <stdint.h>

// CID를 인덱스로 쓰는 접근 요청 구조체
struct TableCidReq {
    bool       we;    // write enable (true=write, false=read)
    ap_uint<16> cid;  // CID (16비트 가정)
    struct cmd_info   wdata; // write일 때 쓰고자 하는 데이터
};

// 테이블 read 결과 구조체
struct TableCidResp {
    struct cmd_info rdata;   // 읽어온 데이터
};


void mgmt_tables(
    hls::stream<TableCidReq>  &in_req,
    hls::stream<TableCidResp> &out_resp
)
{
    #pragma HLS PIPELINE II=1

    static cmd_info cmd_info_tbl[1024];
    #pragma HLS RESOURCE variable=cmd_info_tbl core=RAM_T2P_BRAM

    if (!in_req.empty() && !out_resp.full()) {
        TableCidReq  req  = in_req.read();
        TableCidResp resp;

        ap_uint<10> index = req.cid.range(9, 0);

        if (req.we) {
            struct cmd_info tmp = cmd_info_tbl[index];
            if (tmp.valid == 0) {
                cmd_info_tbl[index] = req.wdata;
            } else {
                if (req.wdata.valid == 1) {
                    cmd_info_tbl[index] = req.wdata;
                }
            }
            cmd_info_tbl[index] = req.wdata;
            resp.rdata = cmd_info();
        } else {
            resp.rdata = cmd_info_tbl[index];
        }

        out_resp.write(resp);
    }
}


/*
void moduleWrite(hls::stream<TableCidReq> &out_req)
{
#pragma HLS PIPELINE II=1
    // 예: CID=50, 데이터=0x1234
    TableCidReq req;
    req.we    = true;   // write
    req.cid   = 50;
    req.wdata = 0x1234;

    out_req.write(req);
}

void moduleRead(hls::stream<TableCidReq>  &out_req,
                hls::stream<TableCidResp> &in_resp)
{
#pragma HLS PIPELINE II=1
    // 1) Read 요청
    {
        TableCidReq req;
        req.we    = false;  // read
        req.cid   = 50;
        req.wdata = 0; // don't care
        out_req.write(req);
    }

    // 2) Read 응답 수신
    if (!in_resp.empty()) {
        TableCidResp resp = in_resp.read();
        uint32_t val = resp.rdata;
        // 여기서 val=0x1234가 읽힐 것으로 예상
        // (바로 앞선 moduleWrite가 같은 CID=50에 쓴 경우)
    }
}

*/

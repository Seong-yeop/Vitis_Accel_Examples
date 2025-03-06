//----------------------------------------------------------------------
// 간단한 testbench용 함수
//----------------------------------------------------------------------
#include "nvme_driver_top.hpp"
#define __gmp_const const
// 임의의 cmd_info를 만드는 헬퍼 함수 (예: cid만 채우고 나머지는 0)
static cmd_info make_cmd_info(ap_uint<16> cid) {
    cmd_info temp;
    temp.cid   = cid;
    temp.valid = false;  // 등록 시 valid는 내부에서 true로 설정될 것
    // 필요한 경우 나머지 필드를 초기화
    return temp;
}

//----------------------------------------------------------------------
// 테스트벤치 메인 (CSimulation 용)
//----------------------------------------------------------------------
int main() {
    // HLS 스트림 객체
    hls::stream<mgmt_table_req>  submit_in_req("submit_in_req");
    hls::stream<mgmt_table_resp> submit_out_resp("submit_out_resp");
    hls::stream<mgmt_table_req>  cpl_in_req("cpl_in_req");
    hls::stream<mgmt_table_resp> cpl_out_resp("cpl_out_resp");

    //--------------------------------------------------------------------------
    // 1) 간단한 테스트: Submit 경로에 요청을 넣어 등록
    //--------------------------------------------------------------------------
    // 예: CID = 0x0010, 0x0011, 0x0012 ... 로 3개를 등록 요청
    for (int i = 0; i < 3; i++) {
        mgmt_table_req req;
        req.cid   = 0;  // 등록 시 cid는 크게 의미 없고, wdata.cid가 중요
        req.wdata = make_cmd_info((ap_uint<16>)(0x10 + i));

        // 스트림에 write
        submit_in_req.write(req);

        // mgmt_table 모듈 호출 (간단히 수 회 호출; 실제론 더 많이 호출 가능)
        // 한 번 호출할 때마다 내부에서 한 사이클 파이프라인이 진행된다고 보면 됨.
        mgmt_table(submit_in_req, submit_out_resp,
                   cpl_in_req, cpl_out_resp);
    }

    //--------------------------------------------------------------------------
    // 2) 처리 결과 읽기 (submit_out_resp)
    //    - 위에서 3개의 요청을 넣었으니, 3개의 응답이 나올 것으로 기대
    //--------------------------------------------------------------------------
    // mgmt_table는 파이프라인이므로, 이미 호출된 횟수만큼 데이터가 나왔을 수도,
    // 더 호출해야 나올 수도 있습니다. 안전하게 몇 번 더 호출해 봄.
    for (int i = 0; i < 10; i++) {
        mgmt_table(submit_in_req, submit_out_resp,
                   cpl_in_req, cpl_out_resp);
    }

    // 이제 스트림에 데이터가 있다면 읽어보기
    while (!submit_out_resp.empty()) {
        mgmt_table_resp resp;
        resp = submit_out_resp.read();

        std::cout << "[SubmitOutResp] status = " << resp.status
                  << ", rdata.cid = " << resp.rdata.cid
                  << ", rdata.valid = " << resp.rdata.valid
                  << std::endl;
    }

    //--------------------------------------------------------------------------
    // 3) Completion 경로 테스트: 특정 CID를 clear 해 달라고 요청
    //--------------------------------------------------------------------------
    {
        mgmt_table_req req;
        req.cid   = 0x10;     // 등록했던 0x10을 clear 해보자
        req.wdata = cmd_info();  // cpl_in_req에선 wdata는 의미 없음

        cpl_in_req.write(req);

        // 모듈 여러 번 호출
        for (int i = 0; i < 5; i++) {
            mgmt_table(submit_in_req, submit_out_resp,
                       cpl_in_req, cpl_out_resp);
        }

        // cpl_out_resp 읽어보기 (1개 나올 것으로 기대)
        while (!cpl_out_resp.empty()) {
            mgmt_table_resp resp;
            resp = cpl_out_resp.read();
            std::cout << "[CplOutResp] status = " << resp.status
                      << ", rdata.cid = " << resp.rdata.cid
                      << ", rdata.valid = " << resp.rdata.valid
                      << std::endl;
        }
    }

    //--------------------------------------------------------------------------
    // 4) 추가로 여러 번 더 호출하여 남은 데이터 유무 확인
    //--------------------------------------------------------------------------
    for (int i = 0; i < 10; i++) {
        mgmt_table(submit_in_req, submit_out_resp,
                   cpl_in_req, cpl_out_resp);
    }

    // 남아 있는 결과가 있다면 마저 출력
    while (!submit_out_resp.empty()) {
        mgmt_table_resp resp;
        resp = submit_out_resp.read();
        std::cout << "[SubmitOutResp - leftover] status = " << resp.status
                  << ", rdata.cid = " << resp.rdata.cid
                  << ", rdata.valid = " << resp.rdata.valid
                  << std::endl;
    }
    while (!cpl_out_resp.empty()) {
        mgmt_table_resp resp;
        resp = cpl_out_resp.read();
        std::cout << "[CplOutResp - leftover] status = " << resp.status
                  << ", rdata.cid = " << resp.rdata.cid
                  << ", rdata.valid = " << resp.rdata.valid
                  << std::endl;
    }

    std::cout << "Testbench simulation done." << std::endl;
    return 0;
}

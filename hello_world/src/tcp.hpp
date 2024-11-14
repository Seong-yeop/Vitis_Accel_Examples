#ifndef TCP_HPP
#define TCP_HPP
#include <stdint.h>
#include <ap_int.h>

template <int D>
struct net_axis
{
	ap_uint<D>		data;
	ap_uint<D/8>	keep;
	ap_uint<1>		last;
	net_axis() {}
	net_axis(ap_uint<D> data, ap_uint<D/8> keep, ap_uint<1> last)
		:data(data), keep(keep), last(last) {}
};

template <int D>
struct rx_packet {
    ap_uint<16> session_id;
    ap_uint<16> length;
    net_axis<D> data;
};

struct rx_packet_host {
    uint16_t session_id;
    uint16_t length;
    void *data;
};

template <int D>
struct tx_packet {
    ap_uint<16> session_id;
    ap_uint<16> length;
    net_axis<D> data;
};

enum class rx_state { IDLE, WRITE_PACKET, UPDATE_TAIL };


#endif // TCP_HPP
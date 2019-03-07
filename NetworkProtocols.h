#ifndef RTP_PARSER_NETWORKPROTOCOLS_H
#define RTP_PARSER_NETWORKPROTOCOLS_H

#include <cstdint>

typedef struct {
    uint8_t mac_src[6];
    uint8_t mac_dst[6]; /* ethernet MACs */
    uint16_t type;
} eth_header;

typedef struct {
    int family;
} loop_header;

typedef struct {
    uint8_t version;
    uint8_t header_size;
    uint8_t protocol;
    uint8_t src[4], dst[4]; /* ipv4 addrs */
} ip_header;

typedef struct {
    uint16_t src, dst; /* ports */
    uint16_t size, checksum;
} udp_header;

typedef struct {
    int version;
    int type;
    int pad, ext, cc, mark;
    int seq, time;
    int ssrc;
    int *csrc;
    int header_size;
    int payload_size;
} rtp_header;

typedef struct {
    bool mark;
    int payload_type;
    int seq;
    int ssrc;
    uint8_t src[4], dst[4];
    uint16_t srcPort, dstPort;
} rtp_info;

class NetworkProtocols {
public:
    static int ETH_HEADER_LEN;
    static int LOOP_HEADER_LEN;
    static int IP_HEADER_MIN_LEN;
    static int UDP_HEADER_LEN;
    static int RTP_HEADER_MIN_LEN;
    static bool eth_parser(const uint8_t *packet, int size, eth_header *eth);
    static bool loop_parser(const uint8_t *packet, int size, loop_header *loop);
    static bool ip_parser(const uint8_t *packet, int size, ip_header *ip);
    static bool udp_parser(const uint8_t *packet, int size, udp_header *udp);
    static bool rtp_parser(const uint8_t *packet, int size, rtp_header *rtp);
private:
    static uint16_t read_big_endian_16(const uint8_t *data);
    static uint32_t read_native_endian_32(const uint8_t *data);
};


#endif //RTP_PARSER_NETWORKPROTOCOLS_H

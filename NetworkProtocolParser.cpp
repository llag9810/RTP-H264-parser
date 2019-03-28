//
// Created by Yifan Zhu on 2019-03-06.
//

#include <iostream>
#include "NetworkProtocolParser.h"

int NetworkProtocolParser::ETH_HEADER_LEN = 14;
int NetworkProtocolParser::LOOP_HEADER_LEN = 4;
int NetworkProtocolParser::IP_HEADER_MIN_LEN = 20;
int NetworkProtocolParser::UDP_HEADER_LEN = 8;
int NetworkProtocolParser::RTP_HEADER_MIN_LEN = 12;
int NetworkProtocolParser::TCP_HEADER_MIN_LEN = 20;

uint16_t NetworkProtocolParser::read_big_endian_16(const uint8_t *data) {
    return data[0] << 8 | data[1];
}


uint32_t NetworkProtocolParser::read_native_endian_32(const uint8_t *data) {
    uint32_t d = 0;
    memcpy(&d, data, 4);
    return d;
}

bool NetworkProtocolParser::eth_parser(const uint8_t *packet, int size, eth_header *eth) {
    if (!packet || !eth) {
//        std::cout << "packet or eth is nullptr!" << std::endl;
        return false;
    }

    if (size < ETH_HEADER_LEN) {
        // std::cout << "Invalid eth packet!" << std::endl;
        return false;
    }

    memcpy(eth->mac_dst, packet, 6);
    memcpy(eth->mac_dst, packet + 6, 6);
    eth->type = read_big_endian_16(packet + 12);
    return true;
}

bool NetworkProtocolParser::loop_parser(const uint8_t *packet, int size, loop_header *loop) {
    if (!packet || !loop) {
        return false;
    }
    if (size < LOOP_HEADER_LEN) {
//        fprintf(stdout, "Packet too short for loopback\n");
        return false;
    }
    /* protocol is in host byte order on osx. may be big endian on openbsd? */
    loop->family = read_native_endian_32(packet);
    return true;
}

bool NetworkProtocolParser::ip_parser(const uint8_t *packet, int size, ip_header *ip) {
    if (!packet || !ip) {
        return false;
    }
    if (size < IP_HEADER_MIN_LEN) {
//        fprintf(stdout, "Packet too short for ip\n");
        return false;
    }
    ip->version = static_cast<uint8_t>((packet[0] & 0xF0) >> 4);
    if (ip->version != 4) {
//        fprintf(stdout, "ipv6 not supported yet.\n");
        return false;
    }
    ip->header_size = static_cast<uint8_t>((packet[0] & 0x0F) * 4);
    ip->protocol = packet[9];
    memcpy(ip->src, packet + 12, 4);
    memcpy(ip->dst, packet + 16, 4);
    return true;
}


bool NetworkProtocolParser::tcp_parser(const uint8_t *packet, int size, tcp_header *tcp) {
    if (!packet || !tcp) {
        return false;
    }
    if (size < TCP_HEADER_MIN_LEN) {
        return false;
    }

    tcp->src = read_big_endian_16(packet);
    tcp->dst = read_big_endian_16(packet + 2);
    tcp->header_size = (packet[12] & 0xF0) >> 2;

    return size - tcp->header_size >= 0;

}


bool NetworkProtocolParser::udp_parser(const uint8_t *packet, int size, udp_header *udp) {
    if (!packet || !udp) {
        return false;
    }
    if (size < UDP_HEADER_LEN) {
        // fprintf(stdout, "Packet too short for udp\n");
        return false;
    }
    udp->src = read_big_endian_16(packet);
    udp->dst = read_big_endian_16(packet + 2);
    udp->size = read_big_endian_16(packet + 4);
    udp->checksum = read_big_endian_16(packet + 6);
    return true;
}

bool NetworkProtocolParser::rtp_parser(const uint8_t *packet, int size, rtp_header *rtp) {
    if (!packet || !rtp) {
        return false;
    }

    if (size < RTP_HEADER_MIN_LEN) {
//        fprintf(stdout, "Packet too short for rtp\n");
        return false;
    }
    rtp->version = (packet[0] >> 6) & 3;
    rtp->pad = (packet[0] >> 5) & 1;
    rtp->ext = (packet[0] >> 4) & 1;
    rtp->cc = packet[0] & 7;
    rtp->header_size = 12 + 4 * rtp->cc;
    if (rtp->ext == 1) {
        uint16_t ext_length;
        rtp->header_size += 4;
        ext_length = read_big_endian_16(packet + rtp->header_size - 2);
        rtp->header_size += ext_length * 4;
    }
    rtp->payload_size = size - rtp->header_size;

    rtp->mark = (packet[1] >> 7) & 1;
    rtp->type = (packet[1]) & 127;

    unsigned short s = 0;
    memcpy(&s, (packet + 2), 2);
    rtp->seq = ntohs(s);

    unsigned int t = 0;
    memcpy(&t, packet + 4, 4);
    rtp->time = ntohl(t);

    unsigned int ssrc = 0;
    memcpy(&ssrc, packet + 8, 4);
    rtp->ssrc = ntohl(ssrc);
    rtp->csrc = nullptr;
    if (size < rtp->header_size) {
        fprintf(stdout, "Packet too short for RTP header\n");
        return false;
    }

    return true;
}




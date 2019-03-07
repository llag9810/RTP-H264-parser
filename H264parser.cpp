//
// Created by Yifan Zhu on 2019-03-07.
//

#include "H264parser.h"

H264parser *H264parser::instance;
H264parser *H264parser::get_instance() {
    if (instance == nullptr) {
        instance = new H264parser;
    }
    return instance;
}

void H264parser::release() {
    delete instance;
    instance = nullptr;
}

void H264parser::parse(const rtp_info &rtpInfo, const unsigned char *data, int size) {
    const int type_fu_a = 28;
    const int type_stap_a = 24;
    nalu_header nalu;
    nalu.f = static_cast<uint8_t>((data[0] >> 7) & 0x1);
    nalu.nri = static_cast<uint8_t>((data[0] >> 5) & 0x3);
    nalu.type = static_cast<uint8_t>(data[0] & 0x1f);
//    fprintf(stdout, "seq = %d, nalu: F = %d, NRI = %d, type = %d\n", rtpInfo.seq, nalu.f, nalu.nri, nalu.type);
    if (nalu.type == type_fu_a) {
        fu_a_header fu_a;
        fu_a.s = static_cast<uint8_t>((data[1] >> 7) & 0x1);
        fu_a.e = static_cast<uint8_t>((data[1] >> 6) & 0x1);
        fu_a.r = static_cast<uint8_t>((data[1] >> 5) & 0x1);
        fu_a.type = static_cast<uint8_t>((data[1]) & 0x1f);
        fprintf(stdout, "fu_a: s = %d, e = %d, type = %d\n", fu_a.s, fu_a.e, fu_a.type);
    } else {
        fprintf(stdout, "seq = %d, nalu: F = %d, NRI = %d, type = %d\n", rtpInfo.seq, nalu.f, nalu.nri, nalu.type);
    }
}


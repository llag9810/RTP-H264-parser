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

void H264parser::parse(const rtp_info &rtpInfo, const unsigned char *data, size_t size) {
    const int type_fu_a = 28;
    const int type_stap_a = 24;
    nalu_header nalu;
    nalu.f = static_cast<uint8_t>((data[0] >> 7) & 0x1);
    nalu.nri = static_cast<uint8_t>((data[0] >> 5) & 0x3);
    nalu.type = static_cast<uint8_t>(data[0] & 0x1f);

    unsigned char identifier[4] = {0, 0, 0, 1};  // H.264 identifier.
    if (nalu.type == type_fu_a) {                  // fu_a
        fu_a_header fu_a;
        fu_a.s = static_cast<uint8_t>((data[1] >> 7) & 0x1);
        fu_a.e = static_cast<uint8_t>((data[1] >> 6) & 0x1);
        fu_a.r = static_cast<uint8_t>((data[1] >> 5) & 0x1);
        fu_a.type = static_cast<uint8_t>((data[1]) & 0x1f);
        // fprintf(stdout, "fu_a: s = %d, e = %d, type = %d\n", fu_a.s, fu_a.e, fu_a.type);
        if (fu_a.s == 1) {
            assert(cache.size() == 0);
            cache.add(identifier, 4);
            uint8_t reassembled_nalu = 0;
            reassembled_nalu |= (nalu.f << 7);
            reassembled_nalu |= (nalu.nri << 5);
            reassembled_nalu |= (fu_a.type);
            cache.add(&reassembled_nalu, 1);
        }
        data += 2;
        size -= 2;
        cache.add(data, size);
        if (fu_a.e == 1) {
            FILE *file = fopen("test.h264", "ab");
            fwrite(cache.get(), cache.size(), 1, file);
            fclose(file);
            cache.reset();
        }
    } else if (nalu.type == type_stap_a) {
        fprintf(stdout, "not supported yet\n");
    } else {   // NALU in a single packet
        cache.add(identifier, 4);
        cache.add(data, size);
        FILE *file = fopen("test.h264", "ab");
        fwrite(cache.get(), cache.size(), 1, file);
        fclose(file);
        cache.reset();
    }
}




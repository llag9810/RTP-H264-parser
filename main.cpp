#include <iostream>
#include "pcap.h"
#include "PcapManager.h"
#include "H264parser.h"

const int H_264 = 96;
const int AAC = 97;

int main(int argc, char **argv) {
    auto pcapManager = PcapManager::get_instance();
    auto h264Parser = H264parser::get_instance();
    auto parsefunc = [=](const rtp_info &info, const u_char *data, int size) {
        switch (info.payload_type) {
            case H_264:
                h264Parser->parse(info, data, size);
                break;
            case AAC:
                // TODO: parse AAC audio.
                break;
            default:
                break;
        }

//        fprintf(stdout, "RTP ssrc = %d, seq = %d, type = %d, mark = %d\n", info.ssrc, info.seq, info.payload_type,info.mark);
    };
    auto endfunc = [](){};
    pcapManager->init("lo0", parsefunc, endfunc);
    pcapManager->read_loop();
    return 0;
}
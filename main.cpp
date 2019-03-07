#include <iostream>
#include "pcap.h"
#include "PcapManager.h"

int main(int argc, char **argv) {
    PcapManager *pcapManager = PcapManager::GetInstance();
    auto parsefunc = [](const rtp_info &info, const u_char *data, int size){
        fprintf(stdout, "RTP ssrc = %d, seq = %d, type = %d, mark = %d\n", info.ssrc, info.seq, info.payload_type,info.mark);
    };
    auto endfunc = [](){};
    pcapManager->Init("lo0", parsefunc, endfunc);
    pcapManager->ReadLoop();
    return 0;
}
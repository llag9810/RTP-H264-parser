#ifndef RTP_PARSER_PCAPMANAGER_H
#define RTP_PARSER_PCAPMANAGER_H

#include <string>
#include <functional>
#include "pcap.h"
#include "NetworkProtocols.h"

typedef std::function<void(const rtp_info &rtp_info, const unsigned char *data, int payload_size)> ParseFunc;
typedef std::function<void()> LoopEndFunc;

class PcapManager {
public:
    static PcapManager *GetInstance();

    static void Release();

    int Init(const std::string &dev, const ParseFunc &Parsef, const LoopEndFunc &LoopEndf);

    void ReadLoop();

private:
    static PcapManager *instance;
    pcap_t *pcap;
    int linkType;
    ParseFunc funcParse;
    LoopEndFunc funcLoopEnd;

    static void GetPacket(unsigned char *args, const struct pcap_pkthdr *header, const u_char *data);
};


#endif //RTP_PARSER_PCAPMANAGER_H

#ifndef RTP_PARSER_PCAPMANAGER_H
#define RTP_PARSER_PCAPMANAGER_H

#include <string>
#include <functional>
#include "pcap.h"
#include "NetworkProtocolParser.h"

typedef std::function<void(const rtp_info &rtp_info, const unsigned char *data, int payload_size)> ParseFunc;
typedef std::function<void()> LoopEndFunc;

class PcapManager {
public:
    static PcapManager *get_instance();

    static void release();

    int init(const std::string &dev, const ParseFunc &Parsef, const LoopEndFunc &LoopEndf);

    void read_loop();

private:
    PcapManager() : pcap(nullptr), linkType(-1) {}
    ~PcapManager() { pcap_close(pcap); }
    static PcapManager *instance;
    pcap_t *pcap;
    int linkType;
    ParseFunc funcParse;
    LoopEndFunc funcLoopEnd;

    static void get_packet(unsigned char *args, const struct pcap_pkthdr *header, const u_char *data);
};


#endif //RTP_PARSER_PCAPMANAGER_H

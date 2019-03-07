#include <iostream>
#include <sys/socket.h>
#include "PcapManager.h"
#include "NetworkProtocolParser.h"

PcapManager *PcapManager::instance;

PcapManager *PcapManager::get_instance() {
    if (instance == nullptr) {
        instance = new PcapManager;
    }
    return instance;
}

int PcapManager::init(const std::string &dev, const ParseFunc &Parsef, const LoopEndFunc &LoopEndf) {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap = pcap_open_live(dev.c_str(), BUFSIZ, 1, 2000, errbuf);
    if (pcap == nullptr) {
        std::cout << "open pcap failed,error info " << errbuf << std::endl;
        return -1;
    }
    linkType = pcap_datalink(pcap);
    funcParse = Parsef;
    funcLoopEnd = LoopEndf;
    return 0;
}

void PcapManager::release() {
    if (instance != nullptr) {
        delete instance;
        instance = nullptr;
    }

}

void PcapManager::read_loop() {
    pcap_loop(pcap, 0, PcapManager::get_packet, (u_char *) this);
    funcLoopEnd();
}

void PcapManager::get_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *data) {
    auto manager = reinterpret_cast<PcapManager *>(args);
    const unsigned char *packet = data;
    int size = header->caplen;

    switch (manager->linkType) {
        case DLT_EN10MB:
            eth_header eth;
            if (!NetworkProtocolParser::eth_parser(packet, size, &eth)) {
                return;
            }

//            fprintf(stdout, "  eth 0x%04x", eth.type);
//            fprintf(stdout, " %02x:%02x:%02x:%02x:%02x:%02x ->",
//                    eth.mac_src[0], eth.mac_src[1], eth.mac_src[2],
//                    eth.mac_src[3], eth.mac_src[4], eth.mac_src[5]);
//
//            fprintf(stdout, " %02x:%02x:%02x:%02x:%02x:%02x\n",
//                    eth.mac_dst[0], eth.mac_dst[1], eth.mac_dst[2],
//                    eth.mac_dst[3], eth.mac_dst[4], eth.mac_dst[5]);

            if (eth.type != 0x0800) { // not IPv4
//                fprintf(stdout, "skipping packet: not IPv4\n");
                return;
            }
            packet += NetworkProtocolParser::ETH_HEADER_LEN;
            size -= NetworkProtocolParser::ETH_HEADER_LEN;
            break;
        case DLT_NULL:
            loop_header loop;
            if (!NetworkProtocolParser::loop_parser(data, header->caplen, &loop)) {
                return;
            }
            if (loop.family != PF_INET) {
//                fprintf(stdout, "skipping packet: not INET\n");
                return;
            }
//            fprintf(stdout, "%d\n", loop.family);
            packet += NetworkProtocolParser::LOOP_HEADER_LEN;
            size -= NetworkProtocolParser::LOOP_HEADER_LEN;
            break;
        default:
//            fprintf(stdout, "  skipping packet: unrecognized linktype %d\n", manager->linkType);
            return;
    }
    ip_header ip;
    if (!NetworkProtocolParser::ip_parser(packet, size, &ip)) {
//        fprintf(stdout, "skipping packet: not IP\n");
        return;
    }

    if (ip.protocol != 0x11) {
//        fprintf(stdout, "skipping packet: not UDP\n");
        return;
    }

//    fprintf(stdout, "IP packet: %d.%d.%d.%d -> %d.%d.%d.%d\n", \
//    ip.src[0], ip.src[1], ip.src[2], ip.src[3], \
//    ip.dst[0], ip.dst[1], ip.dst[2], ip.dst[3]);
    packet += ip.header_size;
    size -= ip.header_size;

    udp_header udp;
    if (!NetworkProtocolParser::udp_parser(packet, size, &udp)) {
        //        fprintf(stdout, "skipping packet: parse UDP failed.\n");
        return;
    }

//    fprintf(stdout, "UDP packet: %d.%d.%d.%d:%d -> %d.%d.%d.%d:%d\n", \
//    ip.src[0], ip.src[1], ip.src[2], ip.src[3], udp.src, \
//    ip.dst[0], ip.dst[1], ip.dst[2], ip.dst[3], udp.dst);

    packet += NetworkProtocolParser::UDP_HEADER_LEN;
    size -= NetworkProtocolParser::UDP_HEADER_LEN;

    rtp_header rtp;
    if (!NetworkProtocolParser::rtp_parser(packet, size, &rtp)) {
        return;
    }
//    fprintf(stdout, "RTP ssrc = %u, seq = %d, payload type = %d, mark = %d\n", rtp.ssrc, rtp.seq, rtp.type, rtp.mark);
    packet += rtp.header_size;
    size -= rtp.header_size;

    rtp_info info;
    info.mark = rtp.mark != 0;
    info.payload_type = rtp.type;
    info.ssrc = rtp.ssrc;
    info.seq = rtp.seq;
    info.srcPort = udp.src;
    info.dstPort = udp.dst;
    memcpy(info.src, ip.src, 4);
    memcpy(info.dst, ip.dst, 4);

    manager->funcParse(info, packet, size);
}

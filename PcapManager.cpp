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
    pcap = pcap_open_live(dev.c_str(), 2048, 1, 2000, errbuf);
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

            if (eth.type != 0x0800) { // not IPv4
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
                return;
            }
            packet += NetworkProtocolParser::LOOP_HEADER_LEN;
            size -= NetworkProtocolParser::LOOP_HEADER_LEN;
            break;
        default:
            return;
    }

    ip_header ip;
    if (!NetworkProtocolParser::ip_parser(packet, size, &ip)) {
        return;
    }

    packet += ip.header_size;
    size -= ip.header_size;

    switch (ip.protocol) {
        case 0x06: { // tcp
            tcp_header tcp;
            if (!NetworkProtocolParser::tcp_parser(packet, size, &tcp)) {
                return;
            }
            packet += tcp.header_size;
            size -= tcp.header_size;
            AddressPair address = AddressPair(ip.src, tcp.src, ip.dst, tcp.dst);
            if (!get_instance()->rtspProcessor.process(packet, size, address)) {
                return;
            }
            break;
        }
        case 0x11: {  // udp
            udp_header udp;
            if (!NetworkProtocolParser::udp_parser(packet, size, &udp)) {
                return;
            }

            packet += NetworkProtocolParser::UDP_HEADER_LEN;
            size -= NetworkProtocolParser::UDP_HEADER_LEN;

            rtp_header rtp;
            if (!NetworkProtocolParser::rtp_parser(packet, size, &rtp)) {
                return;
            }
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
            break;
        }
        default:
            return;
    }
}

//
// Created by Yifan Zhu on 2019-03-26.
//

#ifndef RTP_PARSER_ADDRESSPAIR_H
#define RTP_PARSER_ADDRESSPAIR_H

#include <cstdint>
#include <cstring>

class AddressPair {
public:
    uint8_t srcIP[4];
    uint16_t srcPort;
    uint8_t dstIP[4];
    uint16_t dstPort;

    AddressPair(uint8_t srcIP[4], uint16_t srcPort, uint8_t dstIP[4], uint16_t dstPort) {
        memcpy(this->srcIP, srcIP, 4);
        memcpy(this->dstIP, dstIP, 4);
        this->srcPort = srcPort;
        this->dstPort = dstPort;
    }

    bool operator==(const AddressPair &rhs) const {
        return (srcIP[0] == rhs.srcIP[0] && srcIP[1] == rhs.srcIP[1]
        && srcIP[2] == rhs.srcIP[2] && srcIP[3] == rhs.srcIP[3])
        && srcPort == rhs.srcPort && dstPort == rhs.dstPort;
    }
};

struct KeyHash {
    std::size_t operator()(const AddressPair& k) const {
        std::size_t value = 2166136261;
        std::hash<int> hash;
        for (int i = 0; i < 4; i++) {
            value ^= hash(k.srcIP[i]);
            value ^= hash(k.dstIP[i]);
        }
        value ^= hash(k.srcPort);
        value ^= hash(k.dstPort);
        return value;
    }
};

#endif //RTP_PARSER_ADDRESSPAIR_H

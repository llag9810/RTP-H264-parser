//
// Created by Yifan Zhu on 2019-03-27.
//

#ifndef RTP_PARSER_RTSPPARSER_H
#define RTP_PARSER_RTSPPARSER_H

#include <regex>
#include "RtspConnectionManager.h"
#include "NetworkProtocolParser.h"

class RtspProcessor {
public:
    bool process(const uint8_t *packet, size_t size, AddressPair &pair);
    RtspProcessor() {
        _valid_rtsp_keyword = std::regex("RTSP.*OK");
    };
private:
    RtspConnectionManager _connectionManager;
    std::regex _valid_rtsp_keyword;
};


#endif //RTP_PARSER_RTSPPARSER_H

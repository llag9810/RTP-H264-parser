//
// Created by Yifan Zhu on 2019-03-29.
//

#ifndef RTP_PARSER_AACPARSER_H
#define RTP_PARSER_AACPARSER_H


#include <cstddef>
#include "FrameCache.h"
#include "NetworkProtocolParser.h"

class AACParser {
public:
    static AACParser *get_instance();
    static void release();
    void parse(const rtp_info &info, const uint8_t *data, size_t size);
private:
    AACParser() = default;
    static AACParser *instance;
    FrameCache cache;
};


#endif //RTP_PARSER_AACPARSER_H

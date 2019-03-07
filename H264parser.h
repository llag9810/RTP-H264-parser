#ifndef RTP_PARSER_H264PARSER_H
#define RTP_PARSER_H264PARSER_H

#include <iostream>
#include "NetworkProtocolParser.h"

typedef struct {
    uint8_t f : 1;
    uint8_t nri : 2;
    uint8_t type : 5;
} nalu_header;

typedef struct {
    uint8_t s : 1;
    uint8_t e : 1;
    uint8_t r : 1;
    uint8_t type : 5;
} fu_a_header;

class H264parser {
public:
    static H264parser *get_instance();
    static void release();
    void parse(const rtp_info& rtpInfo, const unsigned char *data, int size);
private:
    H264parser() = default;
    static H264parser *instance;
};


#endif //RTP_PARSER_H264PARSER_H

//
// Created by Yifan Zhu on 2019-03-26.
//

#ifndef RTP_PARSER_RTSPCONNECTIONMANAGER_H
#define RTP_PARSER_RTSPCONNECTIONMANAGER_H

#include <unordered_map>
#include "AddressPair.h"
enum RtspStatus { OPTION, DESCRIBE, SETUP, PLAY, TEARDOWN };

class RtspConnectionManager {
public:
    bool contains(AddressPair &pair);
    void set(AddressPair &pair, RtspStatus status);
    RtspStatus& get(AddressPair& key);
    void remove(AddressPair& key);
private:
    std::unordered_map<AddressPair, RtspStatus, KeyHash> _status;
};


#endif //RTP_PARSER_RTSPCONNECTIONMANAGER_H

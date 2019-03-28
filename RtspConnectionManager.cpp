//
// Created by Yifan Zhu on 2019-03-26.
//

#include "RtspConnectionManager.h"

bool RtspConnectionManager::contains(AddressPair &pair) {
    return _status.find(pair) != _status.end();
}

void RtspConnectionManager::set(AddressPair &pair, RtspStatus status) {
    _status[pair] = status;
}

RtspStatus &RtspConnectionManager::get(AddressPair &key) {
    return _status.at(key);
}

void RtspConnectionManager::remove(AddressPair &key) {
    _status.erase(key);
}

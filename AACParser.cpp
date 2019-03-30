//
// Created by Yifan Zhu on 2019-03-29.
//

#include <cstdio>
#include "AACParser.h"

AACParser *AACParser::instance;

AACParser *AACParser::get_instance() {
    if (instance == nullptr) {
        instance = new AACParser;
    }
    return instance;
}

void AACParser::release() {
    delete instance;
    instance = nullptr;
}

void AACParser::parse(const rtp_info &info, const uint8_t *data, size_t size) {
    uint8_t adtsHeader[7] = {0};


    adtsHeader[0] = 0xFF;
    adtsHeader[1] = 0xF1;
    int profile = 2;
    int freqIdx = 3;

    int chanCfg = 2;
    int packetLen = size + 7 - 4;


    adtsHeader[2] = ((profile - 1) << 6) + (freqIdx << 2) + (chanCfg >> 2);
    adtsHeader[3] = ((chanCfg & 3) << 6) + (packetLen >> 11);
    adtsHeader[4] = (packetLen & 0x7ff) >> 3;
    adtsHeader[5] = ((packetLen & 0x7) << 5) | 0x1f;
    adtsHeader[6] = 0xFC;
    FILE *file = fopen("test.aac", "ab");
    fwrite(&adtsHeader, sizeof(adtsHeader), 1, file);
    fwrite(data + 4, size - 4, 1, file);
    fclose(file);
    cache.reset();
}



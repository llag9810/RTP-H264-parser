//
// Created by Yifan Zhu on 2019-03-27.
//

#include "RtspProcessor.h"
#include "FrameCache.h"
#include <regex>
#include <iostream>

std::string get_parameter_set(const std::string &body) {
    std::string keyword("sprop-parameter-set");
    auto start_pos = body.find(keyword);
    if (start_pos == std::string::npos) {
        return std::string();
    }
    start_pos += keyword.size() + 2;
    auto end_pos = body.find("\r\n", start_pos);
    return body.substr(start_pos, end_pos - start_pos);
}

std::string get_aac_config(const std::string &body) {
    std::string keyword("sizelength");
    auto start_pos = body.find(keyword);
    if (start_pos == std::string::npos) {
        return std::string();
    }
    auto end_pos = body.find("\r\n", start_pos + 1);
    return body.substr(start_pos, end_pos - start_pos);
}


const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";


static inline bool is_base64(unsigned char c) {
    return (isalnum(c) || (c == '+') || (c == '/'));
}

std::vector<uint8_t> base64_decode(std::string const &encoded_string) {
    int in_len = encoded_string.size();
    int i = 0;
    int j = 0;
    int in_ = 0;
    uint8_t char_array_4[4], char_array_3[3];
    std::vector<uint8_t> ret;

    while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
        char_array_4[i++] = encoded_string[in_];
        in_++;
        if (i == 4) {
            for (i = 0; i < 4; i++) {
                char_array_4[i] = base64_chars.find(char_array_4[i]);
            }
            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (i = 0; i < 3; i++) {
                ret.push_back(char_array_3[i]);
            }
            i = 0;
        }
    }

    if (i) {
        for (j = i; j < 4; j++)
            char_array_4[j] = 0;

        for (j = 0; j < 4; j++)
            char_array_4[j] = base64_chars.find(char_array_4[j]);

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (j = 0; (j < i - 1); j++) ret.push_back(char_array_3[j]);
    }

    return ret;
}



void decode_h264_sps_and_pps(std::string &encoded_string) {
    auto comma = encoded_string.find(',');
    std::string encoded_sps = encoded_string.substr(0, comma);
    std::string encoded_pps = encoded_string.substr(comma + 1);
    auto sps = base64_decode(encoded_sps);
    auto pps = base64_decode(encoded_pps);
    uint8_t identifier[4] = { 0, 0, 0, 1 };
    std::vector<uint8_t> cache;
    for (int i = 0; i < 4; i++) cache.push_back(identifier[i]);
    cache.insert(cache.end(), sps.begin(), sps.end());
    for (int i = 0; i < 4; i++) cache.push_back(identifier[i]);
    cache.insert(cache.end(), pps.begin(), pps.end());
    FILE *file = fopen("test.h264", "ab");
    fwrite(cache.data(), cache.size(), 1, file);
    fclose(file);
}

void decode_aac_info(std::string &aac_info) {

}


bool RtspProcessor::process(const uint8_t *packet, size_t size, AddressPair &pair) {
    if (!packet) {
        return false;
    }
    const char *data = reinterpret_cast<const char *>(packet);
    std::string body(data, size);
    if (!std::regex_search(body, _valid_rtsp_keyword)) {
        return false;
    }
    auto encoded_sps_pps = get_parameter_set(body);
    auto encoded_aac_info = get_aac_config(body);
    if (encoded_sps_pps != "") {
        decode_h264_sps_and_pps(encoded_sps_pps);
    }

    if (encoded_aac_info != "") {
        decode_aac_info(encoded_aac_info);
    }
    return true;
}

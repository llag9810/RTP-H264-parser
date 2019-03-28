//
// Created by Yifan Zhu on 2019-03-08.
//

#ifndef RTP_PARSER_FRAMECACHE_H
#define RTP_PARSER_FRAMECACHE_H


class FrameCache {
public:
    FrameCache();
    ~FrameCache();
    void reset();
    void add(const unsigned char *data, size_t size);
    unsigned char * get();
    unsigned int size();
private:
    const unsigned int initial_capacity = 1024 * 1024;
    uint8_t *_data;
    unsigned int _size;
    unsigned int _capacity;
};


#endif //RTP_PARSER_FRAMECACHE_H

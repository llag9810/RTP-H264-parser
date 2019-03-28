#include <cstdlib>
#include <cstring>
#include "FrameCache.h"

FrameCache::FrameCache() : _size(0), _capacity(initial_capacity) {
    _data = static_cast<unsigned char *>(malloc(initial_capacity));
    memset(_data, 0, initial_capacity);
}

FrameCache::~FrameCache() {
    free(_data);
    _data = nullptr;
}

void FrameCache::reset() {
    if (_data) {
        memset(_data, 0, _size);
        _size = 0;
    }
}

void FrameCache::add(const unsigned char *data, size_t size) {
    if (_size + size >= _capacity) {
        _data = static_cast<unsigned char *>(realloc(_data, 2 * _capacity));
        _capacity *= 2;
    }
    memcpy(_data + _size, data, size);
    _size += size;
}

unsigned char * FrameCache::get() {
    return _data;
}

unsigned int FrameCache::size() {
    return _size;
}

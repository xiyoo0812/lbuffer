#include "buffer.h"
#include <assert.h>
#include <stdlib.h>
#include <memory.h>

#define BUFFER_MAX 65535 * 65535    //1GB

//�����ڴ�
size_t _buffer_regularize(struct buffer* buf) {
    size_t data_len = (size_t)(buf->tail - buf->head);
    if (buf->head > buf->data) {
        if (data_len > 0){
            memmove(buf->data, buf->head, data_len);
        }
        buf->tail = buf->data + data_len;
        buf->head = buf->data;
    }
    return buf->size - data_len;
}

//�������ó���
size_t _buffer_resize(struct buffer* buf, size_t size) {
    size_t data_len = (size_t)(buf->tail - buf->head);
    if (buf->size == size || size < data_len || size >(size_t)BUFFER_MAX) {
        return buf->end - buf->tail;
    }
    buf->data = (uint8_t*)realloc(buf->data, size);
    buf->tail = buf->data + data_len;
    buf->end = buf->data + size;
    buf->head = buf->data;
    buf->size = size;
    return size - data_len;
}

struct buffer* buffer_alloc(size_t size) {
    struct buffer* buf = (struct buffer*)malloc(sizeof(struct buffer));
    buf->data = (uint8_t*)malloc(size);
    buf->end = buf->data + size;
    buf->head = buf->data;
    buf->tail = buf->head;
    buf->ori_size = size;
    buf->size = size;
    return buf;
}

void buffer_close(struct buffer* buf) {
    if (buf) {
        free(buf->data);
        buf->head = buf->tail = buf->end = buf->data = NULL;
        free(buf);
        buf = NULL;
    }
}

void buffer_reset(struct buffer* buf) {
    buf->data = (uint8_t*)realloc(buf->data, buf->ori_size);
    memset(buf->data, 0, buf->ori_size);
    buf->head = buf->tail = buf->data;
    buf->size = buf->ori_size;
}

size_t buffer_size(struct buffer* buf) {
    return buf->tail - buf->head;
}

size_t buffer_copy(struct buffer* buf, size_t offset, const uint8_t* src, size_t src_len) {
    size_t data_len = buf->tail - buf->head;
    if (offset + src_len <= data_len) {
        memcpy(buf->head + offset, src, src_len);
        return src_len;
    }
    return 0;
}

size_t buffer_apend(struct buffer* buf, const uint8_t* src, size_t src_len) {
    uint8_t* target = buffer_attach(buf, src_len);
    if (target) {
        memcpy(target, src, src_len);
        buf->tail += src_len;
        return src_len;
    }
    return 0;
}

size_t buffer_erase(struct buffer* buf, size_t erase_len) {
    if (buf->head + erase_len <= buf->tail) {
        buf->head += erase_len;
        size_t data_len = (size_t)(buf->tail - buf->head);
        if (buf->size > buf->ori_size && data_len < buf->size / 4) {
            _buffer_regularize(buf);
            _buffer_resize(buf, buf->size / 2);
        }
        return erase_len;
    }
    return 0;
}

size_t buffer_peek(struct buffer* buf, uint8_t* dest, size_t peek_len) {
    size_t data_len = buf->tail - buf->head;
    if (peek_len > 0 && data_len > peek_len) {
        dest = buf->head;
        return peek_len;
    }
    return 0;
}

size_t buffer_read(struct buffer* buf, uint8_t* dest, size_t read_len) {
    size_t data_len = buf->tail - buf->head;
    if (read_len > 0 && data_len > read_len) {
        memcpy(dest, buf->head, read_len);
        buf->tail += read_len;
        return read_len;
    }
    return 0;
}

uint8_t* buffer_data(struct buffer* buf, size_t* len) {
    *len = (size_t)(buf->tail - buf->head);
    return buf->head;
}

uint8_t* buffer_attach(struct buffer* buf, size_t len) {
    size_t space_len = buf->end - buf->tail;
    if (space_len >= len) {
        return buf->tail;
    }
    space_len = _buffer_regularize(buf);
    if (space_len >= len) {
        return buf->tail;
    }
    size_t data_len = buf->tail - buf->head;
    if ((data_len + len) > (size_t)BUFFER_MAX) {
        return NULL;
    }
    size_t nsize = buf->size * 2;
    while (nsize - data_len < len) {
        nsize *= 2;
    }
    _buffer_resize(buf, nsize);
    return buf->tail;
}

size_t buffer_grow(struct buffer* buf, size_t graw_len) {
    if (buf->tail + graw_len <= buf->end) {
        buf->tail += graw_len;
        return graw_len;
    }
    return 0;
}

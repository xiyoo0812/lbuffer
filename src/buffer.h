#pragma once
#include "slice.h"

namespace lbuffer {

    const size_t BUFFER_MAX = 256 * 1024 * 1024;    //256mb

    class var_buffer {
    public:
        var_buffer(size_t size) { _alloc(size); }
        ~var_buffer() { free(m_data); }

        void reset() {
            if (m_size != m_ori_size) {
                m_data = (uint8_t*)realloc(m_data, m_ori_size);
            }
            memset(m_data, 0, m_ori_size);
            m_end = m_data + m_ori_size;
            m_head = m_tail = m_data;
            m_size = m_ori_size;
        }

        size_t size() {
            return m_tail - m_head;
        }

        size_t copy(size_t offset, const uint8_t* src, size_t src_len) {
            size_t data_len = m_tail - m_head;
            if (offset + src_len <= data_len) {
                memcpy(m_head + offset, src, src_len);
                return src_len;
            }
            return 0;
        }

        size_t push(const uint8_t* src, size_t push_len) {
            uint8_t* target = attach(push_len);
            if (target) {
                memcpy(target, src, push_len);
                m_tail += push_len;
                return push_len;
            }
            return 0;
        }

        size_t pop(uint8_t* dest, size_t pop_len) {
            size_t data_len = m_tail - m_head;
            if (pop_len > 0 && data_len >= pop_len) {
                memcpy(dest, m_head, pop_len);
                m_head += pop_len;
                return pop_len;
            }
            return 0;
        }

        size_t pop_space(size_t erase_len) {
            if (m_head + erase_len <= m_tail) {
                m_head += erase_len;
                size_t data_len = (size_t)(m_tail - m_head);
                if (m_size > m_ori_size && data_len < m_size / 4) {
                    _regularize();
                    _resize(m_size / 2);
                }
                return erase_len;
            }
            return 0;
        }

        slice* slice(size_t len) {
            size_t data_len = m_tail - m_head;
            if (data_len > len) {
                m_slice.attach(m_head, len);
                return &m_slice;
            }
            return nullptr;
        }

        uint8_t* peek(size_t peek_len) {
            size_t data_len = m_tail - m_head;
            if (peek_len > 0 && data_len >= peek_len) {
                return m_head;
            }
            return nullptr;
        }

        uint8_t* attach(size_t len) {
            size_t space_len = m_end - m_tail;
            if (space_len >= len) {
                return m_tail;
            }
            space_len = _regularize();
            if (space_len >= len) {
                return m_tail;
            }
            size_t data_len = m_tail - m_head;
            if ((data_len + len) > BUFFER_MAX) {
                return nullptr;
            }
            size_t nsize = m_size * 2;
            while (nsize - data_len < len) {
                nsize *= 2;
            }
            _resize(nsize);
            return m_tail;
        }

        uint8_t* data(size_t* len) {
            *len = (size_t)(m_tail - m_head);
            return m_head;
        }

        std::string string() {
            size_t len = (size_t)(m_tail - m_head);
            return std::string((const char*)m_head, len);
        }

    protected:
        //整理内存
        size_t _regularize() {
            size_t data_len = (size_t)(m_tail - m_head);
            if (m_head > m_data) {
                if (data_len > 0) {
                    memmove(m_data, m_head, data_len);
                }
                m_tail = m_data + data_len;
                m_head = m_data;
            }
            return m_size - data_len;
        }

        //重新设置长度
        size_t _resize(size_t size) {
            size_t data_len = (size_t)(m_tail - m_head);
            if (m_size == size || size < data_len || size > BUFFER_MAX) {
                return m_end - m_tail;
            }
            m_data = (uint8_t*)realloc(m_data, size);
            m_tail = m_data + data_len;
            m_end = m_data + size;
            m_head = m_data;
            m_size = size;
            return size - data_len;
        }

        void _alloc(size_t size) {
            m_data = (uint8_t*)malloc(size);
            m_head = m_tail = m_data;
            m_end = m_data + size;
            m_ori_size = size;
            m_size = size;
        }

    private:
        size_t m_size;
        uint8_t* m_head;
        uint8_t* m_tail;
        uint8_t* m_end;
        uint8_t* m_data;
        size_t m_ori_size;
        lbuffer::slice m_slice;
    };
}

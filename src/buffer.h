#ifndef __BUFFER_H__
#define __BUFFER_H__

#include <stdint.h>
#include "lbuffer.h"

#ifdef __cplusplus
extern "C" {
#endif

struct buffer {
    uint8_t* head;
    uint8_t* tail;
    uint8_t* end;
    uint8_t* data;
    size_t ori_size;
    size_t size;
};

//����buffer
LBUFF_API struct buffer* buffer_alloc(size_t size);
//�ͷ�buffer
LBUFF_API void buffer_close(struct buffer* buf);
//����
LBUFF_API void buffer_reset(struct buffer* buf);
//��ȡbuffsize
LBUFF_API size_t buffer_size(struct buffer* buf);
//����
LBUFF_API size_t buffer_copy(struct buffer* buf, size_t offset, const uint8_t* src, size_t src_len);
//д��
LBUFF_API size_t buffer_apend(struct buffer* buf, const uint8_t* src, size_t src_len);
//�ƶ�ͷָ��
LBUFF_API size_t buffer_erase(struct buffer* buf, size_t erase_len);
//ȫ������
LBUFF_API uint8_t* buffer_data(struct buffer* buf, size_t* len);
//���Զ���
LBUFF_API size_t buffer_peek(struct buffer* buf, uint8_t* dest, size_t peek_len);
//����
LBUFF_API size_t buffer_read(struct buffer* buf, uint8_t* dest, size_t read_len);
//���ؿ�дָ��
LBUFF_API uint8_t* buffer_attach(struct buffer* buf, size_t len);
//�ƶ�βָ��
LBUFF_API size_t buffer_grow(struct buffer* buf, size_t graw_len);

#ifdef __cplusplus
}
#endif

#endif

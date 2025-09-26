
#ifndef _SERIAL_MSG_H
#define _SERIAL_MSG_H

#include <stdint.h>

// [type][len][data][CRC]
#define MSG_LEN(data_len) (6 + (data_len))
#define MSG_HEAD_LEN 4

enum
{
    _MSG_LOG = 0x4c4c,            // 'L' 'L'
    _MSG_LOG_OBFUSCATED = 0x205a, //
};

static inline void _msg_set_hw_LE(uint8_t *buf, uint16_t n)
{
    buf[0] = 0xff & n;
    buf[1] = 0xff & (n >> 8);
}

static inline void _msg_set_word_LE(uint8_t *buf, uint32_t n)
{
    buf[0] = 0xff & n;
    buf[1] = 0xff & (n >> 8);
    buf[2] = 0xff & (n >> 16);
    buf[3] = 0xff & (n >> 24);
}

static inline void msg_set_type(uint8_t *buf, uint16_t type)
{
    _msg_set_hw_LE(buf, type);
}

static inline void msg_set_len(uint8_t *buf, uint16_t len)
{
    _msg_set_hw_LE(buf + 2, len);
}

uint16_t msg_CRC(const uint8_t *buf, uint16_t size);
void msg_obfus(uint8_t *buf, uint16_t size);
uint16_t msg_tx(const uint8_t *buf, uint16_t size);

#endif // _SERIAL_MSG_H

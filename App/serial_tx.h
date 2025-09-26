

#ifndef _SERIAL_TX_H
#define _SERIAL_TX_H

#include <stdint.h>
#include <string.h>

void serial_tx_init();
uint32_t serial_tx(const uint8_t *buf, uint32_t size);

static inline uint32_t serial_tx_str(const char *buf)
{
    return serial_tx((const uint8_t *)buf, strlen(buf));
}

#endif // _SERIAL_TX_H

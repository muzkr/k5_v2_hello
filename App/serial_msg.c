
#include "serial_msg.h"
#include "serial_tx.h"

static uint8_t _OBFUS_TBL[] = "\x16\x6c\x14\xe6\x2e\x91\x0d\x40\x21\x35\xd5\x40\x13\x03\xe9\x80";

uint16_t msg_CRC(const uint8_t *buf, uint16_t size)
{
    uint16_t CRC = 0;

    for (int i = 0; i < size; i++)
    {

        uint16_t b = 0xff & buf[i];
        CRC ^= (b << 8);

        for (int j = 0; j < 8; j++)
        {

            // Check bit [15]
            if (CRC >> 15)
            {
                CRC = (CRC << 1) ^ 0x1021;
            }
            else
            {
                CRC = CRC << 1;
            }
        }
    }

    return CRC;
}

void msg_obfus(uint8_t *buf, uint16_t size)
{
    uint32_t N = sizeof(_OBFUS_TBL);
    for (int i = 0; i < size; i++)
    {
        buf[i] ^= _OBFUS_TBL[i % N];
    }
}

uint16_t msg_tx(const uint8_t *buf, uint16_t size)
{
    // [0xcdab][len][...][0xbadc]
    uint16_t size1 = size + 6;
    uint8_t buf1[4];

    //
    _msg_set_hw_LE(buf1, 0xcdab);
    _msg_set_hw_LE(buf1 + 2, size - 2); // Excluding CRC
    uint16_t size2 = serial_tx(buf1, 4);
    size1 -= size2;
    if (size2 < 4)
    {
        return size1;
    }

    //
    size2 = serial_tx(buf, size);
    size1 -= size2;
    if (size2 < size)
    {
        return size1;
    }

    //
    _msg_set_hw_LE(buf1, 0xbadc);
    size2 = serial_tx(buf1, 2);
    size1 -= size2;
    if (size2 < 2)
    {
        return size1;
    }

    return size1;
}

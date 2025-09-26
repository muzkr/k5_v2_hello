
#include "serial_tx.h"
#include "main.h"
#include "lwrb/lwrb.h"
#include "FreeRTOS.h"
#include "semphr.h"

#define PTR_MASK 0xff
#define BUF_SIZE (1 + PTR_MASK)

#define USART USART1

#define DMA DMA1
#define DMA_CHANNEL LL_DMA_CHANNEL_3
#define DMA_IRQn DMA1_Channel2_3_IRQn

static uint8_t tx_buf[BUF_SIZE] = {0};
static lwrb_t tx_rb = {0};
static struct
{
    uint32_t size;
} DMA_params = {0};

static SemaphoreHandle_t mutex;
static StaticSemaphore_t mutex_obj;

void serial_tx_init()
{

    lwrb_init(&tx_rb, tx_buf, BUF_SIZE);

    LL_SYSCFG_SetDMARemap_CH3(LL_SYSCFG_DMA_MAP_USART1_TX);

    LL_USART_EnableDirectionTx(USART);
    LL_USART_EnableDMAReq_TX(USART);
    LL_USART_ClearFlag_TC(USART);

    do
    {
        LL_DMA_InitTypeDef init;
        LL_DMA_StructInit(&init);
        init.PeriphOrM2MSrcAddress = LL_USART_DMA_GetRegAddr(USART);
        init.MemoryOrM2MDstAddress = (uint32_t)tx_buf;
        init.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
        init.Mode = LL_DMA_MODE_NORMAL;
        init.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
        init.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
        init.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_BYTE;
        init.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_BYTE;
        init.NbData = 0;
        init.Priority = LL_DMA_PRIORITY_HIGH;

        LL_DMA_Init(DMA, DMA_CHANNEL, &init);

        LL_DMA_EnableIT_TC(DMA, DMA_CHANNEL);
        LL_DMA_EnableIT_HT(DMA, DMA_CHANNEL);
        LL_DMA_EnableIT_TE(DMA, DMA_CHANNEL);
        NVIC_SetPriority(DMA_IRQn, 1);
        NVIC_EnableIRQ(DMA_IRQn);
    } while (0);

    LL_DMA_DisableChannel(DMA, DMA_CHANNEL);
    LL_DMA_ClearFlag_TE3(DMA);
    LL_DMA_ClearFlag_HT3(DMA);
    LL_DMA_ClearFlag_TC3(DMA);
    LL_DMA_ClearFlag_GI3(DMA);

    mutex = xSemaphoreCreateMutexStatic(&mutex_obj);
}

uint32_t serial_tx(const uint8_t *buf, uint32_t size)
{
    xSemaphoreTake(mutex, portMAX_DELAY);
    {
        size = (uint32_t)lwrb_write(&tx_rb, buf, size);
    }
    xSemaphoreGive(mutex);

    NVIC_SetPendingIRQ(DMA_IRQn);

    return size;
}

void DMA1_Channel2_3_IRQHandler()
{
    lwrb_t *rb = &tx_rb;

    if (!LL_DMA_IsEnabledChannel(DMA, DMA_CHANNEL))
    {
        uint32_t size = lwrb_get_linear_block_read_length(rb);
        if (size > 0)
        {
            LL_DMA_SetMemoryAddress(DMA, DMA_CHANNEL, (uint32_t)lwrb_get_linear_block_read_address(rb));
            LL_DMA_SetDataLength(DMA, DMA_CHANNEL, size);
            DMA_params.size = size;
            LL_DMA_EnableChannel(DMA, DMA_CHANNEL);
        }

        return;
    }

    if (LL_DMA_IsActiveFlag_HT3(DMA))
    {
        LL_DMA_ClearFlag_HT3(DMA);

        uint32_t rem = LL_DMA_GetDataLength(DMA, DMA_CHANNEL);
        if (DMA_params.size > rem)
        {
            uint32_t size = DMA_params.size - rem;
            size = lwrb_skip(rb, size);
            DMA_params.size -= size;
        }
    }

    if (LL_DMA_IsActiveFlag_TC3(DMA) || LL_DMA_IsActiveFlag_TE3(DMA))
    {
        if (LL_DMA_IsActiveFlag_TC3(DMA))
        {
            LL_DMA_ClearFlag_TC3(DMA);
        }
        if (LL_DMA_IsActiveFlag_TE3(DMA))
        {
            LL_DMA_ClearFlag_TE3(DMA);
        }

        lwrb_skip(rb, DMA_params.size);

        LL_DMA_DisableChannel(DMA, DMA_CHANNEL);
        DMA_params.size = 0;

        uint32_t size = lwrb_get_linear_block_read_length(rb);
        if (size > 0)
        {
            // Wait tx complete
            while (!LL_USART_IsActiveFlag_TXE(USART))
                ;
            while (!LL_USART_IsActiveFlag_TC(USART))
                ;

            LL_USART_ClearFlag_TC(USART);

            LL_DMA_SetMemoryAddress(DMA, DMA_CHANNEL, (uint32_t)lwrb_get_linear_block_read_address(rb));
            LL_DMA_SetDataLength(DMA, DMA_CHANNEL, size);
            DMA_params.size = size;
            LL_DMA_EnableChannel(DMA, DMA_CHANNEL);
        }
    }
}

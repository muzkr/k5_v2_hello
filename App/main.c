
#include "main.h"
#include "serial_tx.h"

static void APP_SystemClockConfig(void);
static void init_board();

/**
 * @brief  Main program.
 * @param  None
 * @retval int
 */
int main(void)
{
    /* Enable SYSCFG and PWR clocks */
    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_SYSCFG);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

    /* Configure system clock */
    APP_SystemClockConfig();

    //   NVIC_SetPriority(PendSV_IRQn, 3);
    init_board();
    serial_tx_init();

    // PB6: flashlight
    LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_6);

    while (1)
    {
        LL_GPIO_TogglePin(GPIOB, LL_GPIO_PIN_6);
        LL_mDelay(500);
    }
}

/**
 * @brief  System Clock Configuration
 * @param  None
 * @retval None
 */
static void APP_SystemClockConfig(void)
{
    /* Enable HSI */
    LL_RCC_HSI_Enable();
    LL_RCC_HSI_SetCalibFreq(LL_RCC_HSICALIBRATION_24MHz);
    while (LL_RCC_HSI_IsReady() != 1)
    {
    }

    /* Set AHB prescaler */
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);

    /* Configure HSISYS as system clock source */
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSISYS);
    while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSISYS)
    {
    }

    LL_FLASH_SetLatency(LL_FLASH_LATENCY_0);

    /* Set APB1 prescaler*/
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
    LL_Init1msTick(24000000);

    /* Update system clock global variable SystemCoreClock (can also be updated by calling SystemCoreClockUpdate function) */
    LL_SetSystemCoreClock(24000000);
}

static void init_board()
{
    // GPIO
    do
    {
        // PA2 - USART1_TX
        // PA3 - USART1_RX, PTT
        // PB6 - Flashlight

        LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA | LL_IOP_GRP1_PERIPH_GPIOB);

        LL_GPIO_InitTypeDef init;

        // PA2
        // LL_GPIO_StructInit(&init ) ;
        init.Pin = LL_GPIO_PIN_2;
        init.Mode = LL_GPIO_MODE_ALTERNATE;
        init.Alternate = LL_GPIO_AF_1;
        init.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
        init.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        init.Pull = LL_GPIO_PULL_UP;
        LL_GPIO_Init(GPIOA, &init);

        // PA3
        // LL_GPIO_StructInit(&init ) ;
        init.Pin = LL_GPIO_PIN_3;
        init.Mode = LL_GPIO_MODE_ALTERNATE;
        init.Alternate = LL_GPIO_AF_1;
        init.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
        init.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        init.Pull = LL_GPIO_PULL_UP;
        LL_GPIO_Init(GPIOA, &init);

        // PB6
        LL_GPIO_StructInit(&init);
        init.Pin = LL_GPIO_PIN_6;
        init.Mode = LL_GPIO_MODE_OUTPUT;
        init.Speed = LL_GPIO_SPEED_FREQ_LOW;
        init.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
        LL_GPIO_Init(GPIOB, &init);

    } while (0);

    // USART1
    do
    {
        LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_USART1);

        LL_USART_InitTypeDef init;
        LL_USART_StructInit(&init);
        init.BaudRate = 38400;
        init.DataWidth = LL_USART_DATAWIDTH_8B;
        init.Parity = LL_USART_PARITY_NONE;
        init.StopBits = LL_USART_STOPBITS_1;
        init.TransferDirection = LL_USART_DIRECTION_TX_RX;
        LL_USART_Init(USART1, &init);

        LL_USART_Enable(USART1);

    } while (0);

    // DMA
    do
    {
        LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
    } while (0);
}

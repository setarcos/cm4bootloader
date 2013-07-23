/* vim: set ts=2 sw=2: */
#include "flash_if.h"
#include "flash_spi.h"
#include "yycm.h"

#define FLASH_BLOCK 26
#define BIN_START (FLASH_BLOCK * 3)
#define BIN_SIZE 256
#define SEC_SIZE 0x1000
#define BK(x) (*(__IO uint32_t *)(BKPSRAM_BASE + x))

typedef  void (*pFunction)(void);
pFunction Jump_To_Application;
uint32_t JumpAddress;

int readyToUpgrade(void);
void Upgrade(void);

int main(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f4xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
        system_stm32f4xx.c file
     */

  /* Enable the PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_BKPSRAM, ENABLE);

  /* Allow access to Backup domain */
  PWR_BackupAccessCmd(ENABLE);

  /* Test if Key push-button on STM324xG-EVAL Board is pressed */
  if (readyToUpgrade())
  { 
    FLASH_If_Init();
    /* Upgrade the firmware from SPI Flash */
    Upgrade();
  }

  /* Test if user code is programmed starting from address "APPLICATION_ADDRESS" */
  if (((*(__IO uint32_t*)APPLICATION_ADDRESS) & 0x2FFE0000 ) == 0x20000000)
  { 
    /* Enable IWDG */
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler(IWDG_Prescaler_32);
    IWDG_SetReload(0xFFF); // 4096ms for 32KHz LSI
    IWDG_ReloadCounter();
    IWDG_Enable();
    /* Jump to user application */
    JumpAddress = *(__IO uint32_t*) (APPLICATION_ADDRESS + 4);
    Jump_To_Application = (pFunction) JumpAddress;
    /* Initialize user application's Stack Pointer */
    __set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS);
    Jump_To_Application();
  }

  while (1) {}
}

int readyToUpgrade()
{
  uint8_t buf[1000];
  if (BK(0) != 0xaddeefbe) return 0;

  YYCM_InitGPIO();
  YYCM_InitSPI();
  sFLASH_ReadBuffer(buf, BIN_START * SEC_SIZE, 500); // 500 for now
  uint16_t total = (buf[0] << 8) + buf[1];
  if ((total > 250) || (total == 0)) return 0;
  uint16_t lastsize = (buf[total * 2 + 2] << 8) + buf[total * 2 + 3];
  if (lastsize > BIN_SIZE) return 0;
  for (int i = 1; i <= total; ++i) {
    if (i != (buf[i * 2] << 8) + buf[i * 2 + 1]) return 0;
  }
  uint8_t check = 0;
  for (int i = 0; i < total; ++i) {
    uint16_t size;
    if (i == total - 1)
      size = lastsize;
    else
      size = BIN_SIZE;
    sFLASH_ReadBuffer(buf, (BIN_START + 1) * SEC_SIZE + i * BIN_SIZE, size);
    for (int j = 0; j < size; ++j)
      check ^= buf[j];
  }
  if (check != 'Y') return 0;
  return 1;
}

void Upgrade()
{
  uint32_t buf[BIN_SIZE / 4];
  uint8_t *spi_buf;
  spi_buf = (uint8_t *)buf;
  FLASH_If_Erase(APPLICATION_ADDRESS);
  sFLASH_ReadBuffer(spi_buf, BIN_START * SEC_SIZE, 2);
  uint16_t total = (spi_buf[0] << 8) + spi_buf[1];
  for (int i = 0; i < total; ++i) {
    uint32_t add;
    add = APPLICATION_ADDRESS + i * BIN_SIZE;
    sFLASH_ReadBuffer(spi_buf, (BIN_START + 1) * SEC_SIZE + i * BIN_SIZE, BIN_SIZE);
    FLASH_If_Write(&add, buf, BIN_SIZE / 4);
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

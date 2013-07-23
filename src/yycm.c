#include "yycm.h"

void YYCM_InitSPI()
{
  SPI_InitTypeDef SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  // enable the SPI peripheral clock
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);
  // enable the peripheral GPIO port clocks
  //RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  //RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  // now we can set up the SPI peripheral
  SPI_StructInit(&SPI_InitStructure);
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
  SPI_Init(SPI1, &SPI_InitStructure);
  SPI_CalculateCRC(SPI1, DISABLE);
  // Enable the SPI port==================================================
  SPI_Cmd(SPI1, ENABLE);
}

void YYCM_InitGPIO()
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
  /* Enable the GPIO Clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOA, ENABLE);
  /* Configure the GPIO pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_4; // BUZ & SPI1_CS1
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_SetBits(GPIOC, GPIO_Pin_4);
  GPIO_ResetBits(GPIOC, GPIO_Pin_0);
}

uint8_t YYCM_SpiPut(uint8_t cData)
{
  /*!< Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
  /*!< Send byte through the SPI1 peripheral */
  SPI_I2S_SendData(SPI1, cData);
  /*!< Wait to receive a byte */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
  return(SPI_I2S_ReceiveData(SPI1));
}

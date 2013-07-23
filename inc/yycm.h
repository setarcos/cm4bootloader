#ifndef __YYCM4_H__
#define __YYCM4_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

void YYCM_InitSPI();
void YYCM_InitGPIO();
uint8_t YYCM_SpiPut(uint8_t cData);

#ifdef __cplusplus
 extern "C" {
#endif
#endif

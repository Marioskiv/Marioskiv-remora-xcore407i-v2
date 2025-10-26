/**
  ******************************************************************************
  * @file    xcore407i_eth_bsp.h
  * @brief   Header for xcore407i_eth_bsp.c
  ******************************************************************************
  */

#ifndef XCORE407I_ETH_BSP_H
#define XCORE407I_ETH_BSP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"

/* Exported functions --------------------------------------------------------*/
ETH_HandleTypeDef* ETH_BSP_Config(void);

#ifdef __cplusplus
}
#endif

#endif /* XCORE407I_ETH_BSP_H */

/**
  ******************************************************************************
  * @file    xcore407i_eth_bsp.c
  * @brief   XCore407I Ethernet hardware configuration for DP83848I PHY (RMII)
  * @details Pin mapping for XCore407I board:
  *          - REF_CLK:  PA1  (50 MHz input from PHY)
  *          - CRS_DV:   PA7  (Carrier sense / data valid)
  *          - RXD0:     PC4  (Receive data bit 0)
  *          - RXD1:     PC5  (Receive data bit 1)
  *          - TX_EN:    PB11 (Transmit enable)
  *          - TXD0:     PB12 (Transmit data bit 0)
  *          - TXD1:     PB13 (Transmit data bit 1)
  *          - MDC:      PC1  (Management clock)
  *          - MDIO:     PA2  (Management data I/O)
  *          - PHY_RST:  PG13 (PHY reset, active low)
  ******************************************************************************
  */

#include "stm32f4xx_hal.h"
#include "xcore407i_eth_bsp.h"

/* Private defines -----------------------------------------------------------*/
/* Note: DP83848_PHY_ADDRESS now defined in stm32f4xx_hal_conf.h */

/* Ethernet GPIO Configuration for XCore407I
    PA1  ------> ETH_REF_CLK
    PA2  ------> ETH_MDIO
    PA7  ------> ETH_CRS_DV
    PC1  ------> ETH_MDC
    PC4  ------> ETH_RXD0
    PC5  ------> ETH_RXD1
    PB11 ------> ETH_TX_EN
    PB12 ------> ETH_TXD0
    PB13 ------> ETH_TXD1
    PG13 ------> PHY_RESET (GPIO output)
*/

/* Private variables ---------------------------------------------------------*/
ETH_HandleTypeDef heth;

/* Private function prototypes -----------------------------------------------*/
/* GPIO and PHY config now handled in HAL_ETH_MspInit */

/**
  * @brief  Configures the Ethernet Interface
  * @param  None
  * @retval ETH_HandleTypeDef pointer
  */
ETH_HandleTypeDef* ETH_BSP_Config(void)
{
    /* Configure Ethernet peripheral - minimal config for STM32F4 HAL */
    heth.Instance = ETH;
    heth.Init.MACAddr[0] = 0x00;
    heth.Init.MACAddr[1] = 0x80;
    heth.Init.MACAddr[2] = 0xE1;
    heth.Init.MACAddr[3] = 0xCC;
    heth.Init.MACAddr[4] = 0x07;
    heth.Init.MACAddr[5] = 0x01;
    heth.Init.MediaInterface = HAL_ETH_RMII_MODE;
    
    /* HAL_ETH_Init will automatically call HAL_ETH_MspInit */
    if (HAL_ETH_Init(&heth) != HAL_OK)
    {
        // Initialization Error
        return NULL;
    }
    
    return &heth;
}



/**
  * @brief  Ethernet Rx Transfer completed callback
  * @param  heth: ETH handle
  * @retval None
  */
void HAL_ETH_RxCpltCallback(ETH_HandleTypeDef *heth)
{
    // Will be called when packet received
    // ethernetif_input() should be called from here or main loop
}

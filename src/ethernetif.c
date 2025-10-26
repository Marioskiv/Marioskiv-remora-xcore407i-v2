/**
 * @file ethernetif_simple.c
 * @brief Simple Ethernet interface for LwIP v1.3.2 with STM32 HAL
 * 
 * Simplified version that works with basic HAL ETH APIs
 */

#include "main.h"
#include "stm32f4xx_hal.h"
#include "lwip/opt.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "netif/etharp.h"
#include "lwip/err.h"
#include "ethernetif.h"
#include <string.h>

#ifndef ETH_DMARXDESC_FRAMELENGTHSHIFT
#define ETH_DMARXDESC_FRAMELENGTHSHIFT 16U
#endif

/* Network interface name */
#define IFNAME0 's'
#define IFNAME1 't'

/* External ETH handle - defined in xcore407i_eth_bsp.c */
extern ETH_HandleTypeDef heth;

/* Ethernet Rx & Tx DMA Descriptors */
__attribute__((section(".RxDecripSection"))) __attribute__((aligned(4))) ETH_DMADescTypeDef  DMARxDscrTab[ETH_RXBUFNB];
__attribute__((section(".TxDecripSection"))) __attribute__((aligned(4))) ETH_DMADescTypeDef  DMATxDscrTab[ETH_TXBUFNB];

/* Ethernet Driver Receive buffers  */
__attribute__((section(".RxArraySection"))) __attribute__((aligned(4))) uint8_t Rx_Buff[ETH_RXBUFNB][ETH_RX_BUF_SIZE]; 

/* Ethernet Driver Transmit buffers */
__attribute__((section(".TxArraySection"))) __attribute__((aligned(4))) uint8_t Tx_Buff[ETH_TXBUFNB][ETH_TX_BUF_SIZE];

/* Current descriptor cursors (we manage them locally to avoid HAL field differences) */
static __IO ETH_DMADescTypeDef *CurTxDesc;
static __IO ETH_DMADescTypeDef *CurRxDesc;

/**
 * @brief Initialize the low level hardware (descriptors, buffers)
 */
static void low_level_init(struct netif *netif)
{
  uint32_t i;
  
  /* Set MAC hardware address length */
  netif->hwaddr_len = ETHARP_HWADDR_LEN;

  /* Set MAC hardware address */
  netif->hwaddr[0] = MAC_ADDR0;
  netif->hwaddr[1] = MAC_ADDR1;
  netif->hwaddr[2] = MAC_ADDR2;
  netif->hwaddr[3] = MAC_ADDR3;
  netif->hwaddr[4] = MAC_ADDR4;
  netif->hwaddr[5] = MAC_ADDR5;

  /* Maximum transfer unit */
  netif->mtu = 1500;

  /* Device capabilities */
  netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;

  /* Initialize Tx Descriptors list */
  CurTxDesc = DMATxDscrTab;
  for(i=0; i < ETH_TXBUFNB; i++)
  {
    /* Initialize Tx descriptor */
    DMATxDscrTab[i].DESC0 = ETH_DMATXDESC_TCH; /* Second Address Chained */
    DMATxDscrTab[i].DESC2 = (uint32_t)(&Tx_Buff[i][0]); /* Buffer1 address */
    
    /* Set next descriptor address (chain mode) */
    if(i < (ETH_TXBUFNB-1))
    {
      DMATxDscrTab[i].DESC3 = (uint32_t)(&DMATxDscrTab[i+1]);
    }
    else
    {
      DMATxDscrTab[i].DESC3 = (uint32_t)(&DMATxDscrTab[0]);
    }
  }
  
  /* Initialize Rx Descriptors list */
  CurRxDesc = DMARxDscrTab;
  for(i=0; i < ETH_RXBUFNB; i++)
  {
    /* Initialize Rx descriptor */
    DMARxDscrTab[i].DESC0 = ETH_DMARXDESC_OWN; /* Owned by DMA */
    DMARxDscrTab[i].DESC1 = ETH_DMARXDESC_RCH | ETH_RX_BUF_SIZE; /* RCH + size */
    DMARxDscrTab[i].DESC2 = (uint32_t)(&Rx_Buff[i][0]); /* Buffer1 address */
    
    /* Set next descriptor address (chain mode) */
    if(i < (ETH_RXBUFNB-1))
    {
      DMARxDscrTab[i].DESC3 = (uint32_t)(&DMARxDscrTab[i+1]);
    }
    else
    {
      DMARxDscrTab[i].DESC3 = (uint32_t)(&DMARxDscrTab[0]);
    }
  }
  
  /* Set Transmit Descriptor List Address Register */
  heth.Instance->DMATDLAR = (uint32_t)DMATxDscrTab;
  
  /* Set Receive Descriptor List Address Register */
  heth.Instance->DMARDLAR = (uint32_t)DMARxDscrTab;
  
  /* Enable MAC and DMA transmission and reception */
  HAL_ETH_Start(&heth);
}

/**
 * @brief Transmit a packet
 */
static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
  struct pbuf *q;
  uint32_t framelength = 0;
  uint8_t *buffer;
  __IO ETH_DMADescTypeDef *DmaTxDesc;
  
  DmaTxDesc = CurTxDesc;
  buffer = (uint8_t *)(DmaTxDesc->DESC2);
  
  /* Check if the descriptor is owned by DMA */
  if((DmaTxDesc->DESC0 & ETH_DMATXDESC_OWN) != (uint32_t)RESET)
  {
    return ERR_USE;
  }
  
  /* Copy frame from pbufs to driver buffers */
  for(q = p; q != NULL; q = q->next)
  {
    memcpy((uint8_t*)&buffer[framelength], q->payload, q->len);
    framelength += q->len;
  }
  
  /* Prepare transmit descriptor */
  DmaTxDesc->DESC1 = (framelength & ETH_DMATXDESC_TBS1);
  DmaTxDesc->DESC0 = ETH_DMATXDESC_OWN | ETH_DMATXDESC_FS | ETH_DMATXDESC_LS | ETH_DMATXDESC_TCH;
  
  /* Resume DMA transmission */
  heth.Instance->DMATPDR = 0;
  
  /* Update Tx descriptor */
  CurTxDesc = (ETH_DMADescTypeDef *)(DmaTxDesc->DESC3);
  
  return ERR_OK;
}

/**
 * @brief Receive a packet
 */
static struct pbuf * low_level_input(struct netif *netif)
{
  struct pbuf *p = NULL;
  struct pbuf *q;
  uint16_t len = 0;
  uint32_t l = 0;
  uint8_t *buffer;
  __IO ETH_DMADescTypeDef *DmaRxDesc;
  
  /* Get current Rx descriptor */
  DmaRxDesc = CurRxDesc;
  
  /* Check if descriptor is owned by DMA */
  if((DmaRxDesc->DESC0 & ETH_DMARXDESC_OWN) != (uint32_t)RESET)
  {
    return NULL;
  }
  
  /* Check if last descriptor and no errors */
  if(((DmaRxDesc->DESC0 & ETH_DMARXDESC_ES) == (uint32_t)RESET) &&
     ((DmaRxDesc->DESC0 & ETH_DMARXDESC_LS) != (uint32_t)RESET))
  {
    /* Get the Frame Length */
    len = ((DmaRxDesc->DESC0 & ETH_DMARXDESC_FL) >> ETH_DMARXDESC_FRAMELENGTHSHIFT) - 4;
    buffer = (uint8_t *)(DmaRxDesc->DESC2);
    
    /* Allocate a pbuf chain from pool */
    p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
    
    if(p != NULL)
    {
      /* Copy data from buffer to pbuf chain */
      for(q = p; q != NULL; q = q->next)
      {
        memcpy((uint8_t*)q->payload, (uint8_t*)&buffer[l], q->len);
        l += q->len;
      }
    }
  }
  
  /* Give descriptor back to DMA */
  DmaRxDesc->DESC0 = ETH_DMARXDESC_OWN;
  
  /* Clear RBUS flag */
  heth.Instance->DMASR = ETH_DMASR_RBUS;
  
  /* Resume DMA reception */
  heth.Instance->DMARPDR = 0;
  
  /* Update Rx descriptor */
  CurRxDesc = (ETH_DMADescTypeDef *)(DmaRxDesc->DESC3);
  
  return p;
}

/**
 * @brief Called by LwIP when a packet is ready to be read
 */
err_t ethernetif_input(struct netif *netif)
{
  err_t err;
  struct pbuf *p;

  /* Move received packet into a new pbuf */
  p = low_level_input(netif);
  
  /* No packet could be read, silently ignore */
  if(p == NULL) return ERR_MEM;
  
  /* Entry point to the LwIP stack */
  err = netif->input(p, netif);
  
  if(err != ERR_OK)
  {
    pbuf_free(p);
    p = NULL;
  }
  
  return err;
}

/**
 * @brief Initialize the ethernet interface
 */
err_t ethernetif_init(struct netif *netif)
{
  LWIP_ASSERT("netif != NULL", (netif != NULL));
  
  /* Set interface name */
  netif->name[0] = IFNAME0;
  netif->name[1] = IFNAME1;
  
  /* Set output functions */
  netif->output = etharp_output;
  netif->linkoutput = low_level_output;
  
  /* Initialize the hardware */
  low_level_init(netif);
  
  return ERR_OK;
}

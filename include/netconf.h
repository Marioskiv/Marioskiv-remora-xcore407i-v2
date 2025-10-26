/**
  ******************************************************************************
  * @file    netconf.h
  * @brief   Network configuration for XCore407I Ethernet
  ******************************************************************************
  */

#ifndef NETCONF_H
#define NETCONF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Network Configuration based on LAN Host: 192.168.10.0/24 */

/* Static IP configuration (if not using DHCP) */
#define IP_ADDR0   192
#define IP_ADDR1   168
#define IP_ADDR2   10
#define IP_ADDR3   10       /* Board IP: 192.168.10.10 */

#define NETMASK_ADDR0   255
#define NETMASK_ADDR1   255
#define NETMASK_ADDR2   255
#define NETMASK_ADDR3   0   /* Subnet mask: 255.255.255.0 */

#define GW_ADDR0   192
#define GW_ADDR1   168
#define GW_ADDR2   10
#define GW_ADDR3   254      /* Gateway: 192.168.10.254 (your router) */

/* MAC Address - modify last 3 bytes to make unique */
#define MAC_ADDR0   0x00
#define MAC_ADDR1   0x80
#define MAC_ADDR2   0xE1
#define MAC_ADDR3   0xCC    /* Custom MAC for XCore407I */
#define MAC_ADDR4   0x07    /* 07 for F407 */
#define MAC_ADDR5   0x01    /* Board ID */

/* DHCP Configuration (uncomment to enable DHCP instead of static IP) */
// #define USE_DHCP

/* TCP/UDP Ports for Remora communication */
#define REMORA_UDP_PORT     27181   /* Default Remora UDP port */
#define REMORA_TCP_PORT     27181   /* Default Remora TCP port */

/* LwIP periodic task intervals (ms) */
#define TCP_TMR_INTERVAL    250     /* TCP timer */
#define ARP_TMR_INTERVAL    5000    /* ARP timer */

#ifdef USE_DHCP
#define DHCP_FINE_TIMER_INTERVAL    500   /* DHCP fine timer */
#define DHCP_COARSE_TIMER_INTERVAL  60000 /* DHCP coarse timer */
#endif

/* Function prototypes */
void LwIP_Init(void);
void LwIP_Pkt_Handle(void);
void LwIP_Periodic_Handle(uint32_t localtime);

#ifdef __cplusplus
}
#endif

#endif /* NETCONF_H */

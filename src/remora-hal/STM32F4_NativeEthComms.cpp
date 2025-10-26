#include "STM32F4_NativeEthComms.h"

#ifdef USE_NATIVE_ETHERNET

#include "xcore407i_eth_bsp.h"
#include "netconf.h"

extern "C" {
    #include "ethernetif.h"
}

// Static instance pointer for callbacks
static STM32F4_NativeEthComms* ethInstance = nullptr;

STM32F4_NativeEthComms::STM32F4_NativeEthComms(volatile rxData_t* _ptrRxData, volatile txData_t* _ptrTxData)
    : ethLinkUp(false),
      ethInitialized(false),
      lastSysTickCheck(0),
      remora_pcb(nullptr),
      connectionActive(false)
{
    ptrRxData = _ptrRxData;
    ptrTxData = _ptrTxData;
    ethInstance = this;
    
    printf("NativeEthComms: Constructor\n");
}

STM32F4_NativeEthComms::~STM32F4_NativeEthComms()
{
    if (remora_pcb != nullptr)
    {
        tcp_close(remora_pcb);
        remora_pcb = nullptr;
    }
    ethInstance = nullptr;
}

void STM32F4_NativeEthComms::init(void)
{
    printf("NativeEthComms: Initializing Ethernet...\n");
    
    // Initialize Ethernet hardware
    initEthernet();
    
    // Initialize LwIP stack
    initLwIP();
    
    printf("NativeEthComms: Init complete\n");
}

void STM32F4_NativeEthComms::initEthernet()
{
    // Configure Ethernet GPIO and PHY via BSP
    ETH_HandleTypeDef* heth_ptr = ETH_BSP_Config();
    
    if (heth_ptr == NULL)
    {
        printf("ERROR: Ethernet initialization failed!\n");
        return;
    }
    
    heth = *heth_ptr;
    ethInitialized = true;
    
    printf("Ethernet MAC initialized\n");
}

void STM32F4_NativeEthComms::initLwIP()
{
    struct ip_addr ipaddr;
    struct ip_addr netmask;
    struct ip_addr gw;
    
    // Configure static IP (from netconf.h)
    IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
    IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
    IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
    
    // Initialize LwIP stack
    lwip_init();
    
    // Add network interface
    netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input);
    
    // Set as default interface
    netif_set_default(&gnetif);
    
    // Bring interface up
    if (netif_is_link_up(&gnetif))
    {
        netif_set_up(&gnetif);
        ethLinkUp = true;
        printf("Network interface UP\n");
        printf("IP: %d.%d.%d.%d\n", IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
    }
    else
    {
        netif_set_down(&gnetif);
        printf("Network interface DOWN (no link)\n");
    }
}

void STM32F4_NativeEthComms::start(void)
{
    printf("NativeEthComms: Starting TCP server on port %d...\n", REMORA_TCP_PORT);
    
    // Create TCP listening PCB
    remora_pcb = tcp_new();
    
    if (remora_pcb != nullptr)
    {
        err_t err;
        
        // Bind to Remora port
        err = tcp_bind(remora_pcb, IP_ADDR_ANY, REMORA_TCP_PORT);
        
        if (err == ERR_OK)
        {
            remora_pcb = tcp_listen(remora_pcb);
            tcp_accept(remora_pcb, tcp_server_accept);
            
            printf("TCP server listening on port %d\n", REMORA_TCP_PORT);
        }
        else
        {
            printf("ERROR: TCP bind failed: %d\n", err);
        }
    }
    else
    {
        printf("ERROR: Failed to create TCP PCB\n");
    }
}

void STM32F4_NativeEthComms::tasks(void)
{
    // Check for received Ethernet packets
    ethernetif_input(&gnetif);
    
    // Handle LwIP timers (every 250ms for TCP, 5s for ARP)
    uint32_t currentTick = HAL_GetTick();
    
    if (currentTick - lastSysTickCheck >= 250)
    {
        lastSysTickCheck = currentTick;
        
        // TCP periodic timer
        tcp_tmr();
        
        // ARP periodic timer (every 5 seconds)
        static uint8_t arpCounter = 0;
        if (++arpCounter >= 20)  // 20 * 250ms = 5000ms
        {
            arpCounter = 0;
            etharp_tmr();
        }
    }
    
    // Check link status periodically
    checkLinkStatus();
}

void STM32F4_NativeEthComms::checkLinkStatus()
{
    // Read PHY link status (simplified - should use ETH_ReadPHYRegister)
    bool linkUp = netif_is_link_up(&gnetif);
    
    if (linkUp != ethLinkUp)
    {
        ethLinkUp = linkUp;
        
        if (ethLinkUp)
        {
            printf("Ethernet link UP\n");
            netif_set_up(&gnetif);
        }
        else
        {
            printf("Ethernet link DOWN\n");
            netif_set_down(&gnetif);
        }
    }
}

// Static TCP callbacks
err_t STM32F4_NativeEthComms::tcp_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
    if (ethInstance == nullptr) return ERR_VAL;
    
    printf("TCP connection accepted from %d.%d.%d.%d\n",
           ip4_addr1(&newpcb->remote_ip),
           ip4_addr2(&newpcb->remote_ip),
           ip4_addr3(&newpcb->remote_ip),
           ip4_addr4(&newpcb->remote_ip));
    
    ethInstance->connectionActive = true;
    
    // Set callback functions
    tcp_arg(newpcb, ethInstance);
    tcp_recv(newpcb, tcp_server_recv);
    tcp_sent(newpcb, tcp_server_sent);
    tcp_err(newpcb, tcp_server_err);
    
    return ERR_OK;
}

err_t STM32F4_NativeEthComms::tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    STM32F4_NativeEthComms* instance = static_cast<STM32F4_NativeEthComms*>(arg);
    
    if (p == NULL)
    {
        // Connection closed by remote host
        printf("TCP connection closed by remote\n");
        tcp_close(tpcb);
        instance->connectionActive = false;
        return ERR_OK;
    }
    
    // Acknowledge received data
    tcp_recved(tpcb, p->tot_len);
    
    // Process Remora packet (copy to rxData buffer)
    if (p->tot_len <= sizeof(rxData_t))
    {
        pbuf_copy_partial(p, (void*)instance->ptrRxData, p->tot_len, 0);
        instance->flag_new_data();
    }
    
    // Free pbuf
    pbuf_free(p);
    
    return ERR_OK;
}

err_t STM32F4_NativeEthComms::tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
    // Data successfully sent
    return ERR_OK;
}

void STM32F4_NativeEthComms::tcp_server_err(void *arg, err_t err)
{
    STM32F4_NativeEthComms* instance = static_cast<STM32F4_NativeEthComms*>(arg);
    
    printf("TCP error: %d\n", err);
    instance->connectionActive = false;
}

// CommsInterface implementation
uint8_t STM32F4_NativeEthComms::read_byte(void)
{
    // Not used for Ethernet (packet-based)
    return 0;
}

uint8_t STM32F4_NativeEthComms::write_byte(uint8_t data)
{
    // Not used for Ethernet (packet-based)
    return 0;
}

void STM32F4_NativeEthComms::DMA_write(uint8_t* data, uint16_t length)
{
    if (!connectionActive || remora_pcb == nullptr) return;
    
    // Send txData packet via TCP
    err_t err = tcp_write(remora_pcb, data, length, TCP_WRITE_FLAG_COPY);
    
    if (err == ERR_OK)
    {
        tcp_output(remora_pcb);
    }
}

void STM32F4_NativeEthComms::DMA_read(uint8_t* data, uint16_t length)
{
    // Not used - data received via tcp_server_recv callback
}

void STM32F4_NativeEthComms::flag_new_data(void)
{
    // Notify Remora that new data is available
    // This will be called from tcp_server_recv when packet arrives
}

void STM32F4_NativeEthComms::ETH_IRQHandler(void)
{
    HAL_ETH_IRQHandler(&heth);
}

#endif // USE_NATIVE_ETHERNET

#include "STM32F4_NativeEthUDPComms.h"

#ifdef USE_NATIVE_ETHERNET_UDP

#include "xcore407i_eth_bsp.h"
#include "netconf.h"

extern "C" {
    #include "ethernetif.h"
}

static STM32F4_NativeEthUDPComms* udpInstance = nullptr;

STM32F4_NativeEthUDPComms::STM32F4_NativeEthUDPComms(volatile rxData_t* _ptrRxData, volatile txData_t* _ptrTxData)
    : ethLinkUp(false),
      ethInitialized(false),
      udp_ctrl(nullptr),
      last_src_port(0),
      have_peer(false),
      lastSysTickCheck(0)
{
    ptrRxData = _ptrRxData;
    ptrTxData = _ptrTxData;
    udpInstance = this;
}

STM32F4_NativeEthUDPComms::~STM32F4_NativeEthUDPComms()
{
    if (udp_ctrl) {
        udp_remove(udp_ctrl);
        udp_ctrl = nullptr;
    }
    udpInstance = nullptr;
}

void STM32F4_NativeEthUDPComms::init(void)
{
    initEthernet();
    initLwIP();
}

void STM32F4_NativeEthUDPComms::initEthernet()
{
    ETH_HandleTypeDef* heth_ptr = ETH_BSP_Config();
    if (!heth_ptr) {
        printf("ERROR: Ethernet init failed\n");
        return;
    }
    heth = *heth_ptr;
    ethInitialized = true;
}

void STM32F4_NativeEthUDPComms::initLwIP()
{
    struct ip_addr ipaddr, netmask, gw;
    IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
    IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
    IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);

    lwip_init();
    netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input);
    netif_set_default(&gnetif);

    if (netif_is_link_up(&gnetif)) {
        netif_set_up(&gnetif);
        ethLinkUp = true;
    } else {
        netif_set_down(&gnetif);
    }

    // Create UDP PCB and bind to REMORA port
    udp_ctrl = udp_new();
    if (udp_ctrl) {
        if (udp_bind(udp_ctrl, IP_ADDR_ANY, REMORA_UDP_PORT) == ERR_OK) {
            udp_recv(udp_ctrl, &STM32F4_NativeEthUDPComms::udp_recv_cb, this);
        } else {
            printf("ERROR: UDP bind failed\n");
        }
    } else {
        printf("ERROR: UDP pcb allocation failed\n");
    }
}

void STM32F4_NativeEthUDPComms::start(void)
{
    // Nothing specific for UDP
}

void STM32F4_NativeEthUDPComms::tasks(void)
{
    // Pump Ethernet to LwIP
    ethernetif_input(&gnetif);

    // LwIP timers: ARP each 5s (manual, as NO_SYS=1)
    uint32_t now = HAL_GetTick();
    if (now - lastSysTickCheck >= 250) {
        lastSysTickCheck = now;
        static uint8_t arpCounter = 0;
        if (++arpCounter >= 20) {
            arpCounter = 0;
            etharp_tmr();
        }
    }

    checkLinkStatus();
}

void STM32F4_NativeEthUDPComms::checkLinkStatus()
{
    bool linkUp = netif_is_link_up(&gnetif);
    if (linkUp != ethLinkUp) {
        ethLinkUp = linkUp;
        if (ethLinkUp) netif_set_up(&gnetif); else netif_set_down(&gnetif);
    }
}

void STM32F4_NativeEthUDPComms::udp_recv_cb(void* arg, struct udp_pcb* pcb, struct pbuf* p, struct ip_addr* addr, u16_t port)
{
    if (!p) return;
    STM32F4_NativeEthUDPComms* self = static_cast<STM32F4_NativeEthUDPComms*>(arg);
    if (self) self->handle_packet(p, addr, port);
    pbuf_free(p);
}

void STM32F4_NativeEthUDPComms::handle_packet(struct pbuf* p, struct ip_addr* addr, u16_t port)
{
    // Remember peer for replies
    last_src_ip = *addr;
    last_src_port = port;
    have_peer = true;

    // Copy small header for decision
    if (p->tot_len < sizeof(uint32_t)) return;
    uint32_t header = 0;
    pbuf_copy_partial(p, &header, sizeof(header), 0);

    if (header == PRU_READ) {
        // Host requests full txData
        ptrTxData->header = PRU_DATA;
        struct pbuf* q = pbuf_alloc(PBUF_TRANSPORT, sizeof(ptrTxData->txBuffer), PBUF_RAM);
        if (!q) return;
        pbuf_take(q, (const void*)ptrTxData->txBuffer, sizeof(ptrTxData->txBuffer));
        udp_sendto(udp_ctrl, q, &last_src_ip, last_src_port);
        pbuf_free(q);
    } else if (header == PRU_WRITE) {
        // Host sends full rxData
        if (p->tot_len >= sizeof(ptrRxData->rxBuffer)) {
            pbuf_copy_partial(p, (void*)ptrRxData->rxBuffer, sizeof(ptrRxData->rxBuffer), 0);
            flag_new_data();
        }
        // ACK
        uint32_t ack = PRU_ACKNOWLEDGE;
        struct pbuf* q = pbuf_alloc(PBUF_TRANSPORT, sizeof(ack), PBUF_RAM);
        if (q) {
            pbuf_take(q, &ack, sizeof(ack));
            udp_sendto(udp_ctrl, q, &last_src_ip, last_src_port);
            pbuf_free(q);
        }
    } else if (header == PRU_ESTOP) {
        // ESTOP notification - optional handling
        // No reply required in current scheme
    } else {
        // Unknown header: ignore
    }
}

uint8_t STM32F4_NativeEthUDPComms::read_byte(void) { return 0; }
uint8_t STM32F4_NativeEthUDPComms::write_byte(uint8_t) { return 0; }
void STM32F4_NativeEthUDPComms::DMA_write(uint8_t*, uint16_t) {}
void STM32F4_NativeEthUDPComms::DMA_read(uint8_t*, uint16_t) {}
void STM32F4_NativeEthUDPComms::flag_new_data(void) {}

#endif // USE_NATIVE_ETHERNET_UDP

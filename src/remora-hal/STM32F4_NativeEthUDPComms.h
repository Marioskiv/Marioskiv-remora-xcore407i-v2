#ifndef STM32F4_NATIVEETHUDPCOMMS_H
#define STM32F4_NATIVEETHUDPCOMMS_H

#include "remora-core/configuration.h"

#ifdef USE_NATIVE_ETHERNET_UDP

#include "stm32f4xx_hal.h"
#include <stdio.h>
#include <string.h>
#include <memory>

#include "../remora-core/remora.h"
#include "../remora-core/comms/commsInterface.h"
#include "../remora-core/modules/moduleInterrupt.h"
#include "hal_utils.h"

extern "C" {
    #include "lwip/opt.h"
    #include "lwip/init.h"
    #include "lwip/netif.h"
    #include "lwip/ip_addr.h"
    #include "netif/etharp.h"
    #include "lwip/udp.h"
}

class STM32F4_NativeEthUDPComms : public CommsInterface {
    private:
        ETH_HandleTypeDef heth;
        struct netif gnetif;
        bool ethLinkUp;
        bool ethInitialized;

        struct udp_pcb* udp_ctrl;
        struct ip_addr last_src_ip;
        u16_t last_src_port;
        bool have_peer;

        uint32_t lastSysTickCheck;

        static void udp_recv_cb(void* arg, struct udp_pcb* pcb, struct pbuf* p, struct ip_addr* addr, u16_t port);

        void initEthernet();
        void initLwIP();
        void checkLinkStatus();
        void handle_packet(struct pbuf* p, struct ip_addr* addr, u16_t port);

        // Remora headers (match LinuxCNC driver)
        static constexpr uint32_t PRU_DATA         = 0x64617461; // "data"
        static constexpr uint32_t PRU_READ         = 0x72656164; // "read"
        static constexpr uint32_t PRU_WRITE        = 0x77726974; // "writ"
        static constexpr uint32_t PRU_ACKNOWLEDGE  = 0x61636b6e; // "ackn"
        static constexpr uint32_t PRU_ESTOP        = 0x65737470; // "estp"

    public:
        STM32F4_NativeEthUDPComms(volatile rxData_t*, volatile txData_t*);
        virtual ~STM32F4_NativeEthUDPComms();

        void init(void) override;
        void start(void) override;
        void tasks(void) override;

        uint8_t read_byte(void) override;
        uint8_t write_byte(uint8_t) override;
        void DMA_write(uint8_t*, uint16_t) override;
        void DMA_read(uint8_t*, uint16_t) override;
        virtual void flag_new_data(void) override;
};

#endif // USE_NATIVE_ETHERNET_UDP

#endif // STM32F4_NATIVEETHUDPCOMMS_H

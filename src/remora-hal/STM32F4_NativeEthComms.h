#ifndef STM32F4_NATIVEETHCOMMS_H
#define STM32F4_NATIVEETHCOMMS_H

#include "remora-core/configuration.h"

#ifdef USE_NATIVE_ETHERNET

#include "stm32f4xx_hal.h"
#include <stdio.h>
#include <string.h>
#include <memory>

#include "../remora-core/remora.h"
#include "../remora-core/comms/commsInterface.h"
#include "../remora-core/modules/moduleInterrupt.h"
#include "hal_utils.h"

// LwIP includes
extern "C" {
    #include "lwip/opt.h"
    #include "lwip/init.h"
    #include "lwip/netif.h"
    #include "netif/etharp.h"
    #include "lwip/tcp.h"
    #include "lwip/udp.h"
}

class STM32F4_NativeEthComms : public CommsInterface {
    private:
        ETH_HandleTypeDef heth;
        
        struct netif gnetif;
        bool ethLinkUp;
        bool ethInitialized;
        
        uint32_t lastSysTickCheck;
        
        struct tcp_pcb *remora_pcb;
        bool connectionActive;
        
        // Static callbacks for LwIP (must be static to work with C library)
        static err_t tcp_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err);
        static err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
        static err_t tcp_server_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
        static void tcp_server_err(void *arg, err_t err);
        
        void initEthernet();
        void initLwIP();
        void checkLinkStatus();
        
    public:   
        STM32F4_NativeEthComms(volatile rxData_t*, volatile txData_t*);
        virtual ~STM32F4_NativeEthComms();

        void init(void) override;
        void start(void) override;
        void tasks(void) override;

        uint8_t read_byte(void) override;
        uint8_t write_byte(uint8_t) override;
        void DMA_write(uint8_t*, uint16_t) override;
        void DMA_read(uint8_t*, uint16_t) override;
        virtual void flag_new_data(void) override;
        
        // Ethernet IRQ handler
        void ETH_IRQHandler(void);
};

#endif // USE_NATIVE_ETHERNET

#endif // STM32F4_NATIVEETHCOMMS_H

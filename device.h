/*
 * device.h - Device half of library for talking to the LIFX blubs.
 */
#ifndef device_h
    #define device_h

    /* includes */
    #include "common.h"

    class device{
        public:
            /* Member Functions */
            device();
            void setUDP(UDP &udpRef);
            void setBroadcastIP(IPAddress broadcastIP);
            void setRemotePort(uint16_t remotePort);
            void getService();
            void getPower();

            /* Members */
            struct Header
            {
                /* frame */
                uint16_t size;
                uint16_t protocol:12;
                uint8_t  addressable:1;
                uint8_t  tagged:1;
                uint8_t  origin:2;
                uint32_t source;
                /* frame address */
                uint8_t  target[8];
                uint8_t  reservedA[6];
                uint8_t  res_required:1;
                uint8_t  ack_required:1;
                uint8_t  reservedB:6;
                uint8_t  sequence;
                /* protocol header */
                uint64_t reservedC;
                uint16_t type;
                uint16_t reservedD;
            }__attribute__ ((__packed__));

        private:
            /* Member Functions */

            /* Members */
            UDP _deviceUdp;
            IPAddress _broadcastIP;
        	uint16_t _remotePort;

    };

#endif

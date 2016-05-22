/*
* light.h - Light half of library for talking to the light blubs.
*/
#ifndef _light_h
    #define _light_h
      
    // includes
    #include "common.h"
    
    class light{
        public:
            // Members
            struct Header
            {
                // frame
                uint16_t size;
                uint16_t protocol:12;
                uint8_t  addressable:1;
                uint8_t  tagged:1;
                uint8_t  origin:2;
                uint32_t source;
                // frame address
                uint8_t  target[8];
                uint8_t  reservedA[6];
                uint8_t  res_required:1;
                uint8_t  ack_required:1;
                uint8_t  reservedB:6;
                uint8_t  sequence;
                // protocol header
                uint64_t reservedC;
                uint16_t type;
                uint16_t reservedD;
            }__attribute__ ((__packed__));
            
            struct Lamp {
                uint8_t ip[4];
                uint8_t mac[6];
                uint32_t port;
                uint16_t level;
                uint64_t time;
                uint8_t service;
                uint32_t tx;
                uint32_t rx;
                uint64_t build;
                uint32_t version;
                uint32_t vendor;
                uint32_t product;
                uint64_t uptime;
                uint64_t downtime;
                HSBK hsbk;
                float signal;
                char lable[32];
            }__attribute__ ((__packed__));
            
            // Member Functions
            light();
            // lifx Functions
            void setUDP(UDP &udpRef);
            void setBroadcastIP(IPAddress broadcastIP);
            void setRemotePort(uint16_t remotePort);
            void setIP(uint8_t ip[4]);
            void setMAC(uint8_t mac[6]);
            void setPort(uint32_t port);
            void get();
            void setColor(HSBK hsbk);
            void setColor(HSBK hsbk, uint32_t duration);
            void setColor(uint16_t hue, uint16_t saturation, uint16_t brightness, uint16_t kelvin);
            void setColor(uint16_t hue, uint16_t saturation, uint16_t brightness, uint16_t kelvin, uint32_t duration);
            void setPower(uint16_t onOff);
            void setPower(uint16_t onOff, uint32_t duration);
            void state(uint8_t msg[]);
            void statePower(byte msg[]);
            // Switch Functions
            bool matchMac(byte mac[6]);
            void setPowerLevel(uint16_t powerLevel);
            uint16_t getPowerLevel();
            void setHSBK(HSBK hsbk);
            HSBK getHSBK();
            void setSignal(float signal);
            float getSignal();
            void setTx(uint32_t tx);
            uint32_t getTx();
            void setRx(uint32_t rx);
            uint32_t getRx();

        private:
            Lamp _lamp;
            IPAddress _broadcastIP;
            uint16_t _remotePort;
            uint16_t _powerState;
            UDP _lightUdp;
            
    
    };

#endif
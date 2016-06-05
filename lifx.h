/*
 * lifx.h - library for talking to the light blubs.
 */
#ifndef _lifx_h
    #define _lifx_h

    /* includes */
    #include "common.h"
    #include "device.h"
    #include "light.h"

    class lifx{
        public:
        	/* Members Functions */
        	lifx();
        	void setUDP(UDP &udpRef);
            void setBroadcastIP(IPAddress broadcastIP);
            void setRemotePort(uint16_t remotePort);
        	void discover();					                // Find active lights
        	void addLight(uint8_t mac[6], uint32_t port);		// Add light to Lights vector by mac address
        	void removeLight(uint8_t mac[6]);	                // Remove light from Lights vector by mac address
        	void togglePower();					                // Toggle power (on/off) of currently selected light/s
        	void toggleColor();					                // Change currently selected light/s from color to white
        	void cycleColor(float step);				                	// Change color/temperature of currently selected light/s
        	void dimLights(float step);					        // Change the brightness of currently selected light/s
        	void getStatus();                                   // Get status of lights
        	void msgIn(uint8_t packetBuffer[128]);              // UDP message in


        	/* Members */
        	std::vector<light> Lights;
            //light Lights[16];
            uint8_t numberOfLights = 0;  // The current number of lights under control

        private:
        	/* Members Functions */

        	/* Members */
        	bool _powerState;					// 1 := Lights On, 0 := Lights Off
        	device _device;
        	UDP _lifxUdp;
        	IPAddress _broadcastIP;
        	uint16_t _remotePort;


    };

#endif

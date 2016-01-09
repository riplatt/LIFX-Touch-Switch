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
        	void discover();					                // Find active lights
        	void addLight(uint8_t mac[6], uint32_t port);		// Add light to Lights vector by mac address
        	void removeLight(uint8_t mac[6]);	                // Remove light from Lights vector by mac address
        	void togglePower();					                // Toggle power (on/off) of currently selected light/s
        	void toggleColor();					                // Change currently selected light/s from color to white
        	void cycleColor();				                	// Change color/temperature of currently selected light/s
        	void dimLights();					                // Change the brightness of currently selected light/s
        	void msgIn();                                       // UDP message in
        
        
        	/* Members */
        	device Device;
            light Lights[16];
            int numberOfLights = 0;  // The current number of lights under control
        
        private:
        	/* Members Functions */
        	boolean _byteArrayCompare(byte a[], byte b[], int arraySize);
        
        	/* Members */
        	bool _powerState;					// 1 := Lights On, 0 := Lights Off
    
    
    };

#endif

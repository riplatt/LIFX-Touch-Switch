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
            void getService();
            
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
        /* Members */
        // _Udp;
        // Stream *_serial;
        
        enum _msg{
            _getService = 2,
            _stateService = 3,
            _getHostInfo = 12,
            _stateHostInfo = 13,
            _getHostFirmware = 14,
            _stateHostFirmware = 15,
            _getWifiInfo = 16,
            _stateWifiInfo = 17,
            _getWifiFirmware = 18,
            _stateWifiFirmware = 19,
            _getPower = 20,
            _setPower = 21,
            _statePower = 22,
            _getLabel = 23,
            _setLabel = 24,
            _stateLabel = 25,
            _getVersion = 32,
            _stateVersion = 33,
            _getInfo = 34,
            _stateInfo = 35,
            _acknowledgement = 45,
            _echoRequest = 58,
            _echoResponse = 59
        };
    
    };

#endif

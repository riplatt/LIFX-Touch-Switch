#ifndef _common_h
    #define _common_h
    #include <stdint.h>
    #include <stdlib.h>
    #include "application.h"
    
    #define _DEBUG 1
    
    struct _fingerPosition
    {
        uint8_t finger;
        uint16_t x;
        uint16_t y;
    };
    
    struct touchScreenEvent
    {
        uint8_t numberOfFingers;
        uint32_t timeStamp;
        struct _fingerPosition  fingerPositions[5];
    };
    
    struct HSBK  
    {
        uint16_t hue;
        uint16_t saturation;
        uint16_t brightness;
        uint16_t kelvin;
    };

#endif

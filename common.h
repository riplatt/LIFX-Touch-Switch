#ifndef _common_h
    #define _common_h
    #define _USE_MATH_DEFINES

    #include <stdint.h>
    #include <stdlib.h>
    #include <cmath>
    #include "application.h"
    #include <vector>
    #include "lifxUDP.h"

    #define _DEBUG 3
    //#define Serial if(_DEBUG)Serial

    extern uint16_t _waitingForMsg;
    extern uint32_t _myID;
    extern uint32_t _msgSentTime;
    extern bool _msgSent;

    struct _fingerPosition
    {
        uint8_t finger;

        //uint32_t fristTouchTime;
        //uint16_t fristX;
       // uint16_t fristY;

        uint16_t x;
        uint16_t y;

        //uint32_t lastTouchTime;
        //uint16_t lastX;
        //uint16_t lastY;
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

    enum _msg{
        _deviceGetService = 2,
        _deviceStateService = 3,        // Response to GetService message.
        _deviceGetHostInfo = 12,
        _deviceStateHostInfo = 13,
        _deviceGetHostFirmware = 14,
        _deviceStateHostFirmware = 15,
        _deviceGetWifiInfo = 16,
        _deviceStateWifiInfo = 17,
        _deviceGetWifiFirmware = 18,
        _deviceStateWifiFirmware = 19,
        _deviceGetPower = 20,
        _deviceDeviceSetPower = 21,
        _deviceStatePower = 22,
        _deviceGetLabel = 23,
        _deviceSetLabel = 24,
        _deviceStateLabel = 25,
        _deviceGetVersion = 32,
        _deviceStateVersion = 33,
        _deviceGetInfo = 34,
        _deviceStateInfo = 35,
        _deviceAcknowledgement = 45,
        _deviceEchoRequest = 58,
        _deviceEchoResponse = 59,
        _lightGet = 101,
        _lightSetColor = 102,
        _lightState = 107,               // Sent by a device to provide the current light state.
        _lightGetPower = 116,
        _lightSetPower = 117,            // Sent to change the light power level.
        _lightStatePower = 118
    };



#endif

/*
 * lifx.cpp - library for talking to the LIFX blubs.
 */

#include "lifx.h"

lifx::lifx()
{
    _device = device();
    _powerState = 0;
}

void lifx::setUDP(UDP &udpRef)
{
    _lifxUdp = udpRef;
    _device.setUDP(_lifxUdp);
}

void lifx::setBroadcastIP(IPAddress broadcastIP)
{
    _broadcastIP = broadcastIP;
    _device.setBroadcastIP(broadcastIP);
}

void lifx::setRemotePort(uint16_t remotePort)
{
    _remotePort = remotePort;
    _device.setRemotePort(remotePort);
}

void lifx::discover()
{
    Serial.printlnf(Time.timeStr() + " - lifx discover...");
	_device.getPower();
}

void lifx::addLight(uint8_t mac[6], uint32_t port)
{
    // Serial.printlnf("lifx addLight...");
    int i;
    bool found = false;

    for(auto &Light : Lights)
    {
        Serial.printlnf(Time.timeStr() + " - lifx addLight...Looking for MAC: 0x %02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        if (Light.matchMac(mac))
        {
            found = true;
            break;
        }
    }
    Serial.printlnf(Time.timeStr() + " - lifx addLight...Already in Vector: %s", (found ?  "true" : "false"));
    if(found == false)
    {
        light _light = light();

        Lights.push_back (_light);

        if(Lights.empty() == false)
        {
            Lights.back().setMAC(mac);
            Lights.back().setPort(port);
            Lights.back().setUDP(_lifxUdp);
            Lights.back().setBroadcastIP(_broadcastIP);
            Lights.back().setRemotePort(_remotePort);
        }
    }
    Serial.printlnf(Time.timeStr() + " - lifx addLight... Size of Vector: %d", Lights.size());
}
/*
* remove and resort array
*/
void lifx::removeLight(uint8_t mac[6])
{
    // Serial.printlnf("lifx removeLight...");
    int index = 0;
    int i;
    bool found = false;

    if(Lights.size() == 1)
    {
        Lights.clear();
    } else {
        for(auto &Light : Lights)
        {
            if (Light.matchMac(mac))
            {
                index = i;
                found = true;
                break;
            }
            index++;
        }

        if(found == true)
        {
            Lights.erase(Lights.begin() + index);
        }
    }
    Serial.printlnf(Time.timeStr() + " - lifx removedLight... Size of Vector: %d", Lights.size());
}

void lifx::togglePower()
{
    for(auto &Light : Lights)
    {
        uint16_t level = Light.getPowerLevel();
        // Serial.printlnf("lifx togglePower... Light: %d, Current Power Level %d", i, level);
        level = level == 65535 ? 0 : 65535;
        Light.setPower(level);
    }
}

void lifx::toggleColor()
{
    HSBK _hsbk;
    bool _colorMode;
    for(auto &Light : Lights)
    {
        _hsbk = Light.getHSBK();
        if(_hsbk.saturation > 0)
        {
            Light.setLastColorHSBK(_hsbk);
            _hsbk.saturation = 0;
            _colorMode = false;
        } else {
            Light.setLastWhiteHSBK(_hsbk);
            _hsbk.saturation = 65535;
            _colorMode = true;
            Light.setLastColorHSBK(_hsbk);
        }
        Light.setColorMode(_colorMode);
        Light.setColor(_hsbk);
    }
}

void lifx::cycleColor(float step)
{
    Serial.printlnf(Time.timeStr() + " - Cycle Colour, Step:%0.2f", step);
    HSBK _hsbk;
    float _hue;
    float _kelvin;
    for(auto &Light : Lights)
    {
        _hsbk = Light.getHSBK();

        if(Light.getColorMode() == true)
        {
           //0 - 65535 := 0 - 100%
           _hue = _hsbk.hue - (65535 * (step/100.00));
           // range limit
            if (_hue > 65535)
            {
                _hue = 0 + (_hue - 65535);
            } else if (_hue < 0) {
                _hue = 65535 + _hue;
            }
            _hsbk.hue = (uint16_t)_hue;
        } else {
           // 2500 - 9000
           _kelvin = _hsbk.kelvin - (6500 * (step/100.00));
           // range limit
            if (_kelvin > 9000)
            {
                _kelvin = 9000;
            } else if (_kelvin < 2500) {
                _kelvin = 2500;
            }
            _hsbk.kelvin = (uint16_t)_kelvin;
        }
        Light.setColor(_hsbk);
    }
}

void lifx::dimLights(float step)
{
    //65535
    HSBK _hsbk;
    float _brightness;
    for(auto &Light : Lights)
    {
       _hsbk = Light.getHSBK();
        //0 - 65535 := 0 - 100%
        _brightness = _hsbk.brightness - (65535 * (step/100.00));
        // range limit
        if (_brightness > 65535)
        {
            _brightness = 65535;
        } else if (_brightness < 0) {
            _brightness = 0;
        }
        // send new brightness
        _hsbk.brightness = (uint16_t)_brightness;
        Light.setColor(_hsbk);
    }
}

void lifx::getStatus()
{
    for(auto &Light : Lights)
    {
        Light.get();
    }
}

void lifx::msgIn(byte packetBuffer[128])
{
    /*
        is it a lifx msg
        is it for me (senders mac)
        Check for return type Status
        send to light status function
    */
    // LIFX Header Decode
    // Frame
    uint16_t lifxSize       = packetBuffer[0] + (packetBuffer[1] << 8); //little endian
    uint8_t lifxOrigin      = ((packetBuffer[2] + (packetBuffer[3] << 8)) & 0xC000) >> 14;
    bool lifxTagged         = ((packetBuffer[2] + (packetBuffer[3] << 8)) & 0x2000) >> 13;
    bool lifxAddressable    = ((packetBuffer[2] + (packetBuffer[3] << 8)) & 0x1000) >> 12;
    uint16_t lifxProtocol   = ((packetBuffer[2] + (packetBuffer[3] << 8)) & 0x0FFF);
    uint32_t lifxSource     = packetBuffer[4] + (packetBuffer[5] << 8) + (packetBuffer[6] << 16) + (packetBuffer[7] << 24);
    // Frame Address
    uint8_t lifxTarget[8]   = {packetBuffer[8], packetBuffer[9], packetBuffer[10], packetBuffer[11], packetBuffer[12], packetBuffer[13], packetBuffer[14], packetBuffer[15]};
    uint8_t lifxReservedA[6]= {packetBuffer[16], packetBuffer[17], packetBuffer[18], packetBuffer[19], packetBuffer[20], packetBuffer[21]};
    uint8_t lifxReservedB   = (packetBuffer[22] & 0xFC) >> 2;
    bool lifxAckRequired    = (packetBuffer[22] & 0x02) >> 1;
    bool lifxResRequired    = packetBuffer[22] & 0x01;
    uint8_t lifxSequence    = packetBuffer[23];
    // Protocol Header
    uint64_t lifxReservedC  = (packetBuffer[24] + (packetBuffer[25] << 4) + (packetBuffer[26] << 8) + (packetBuffer[27] << 12)) + ((packetBuffer[28] + (packetBuffer[29] << 4) + (packetBuffer[30] << 8) + (packetBuffer[31] << 12)) << 16);
    uint16_t lifxPacketType = packetBuffer[32] + (packetBuffer[33] << 8);
    uint16_t lifxReservedD  = packetBuffer[34] + (packetBuffer[35] << 8);

    // Serial.printlnf("   PacketType: %d", lifxPacketType);

        #if (_DEBUG > 2)
            Serial.println("Header:");
            Serial.println("  Frame:");
            Serial.printlnf("   Size: %d", lifxSize);
            Serial.printlnf("   Origin: %d", lifxOrigin);
            Serial.printlnf("   Tagged: %s", lifxTagged ? "true" : "false");
            Serial.printlnf("   Addressable: %s", lifxAddressable ? "true" : "false");
            Serial.printlnf("   Protocol: %d", lifxProtocol);
            Serial.printlnf("   Source: %d", lifxSource);
            Serial.println("  Frame Address:");
            Serial.printlnf("   Target: 0x %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", lifxTarget[0], lifxTarget[1], lifxTarget[2], lifxTarget[3], lifxTarget[4], lifxTarget[5], lifxTarget[6], lifxTarget[7]);
            Serial.printlnf("   ReservedA: 0x %02X %02X %02X %02X %02X %02X", lifxReservedA[0], lifxReservedA[1], lifxReservedA[2], lifxReservedA[3], lifxReservedA[4], lifxReservedA[5]);
            Serial.printlnf("   ReservedB: %d", lifxReservedB);
            Serial.printlnf("   AckRequired: %s", lifxAckRequired ? "true" : "false");
            Serial.printlnf("   ResRequired: %s", lifxResRequired ? "true" : "false");
            Serial.printlnf("   Sequence: %d", lifxSequence);
            Serial.println("  Protocol Header:");
            Serial.printlnf("   ReservedA: %d", lifxReservedC);
            Serial.printlnf("   PacketType: %d", lifxPacketType);
            Serial.printlnf("   ReservedD: %d", lifxReservedD);

            Serial.println("Payload:");
        #endif

        switch (lifxPacketType)
        {
            case _deviceStateService:
            {
                uint8_t _service;
                uint32_t _port;

                _service = packetBuffer[36];
                _port = packetBuffer[37] + (packetBuffer[38] << 8) + (packetBuffer[39] << 16) + (packetBuffer[40] << 24);

                #if (_DEBUG > 2)
                    Serial.printlnf("   Service: %d", _service);
                    Serial.printlnf("   Port: %d", _port);
                #endif

                _waitingForMsg = 0;
                if(lifxSource = _myID)
                {
                    // we sent the msg that this in response to
                }
                break;
            }
            case _deviceStatePower:
            {
                uint16_t _level;

                _level = packetBuffer[36] + (packetBuffer[37] << 8);

                #if (_DEBUG > 2)
                    Serial.printlnf("   Level: %d", _level);
                #endif

                _waitingForMsg = 0;
                if(lifxSource = _myID)
                {
                    uint8_t mac[6];
                    mac[0] = lifxTarget[0];
                    mac[1] = lifxTarget[1];
                    mac[2] = lifxTarget[2];
                    mac[3] = lifxTarget[3];
                    mac[4] = lifxTarget[4];
                    mac[5] = lifxTarget[5];

                    if(_level > 0)
                    {
                        addLight(mac, 56700);
                    }else {
                        removeLight(mac);
                    }
                }
                break;
            }
            case _deviceStateWifiInfo:
            {
                uint32_t _signal;
                uint32_t _tx;
                uint32_t _rx;
                uint16_t _reserved;

                _signal     =   packetBuffer[36] + (packetBuffer[37] << 8) + (packetBuffer[38] << 16) + (packetBuffer[39] << 24);
                _tx         =   packetBuffer[40] + (packetBuffer[41] << 8) + (packetBuffer[42] << 16) + (packetBuffer[43] << 24);
                _rx         =   packetBuffer[44] + (packetBuffer[45] << 8) + (packetBuffer[46] << 16) + (packetBuffer[47] << 24);
                _reserved   =   packetBuffer[48] + (packetBuffer[49] << 8);

                #if (_DEBUG > 2)
                    Serial.printlnf("   Signal: %d", _signal);
                    Serial.printlnf("   Tx: %d", _tx);
                    Serial.printlnf("   Rx: %d", _rx);
                    Serial.printlnf("   Reserved: %d", _reserved);
                #endif

                uint8_t mac[6] = {lifxTarget[0], lifxTarget[1], lifxTarget[2], lifxTarget[3], lifxTarget[4], lifxTarget[5]};

                for(auto &Light : Lights)
                {
                  if (Light.matchMac(mac))
                  {
                      Light.setSignal(_signal);
                      Light.setTx(_tx);
                      Light.setRx(_rx);
                      //Lights[i].setReserved(_reserved);
                  }
                }
                break;
            }
            case _deviceEchoResponse:
            {
                uint64_t _payload;
                _payload = (packetBuffer[36] + (packetBuffer[37] << 4) + (packetBuffer[38] << 8) + (packetBuffer[39] << 12)) + ((packetBuffer[40] + (packetBuffer[41] << 4) + (packetBuffer[42] << 8) + (packetBuffer[43] << 12)) << 16);

                #if (_DEBUG > 2)
                    Serial.printlnf("   Payload: %ll", _payload);
                #endif

                break;
            }
            case _lightStatePower:          // Sent by a device to provide the current power level.
            {
                uint16_t _level;
                uint8_t _mac[6] = {lifxTarget[0], lifxTarget[1], lifxTarget[2], lifxTarget[3], lifxTarget[4], lifxTarget[5]};
                _level = packetBuffer[36] + (packetBuffer[37] << 8);

                #if (_DEBUG > 2)
                    Serial.printlnf("   Level: %d", _level);
                #endif

                for(auto &Light : Lights)
                {
                  if (Light.matchMac(_mac))
                  {
                      Light.setPowerLevel(_level);
                  }
                }
                break;
            }
            case _lightState:               // Sent by a device to provide the current light state.
            {
                HSBK _hsbk;
                int16_t _reservedA;
                uint16_t _power;
                String _lable;
                uint64_t _reservedB;
                uint8_t _mac[6] = {lifxTarget[0], lifxTarget[1], lifxTarget[2], lifxTarget[3], lifxTarget[4], lifxTarget[5]};

                _hsbk.hue           =   packetBuffer[36] + (packetBuffer[37] << 8);
                _hsbk.saturation    =   packetBuffer[38] + (packetBuffer[39] << 8);
                _hsbk.brightness    =   packetBuffer[40] + (packetBuffer[41] << 8);
                _hsbk.kelvin        =   packetBuffer[42] + (packetBuffer[43] << 8);
                _reservedA          =   packetBuffer[44] + (packetBuffer[45] << 8);
                _power              =   packetBuffer[46] + (packetBuffer[47] << 8);
                //_lable              =   packetBuffer[48] + (packetBuffer[49] << 8) + (packetBuffer[50] << 16) + (packetBuffer[51] << 24) + (packetBuffer[52] << 32) + (packetBuffer[53] << 40) + (packetBuffer[54] << 48) + (packetBuffer[55] << 56) + (packetBuffer[56] << 64) + (packetBuffer[57] << 72) + (packetBuffer[58] << 80) + (packetBuffer[59] << 88) + (packetBuffer[60] << 96) + (packetBuffer[61] << 104) + (packetBuffer[62] << 112) + (packetBuffer[63] << 120) + (packetBuffer[64] << 128) + (packetBuffer[65] << 136) + (packetBuffer[66] << 144) + (packetBuffer[67] << 152) + (packetBuffer[68] << 160) + (packetBuffer[69] << 168) + (packetBuffer[70] << 176) + (packetBuffer[71] << 184) + (packetBuffer[72] << 192) + (packetBuffer[73] << 200) + (packetBuffer[74] << 208) + (packetBuffer[75] << 216) + (packetBuffer[76] << 224) + (packetBuffer[77] << 232) + (packetBuffer[78] << 240) + (packetBuffer[79] << 248);
                _reservedB          =	packetBuffer[80] + (packetBuffer[81] << 8) + (packetBuffer[82] << 16) + (packetBuffer[83] << 24) + (packetBuffer[84] << 32) + (packetBuffer[85] << 40) + (packetBuffer[86] << 48) + (packetBuffer[87] << 56);

                #if (_DEBUG > 2)
                    Serial.printlnf("  Raw:- Hue:%d, Saturation:%d, Brightness:%d, Kelvin:%d", _hsbk.hue, _hsbk.saturation, _hsbk.brightness, _hsbk.kelvin);
                    Serial.printlnf("  Hue: %0.2f", (float)(_hsbk.hue / (65535 / 359)));
                    Serial.printlnf("  Saturation: %0.2f", ((float)_hsbk.saturation / 65535.00) * 100.00);
                    Serial.printlnf("  Brightness: %0.2f", ((float)_hsbk.brightness / 65535.00) * 100.00);
                    Serial.printlnf("  Kelvin: %d", _hsbk.kelvin);
                    Serial.printlnf("  ReservedA: %d", _reservedA);
                    Serial.printlnf("  Power: %d", _power);
                    //Serial.printlnf("  Lable: %s", _lable);
                    Serial.printlnf("  Lable Raw: 0x %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X", packetBuffer[48], packetBuffer[49], packetBuffer[50], packetBuffer[51], packetBuffer[52], packetBuffer[53], packetBuffer[54], packetBuffer[55], packetBuffer[56], packetBuffer[57], packetBuffer[58], packetBuffer[59], packetBuffer[60], packetBuffer[61], packetBuffer[62], packetBuffer[63], packetBuffer[64], packetBuffer[65], packetBuffer[66], packetBuffer[67], packetBuffer[68], packetBuffer[69], packetBuffer[70], packetBuffer[71], packetBuffer[72], packetBuffer[73], packetBuffer[74], packetBuffer[75], packetBuffer[76], packetBuffer[77], packetBuffer[78], packetBuffer[79]);
                    Serial.printlnf("  ReservedB: %d", _reservedB);
                #endif

                for(auto &Light : Lights)
                {
                    if (Light.matchMac(_mac))
                    {
                        Light.setHSBK(_hsbk);
                        Light.setPowerLevel(_power);
                    }
                }
            }
            default:
            {
                #if (_DEBUG > 2)
                    Serial.print("Unknowen Payload:  0x ");
                    for(int i = 36; i < lifxSize; i++ ){
                        Serial.printf("%02X ", packetBuffer[i]);
                    }
                    Serial.println("");
                #endif
            }
                break;
        }
        #if (_DEBUG > 2)
            Serial.println("Raw:");
            Serial.print("  0x ");
            for(int j = 0; j < lifxSize; j++ ){
                Serial.printf("%02X ", packetBuffer[j]);
            }
            Serial.println("");

            Serial.println("---");
        #endif
}

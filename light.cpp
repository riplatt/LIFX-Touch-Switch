/*
 * light.cpp - Light half of library for talking to the LIFX blubs.
 */

#include "light.h"

light::light()
{
    _lamp = Lamp();
}

void light::setUDP(UDP &udpRef)
{
    Serial.printlnf("light setUDP...");
    _lightUdp = udpRef;
}

void light::setBroadcastIP(IPAddress broadcastIP)
{
    Serial.printlnf("light setBroadcastIP...");
    _broadcastIP = broadcastIP;   
}

void light::setRemotePort(uint16_t remotePort)
{
    Serial.printlnf("light setRemotePort...");
    _remotePort = remotePort;
}

void light::setIP(uint8_t ip[4])
{
    Serial.println("Setting IP...");
    memcpy(&_lamp.ip, &ip, sizeof(ip));
}

void light::setMAC(uint8_t mac[6])
{
    Serial.printlnf("light setMAC...");
    //memcpy(&__lamp.mac, &mac, sizeof(mac));
    _lamp.mac[0] = mac[0];
    _lamp.mac[1] = mac[1];
    _lamp.mac[2] = mac[2];
    _lamp.mac[3] = mac[3];
    _lamp.mac[4] = mac[4];
    _lamp.mac[5] = mac[5];
}

void light::setPort(uint32_t port)
{
    _lamp.port = port;
}

// Sent to obtain the light state. No payload required. Causes the device to transmit a State message
void light::get()
{
    /* header */
    Header header = Header();
    int16_t headerSize = sizeof(header);
    
    /* payload */
    // N/A
    
    /* UDP Packet */
    uint8_t udpPacket[headerSize];
    
    /* build header */
    header.size = headerSize; 
    //header.origin = 0;            
    header.tagged = 0;            
    header.addressable = 1;       
    header.protocol = 1024;       
    header.source = _myID;
    header.target[0] = _lamp.mac[0];
    header.target[1] = _lamp.mac[1];
    header.target[2] = _lamp.mac[2];
    header.target[3] = _lamp.mac[3]; 
    header.target[4] = _lamp.mac[4]; 
    header.target[5] = _lamp.mac[5]; 
    //header.target[6] = 0; 
    //header.target[7] = 0;
    //header.reservedA[0] = 0;
    //header.reservedA[1] = 0;
    //header.reservedA[2] = 0;
    //header.reservedA[3] = 0;
    //header.reservedA[4] = 0;
    //header.reservedA[5] = 0;
    //header.reservedB = 0;
    //header.ack_required = 0;
    //header.res_required = 0;
    //header.sequence = 0;
    //header.reservedC = 0;
    header.type = _lightGet;
    //header.reservedD = 0;
    
    /* build udp packet */
    /* header */
    memcpy(&udpPacket, &header, headerSize);
    /* payload */
    // N/A
    
    /* Send UDP Packet */
    // TODO
   if (WiFi.ready()) {
        Serial.printlnf("Light get - Sending UDP to 192.168.1.255:%d", _lamp.port);
        _lightUdp.beginPacket(_broadcastIP, _lamp.port);
        _lightUdp.write(udpPacket, sizeof(udpPacket));
        _lightUdp.endPacket();
        _msgSentTime = millis();
        _msgSent = true;
    }
    
    #if _DEBUG
        Serial.printf("Light setPower - UDP: 0x");
        for(uint8_t i = 0; i < sizeof(udpPacket); i++)
        {
            Serial.printf("%02x ", udpPacket[i]);
        }
        Serial.println("");
    #endif
  
}

void light::setColor(HSBK hsbk)
{
    setColor(hsbk, 0);
}

void light::setColor(HSBK hsbk, uint32_t duration)
{
    setColor(hsbk.hue, hsbk.saturation, hsbk.brightness, hsbk.kelvin, duration); 
}

void light::setColor(uint16_t hue, uint16_t saturation, uint16_t brightness, uint16_t kelvin)
{
    setColor(hue, saturation, brightness, kelvin, 0); 
}

void light::setColor(uint16_t hue, uint16_t saturation, uint16_t brightness, uint16_t kelvin, uint32_t duration)
{
    /* header */
    Header header = Header();
    int16_t headerSize = sizeof(header);
    
    /* payload */
    uint8_t payload[13];
    int16_t payloadSize = sizeof(payload);
    _lamp.hsbk.hue = hue;
    _lamp.hsbk.saturation = saturation;
    _lamp.hsbk.brightness = brightness;
    _lamp.hsbk.kelvin = kelvin;
    
    /* UDP Packet */
    uint8_t udpPacket[headerSize + payloadSize];
    
    /* build header */
    header.size = headerSize  + payloadSize; 
    //header.origin = 0;            
    header.tagged = 0;            
    header.addressable = 1;       
    header.protocol = 1024;       
    header.source = _myID;
    header.target[0] = _lamp.mac[0];
    header.target[1] = _lamp.mac[1];
    header.target[2] = _lamp.mac[2];
    header.target[3] = _lamp.mac[3]; 
    header.target[4] = _lamp.mac[4]; 
    header.target[5] = _lamp.mac[5]; 
    header.target[6] = 0; 
    header.target[7] = 0;
    //header.reservedA[0] = 0;
    //header.reservedA[1] = 0;
    //header.reservedA[2] = 0;
    //header.reservedA[3] = 0;
    //header.reservedA[4] = 0;
    //header.reservedA[5] = 0;
    //header.reservedB = 0;
    //header.ack_required = 0;
    //header.res_required = 0;
    //header.sequence = 0;
    //header.reservedC = 0;
    header.type = _lightSetColor;
    //header.reservedD = 0;
    
    /* build payload */
    /* Level, 0 - 65535 uint16_t */
    payload[0] = (_lamp.level) & 0xff;
    /* HSBK - Hue 0 - 65535 uint16_t */
    payload[1] = (_lamp.hsbk.hue) & 0xff;
    payload[2] = (_lamp.hsbk.hue >> 8) & 0xff;
    /* HSBK - Saturation 0 - 65535 uint16_t */
    payload[3] = (_lamp.hsbk.saturation) & 0xff;
    payload[4] = (_lamp.hsbk.saturation >> 8) & 0xff;
    /* HSBK - Brightness 0 - 65535 uint16_t */
    payload[5] = (_lamp.hsbk.brightness) & 0xff;
    payload[6] = (_lamp.hsbk.brightness >> 8) & 0xff;
    /* HSBK - Kelvin 2500 - 9000 uint16_t */
    payload[7] = (_lamp.hsbk.kelvin) & 0xff;
    payload[8] = (_lamp.hsbk.kelvin >> 8) & 0xff;
    /* Duration, transition time in milliseconds uint32_t */
    payload[9] = (duration) & 0xff;
    payload[10] = (duration >> 8) & 0xff;
    payload[11] = (duration >> 16) & 0xff;
    payload[12] = (duration >> 24) & 0xff;
  
  
    /* build udp packet */
    /* header */
    memcpy(&udpPacket, &header, headerSize);
    /* payload */
    for (uint8_t i = 0; i < sizeof(payload); i++)
    {
        udpPacket[headerSize + i] = payload[i];
    }
    
    /* Send UDP Packet */
    // TODO
    if (WiFi.ready()) {
        Serial.printlnf("Light setColor - Sending UDP to 192.168.1.255:%d", _lamp.port);
        _lightUdp.beginPacket(_broadcastIP, _lamp.port);
        _lightUdp.write(udpPacket, sizeof(udpPacket));
        _lightUdp.endPacket();
        _msgSentTime = millis();
        _msgSent = true;
    }
    
    #if _DEBUG
        Serial.printf("Light setPower - UDP: 0x");
        for(uint8_t i = 0; i < sizeof(udpPacket); i++)
        {
            Serial.printf("%02x ", udpPacket[i]);
        }
        Serial.println("");
    #endif

}

void light::setPower(uint16_t level)
{
    //Serial.printlnf("Light setPower to %s...", onOff ? "On" : "Off");

    setPower(level, 0);
}

void light::setPower(uint16_t level, uint32_t duration)
{
    /* header */
    Header header = Header();
    int16_t headerSize = sizeof(header);
    
    /* payload */
    uint8_t payload[6];
    int16_t payloadSize = sizeof(payload);
    _lamp.level = level;

    /* UDP Packet */
    uint8_t udpPacket[headerSize + payloadSize];
    
    /* build header */
    header.size = headerSize + payloadSize; 
    //header.origin = 0;            
    header.tagged = 0;            
    header.addressable = 1;       
    header.protocol = 1024;       
    header.source = _myID;
    header.target[0] = _lamp.mac[0];
    header.target[1] = _lamp.mac[1];
    header.target[2] = _lamp.mac[2];
    header.target[3] = _lamp.mac[3]; 
    header.target[4] = _lamp.mac[4]; 
    header.target[5] = _lamp.mac[5]; 
    //header.target[6] = 0; 
    //header.target[7] = 0;
    //header.reservedA[0] = 0;
    //header.reservedA[1] = 0;
    //header.reservedA[2] = 0;
    //header.reservedA[3] = 0;
    //header.reservedA[4] = 0;
    //header.reservedA[5] = 0;
    //header.reservedB = 0;
    //header.ack_required = 0;
    header.res_required = 1;
    //header.sequence = 0;
    //header.reservedC = 0;
    header.type = _lightSetPower;
    //header.reservedD = 0;
    
    /* build payload */
    /* Level, 0 or 65535 uint16_t */
    payload[0] = (_lamp.level) & 0xff;
    payload[1] = (_lamp.level >> 8) & 0xff;
    /* Duration, transition time in milliseconds uint32_t */
    payload[2] = (duration) & 0xff;
    payload[3] = (duration >> 8) & 0xff;
    payload[4] = (duration >> 16) & 0xff;
    payload[5] = (duration >> 24) & 0xff;
    
    /* build udp packet */
    /* header */
    memcpy(&udpPacket, &header, headerSize);
    /* payload */
    for (uint8_t i = 0; i < payloadSize; i++)
    {
        udpPacket[headerSize + i] = payload[i];
    }
    
    /* Send UDP Packet */
    // TODO
    if (WiFi.ready()) {
        Serial.printlnf("Light setPower - Sending UDP to 192.168.1.255:%d", _lamp.port);
        _lightUdp.beginPacket(_broadcastIP, _lamp.port);
        _lightUdp.write(udpPacket, sizeof(udpPacket));
        _lightUdp.endPacket();
        _msgSentTime = millis();
        _msgSent = true;
    }

    #if _DEBUG > 2
        Serial.printf("Light setPower - UDP: 0x");
        for(uint8_t i = 0; i < sizeof(udpPacket); i++)
        {
            Serial.printf("%02X ", udpPacket[i]);
        }
        Serial.println("");
    #endif
}

// Switch Functions
bool light::matchMac(byte mac[6])
{
    for (int i = 0; i < 5; i++)
    {
        if (_lamp.mac[i] != mac[i])
        {
            return(false);
        }
    }
    return(true);
}

void light::setPowerLevel(uint16_t powerLevel)
{
    _lamp.level = powerLevel;
}

uint16_t light::getPowerLevel()
{
    return _lamp.level;
}

void light::setHSBK(HSBK hsbk)
{
    _lamp.hsbk.hue = hsbk.hue;
    _lamp.hsbk.saturation = hsbk.saturation;
    _lamp.hsbk.brightness = hsbk.brightness;
    _lamp.hsbk.kelvin = hsbk.kelvin;
}

HSBK light::getHSBK()
{
    return _lamp.hsbk;
}

void light::setSignal(float signal)
{
    _lamp.signal = signal;         
}

float light::getSignal()
{
    return _lamp.signal;
}

void light::setTx(uint32_t tx)
{
    _lamp.tx = tx;         
}

uint32_t light::getTx()
{
    return _lamp.tx;
}

void light::setRx(uint32_t rx)
{
    _lamp.rx = rx;         
}

uint32_t light::getRx()
{
    return _lamp.rx;
}
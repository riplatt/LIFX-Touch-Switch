/*
 * light.cpp - Light half of library for talking to the LIFX blubs.
 */

#include "light.h"

light::light()
{
    /* debug serial interface */
    //lamp _lamp = lamp();

};

void light::setIP(uint8_t ip[4])
{
    Serial.println("Setting IP...");
    memcpy(&lamp.ip, &ip, sizeof(ip));
}

void light::setMAC(uint8_t mac[6])
{
    Serial.printlnf("light setMAC...");
    //memcpy(&_lamp.mac, &mac, sizeof(mac));
    lamp.mac[0] = mac[0];
    lamp.mac[1] = mac[1];
    lamp.mac[2] = mac[2];
    lamp.mac[3] = mac[3];
    lamp.mac[4] = mac[4];
    lamp.mac[5] = mac[5];
}

void light::setPort(uint32_t port)
{
    lamp.port = port;
}

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
    //header.source = 0;
    header.target[0] = lamp.mac[0];
    header.target[1] = lamp.mac[1];
    header.target[2] = lamp.mac[2];
    header.target[3] = lamp.mac[3]; 
    header.target[4] = lamp.mac[4]; 
    header.target[5] = lamp.mac[5]; 
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
    header.type = _get;
    //header.reservedD = 0;
    
    /* build udp packet */
    /* header */
    memcpy(&udpPacket, &header, headerSize);
    /* payload */
    // N/A
    
    /* Send UDP Packet */
    // TODO
    #if _DEBUG > 0
        Serial.printf("Light Get - UDP: ");
        for(int i = 0; i < sizeof(udpPacket); i++)
        {
            Serial.printf("0x%02x ", udpPacket[i]);
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
    lamp.hsbk.hue = hue;
    lamp.hsbk.saturation = saturation;
    lamp.hsbk.brightness = brightness;
    lamp.hsbk.kelvin = kelvin;
    
    /* UDP Packet */
    uint8_t udpPacket[headerSize + payloadSize];
    
    /* build header */
    header.size = headerSize  + payloadSize; 
    //header.origin = 0;            
    header.tagged = 0;            
    header.addressable = 1;       
    header.protocol = 1024;       
    //header.source = 0;
    header.target[0] = lamp.mac[0];
    header.target[1] = lamp.mac[1];
    header.target[2] = lamp.mac[2];
    header.target[3] = lamp.mac[3]; 
    header.target[4] = lamp.mac[4]; 
    header.target[5] = lamp.mac[5]; 
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
    header.type = _setColor;
    //header.reservedD = 0;
    
    /* build payload */
    /* Level, 0 - 65535 uint16_t */
    payload[0] = (lamp.level) & 0xff;
    /* HSBK - Hue 0 - 65535 uint16_t */
    payload[1] = (lamp.hsbk.hue) & 0xff;
    payload[2] = (lamp.hsbk.hue >> 8) & 0xff;
    /* HSBK - Saturation 0 - 65535 uint16_t */
    payload[3] = (lamp.hsbk.saturation) & 0xff;
    payload[4] = (lamp.hsbk.saturation >> 8) & 0xff;
    /* HSBK - Brightness 0 - 65535 uint16_t */
    payload[5] = (lamp.hsbk.brightness) & 0xff;
    payload[6] = (lamp.hsbk.brightness >> 8) & 0xff;
    /* HSBK - Kelvin 2500 - 9000 uint16_t */
    payload[7] = (lamp.hsbk.kelvin) & 0xff;
    payload[8] = (lamp.hsbk.kelvin >> 8) & 0xff;
    /* Duration, transition time in milliseconds uint32_t */
    payload[9] = (duration) & 0xff;
    payload[10] = (duration >> 8) & 0xff;
    payload[11] = (duration >> 16) & 0xff;
    payload[12] = (duration >> 24) & 0xff;
  
  
    /* build udp packet */
    /* header */
    memcpy(&udpPacket, &header, headerSize);
    /* payload */
    for (int i = 0; i < sizeof(payload); i++)
    {
        udpPacket[headerSize + i] = payload[i];
    }
    
    /* Send UDP Packet */
    // TODO
    #if _DEBUG > 0
        Serial.printf("Light setColor - UDP: ");
        for(int i = 0; i < sizeof(udpPacket); i++)
        {
            Serial.printf("0x%02x ", udpPacket[i]);
        }
        Serial.println("");
    #endif

}

void light::setPower(bool onOff)
{
    Serial.println("Light setPower...");

    setPower(onOff, 0);
}

void light::setPower(bool onOff, uint32_t duration)
{
    /* header */
    Header header = Header();
    int16_t headerSize = sizeof(header);
    
    /* payload */
    uint8_t payload[6];
    int16_t payloadSize = sizeof(payload);
    if (onOff)
    {
        lamp.level = 65535;
    } else {
        lamp.level = 0;
    }
    
    /* UDP Packet */
    uint8_t udpPacket[headerSize + payloadSize];
    
    /* build header */
    header.size = headerSize + payloadSize; 
    //header.origin = 0;            
    header.tagged = 0;            
    header.addressable = 1;       
    header.protocol = 1024;       
    //header.source = 0;
    header.target[0] = lamp.mac[0];
    header.target[1] = lamp.mac[1];
    header.target[2] = lamp.mac[2];
    header.target[3] = lamp.mac[3]; 
    header.target[4] = lamp.mac[4]; 
    header.target[5] = lamp.mac[5]; 
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
    header.type = _setPower;
    //header.reservedD = 0;
    
    /* build payload */
    /* Level, 0 or 65535 uint16_t */
    payload[0] = (lamp.level) & 0xff;
    payload[1] = (lamp.level >> 8) & 0xff;
    /* Duration, transition time in milliseconds uint32_t */
    payload[2] = (duration) & 0xff;
    payload[3] = (duration >> 8) & 0xff;
    payload[4] = (duration >> 16) & 0xff;
    payload[5] = (duration >> 24) & 0xff;
    
    /* build udp packet */
    /* header */
    memcpy(&udpPacket, &header, headerSize);
    /* payload */
    for (int i = 0; i < payloadSize; i++)
    {
        udpPacket[headerSize + i] = payload[i];
    }
    
    /* Send UDP Packet */
    // TODO
    #if _DEBUG > 0
        Serial.printf("Light setPower - UDP: ");
        for(int i = 0; i < sizeof(udpPacket); i++)
        {
            Serial.printf("0x%02x ", udpPacket[i]);
        }
        Serial.println("");
    #endif
};

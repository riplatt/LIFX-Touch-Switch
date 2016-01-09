/*
 *  device.cpp - Device half of library for talking to the LIFX blubs.
 */
#include "device.h"

device::device()
{
    /* debug serial interface */
};

void device::getService()
{
    /* header */
    Header header = Header();
    int headerSize = sizeof(header);
    
    /* payload */
    // N/A
    
    /* UDP Packet */
    uint8_t udpPacket[headerSize];
    
    /* build header */
    header.size = headerSize; 
    //header.origin = 0;            
    header.tagged = 1;            
    header.addressable = 1;       
    header.protocol = 1024;       
    //header.source = 0;
    //header.target[0] = 0;
    //header.target[1] = 0;
    //header.target[2] = 0;
    //header.target[3] = 0; 
    //header.target[4] = 0; 
    //header.target[5] = 0; 
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
    header.type = _getService;
    //header.reservedD = 0;
    
    /* build payload */
    // N/A
    
    /* build udp packet */
    memcpy(&udpPacket, &header, headerSize);
    
    /* Send UDP Packet */
    // TODO
    #if DEBUG > 0
        Serial.printf("Deivce getService - UDP: ")
        for(int i = 0; i < sizeof(udpPacket); i++)
        {
            Serial.printf("0x%02x ", udpPacket[i]);
        }
        Serial.println(";
    #endif

};


/*
 *  device.cpp - Device half of library for talking to the LIFX blubs.
 */
#include "device.h"

device::device()
{

};

void device::setUDP(lifxUDP *udpRef)
{
    // Serial.printlnf("device setUDP...");
    _deviceUdp = udpRef;
}

void device::setBroadcastIP(IPAddress broadcastIP)
{
    // Serial.printlnf("device setBroadcastIP...");
    _broadcastIP = broadcastIP;
}
void device::setRemotePort(uint16_t remotePort)
{
    // Serial.printlnf("device setRemotePort...");
    _remotePort = remotePort;
}

void device::getService()
{
    // Serial.printlnf("device getService...");
    //_waitingForMsg = _deviceStateService;
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
    header.source = _myID;
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
    header.type = _deviceGetService;
    //header.reservedD = 0;

    /* build payload */
    // N/A

    /* build udp packet */
    memcpy(&udpPacket, &header, headerSize);

    /* Send UDP Packet */
    // TODO
    std::vector<byte> data(udpPacket, udpPacket + sizeof(udpPacket));
    _deviceUdp->add(data);
    Serial.printlnf(Time.timeStr() + ":" + millis() + " - Deivce getService - udpPacket size: %d", sizeof(udpPacket));

    #if _DEBUG
        Serial.printf(Time.timeStr() + ":" + millis() + " - Deivce getService - UDP: 0x");
        for(uint8_t i = 0; i < sizeof(udpPacket); i++ )
        {
            Serial.printf("%02x ", udpPacket[i]);
        }
        Serial.println("");
    #endif

};

void device::getPower()
{
    Serial.printlnf(Time.timeStr() + ":" + millis() + " - Device getPower...");
    //_waitingForMsg = _deviceStatePower;
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
    header.source = _myID;
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
    header.type = _deviceGetPower;
    //header.reservedD = 0;

    /* build payload */
    // N/A

    /* build udp packet */
    memcpy(&udpPacket, &header, headerSize);

    /* Send UDP Packet */
    std::vector<byte> data(udpPacket, udpPacket + sizeof(udpPacket));
    _deviceUdp->add(data);
    //_deviceUdp.add(udpPacket);
    Serial.printlnf(Time.timeStr() + ":" + millis() + " - Deivce getPower - udpPacket size: %d", sizeof(udpPacket));

    #if _DEBUG
        Serial.printf(Time.timeStr() + ":" + millis() + " - Deivce getPower - UDP: 0x");
        for(uint8_t i = 0; i < sizeof(udpPacket); i++ )
        {
            Serial.printf("%02x ", udpPacket[i]);
        }
        Serial.println("");
    #endif

}

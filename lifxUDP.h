/*
 * lifxUDP.h - library for multitasking sending of UDP packest.
 */
#ifndef _lifxUDP_h
  #define _lifxUDP_h
  // includes
  #include "common.h"

  class lifxUDP{
    public:
      /* Members Functions */
      lifxUDP();
      void initialise(IPAddress broadcastIP, uint32_t remotePort);
      void add(std::vector<byte> udpPacket);
      void send();
      bool available();
      void begin(int port);
      int parsePacket();
      int read(byte* packetBuffer, int maxSize);
      IPAddress remoteIP();
      int remotePort();
      void flush();

      /* Members */

    private:
      /* Members Functions */
      void _reconnect();
      /* Members */
      uint32_t _remotePort;
      IPAddress _broadcastIP;
    	std::vector<std::vector<byte>> _udpPackets;
    	UDP _myUdp;
  };
#endif

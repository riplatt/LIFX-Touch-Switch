/*
 * lifxUDP.cpp - library for multitasking sending of UDP packest.
 */

#include "lifxUDP.h"

lifxUDP::lifxUDP()
{

}

void lifxUDP::initialise(IPAddress broadcastIP, uint32_t remotePort)
{
  _remotePort = remotePort;
  _broadcastIP = broadcastIP;
  _myUdp.begin(_remotePort);
}

void lifxUDP::add(std::vector<byte> udpPacket)
{
	_udpPackets.push_back(udpPacket);
  Serial.printlnf(Time.timeStr() + ":" + millis() + " - lifxUDP add - Message added, vector size: %d, udpPacket size: %d", _udpPackets.size(), sizeof(udpPacket));

}

void lifxUDP::send()
{
  Serial.printlnf(Time.timeStr() + ":" + millis() + " - lifxUDP send - Sending...");
  if (WiFi.ready())
  {
    for(auto &_vPacket : _udpPackets)
    {
      int _sent = _myUdp.sendPacket(&_vPacket[0], _vPacket.size(), _broadcastIP, _remotePort);
      if( _sent < 0)
      {
        Serial.printlnf(Time.timeStr() + ":" + millis() + " - lifxUDP send - Error: Can't send UDP Code:%d", _sent);
        _reconnect();
      } else {
        Serial.printlnf(Time.timeStr() + ":" + millis() + " - lifxUDP send - %d sent...", _sent);
      }
    }
    _udpPackets.clear();
  } else {
    Serial.printlnf(Time.timeStr() + ":" + millis() + " - lifxUDP send - Error: WiFi is down...");
  }
}

void lifxUDP::begin(int port)
{
  _myUdp.begin(port);
}

int lifxUDP::parsePacket()
{
  return _myUdp.parsePacket();
}

int lifxUDP::read(byte* packetBuffer, int maxSize)
{
  return _myUdp.read(packetBuffer, maxSize);
}

IPAddress lifxUDP::remoteIP()
{
  return _myUdp.remoteIP();
}

int lifxUDP::remotePort()
{
  return _myUdp.remotePort();
}

void lifxUDP::flush()
{
  _myUdp.flush();
}

void lifxUDP::_reconnect()
{
  int _status;
  _myUdp.stop();
  delay(5000);
  _status = _myUdp.begin(_remotePort);
  Serial.printlnf(Time.timeStr() + ":" + millis() + " - lifxUDP _reconnect - Begin Status Code:%d", _status);
}

bool lifxUDP::available()
{
  //Serial.printlnf(Time.timeStr() + ":" + millis() + " - lifxUDP available - Available: %d", _udpPackets.size());
  if(_udpPackets.size() > 0)
  {
    return true;
  }
  return false;
}

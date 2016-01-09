/*
 * lifx.cpp - library for talking to the LIFX blubs.
 */

#include "lifx.h"

lifx::lifx()
{
	device Device = device();
}

void lifx::discover()
{
	Device.getService();
}

void lifx::addLight(uint8_t mac[6], uint32_t port)
{
    Serial.printlnf("lifx addLight...");
    light Light = light();
    Light.setMAC(mac);
    Light.setPort(port);
    
    Lights[numberOfLights] = Light;
    
    numberOfLights++;
}
/*
* remove and resort array
*/
void lifx::removeLight(uint8_t mac[6])
{
  int index;
  int i;

  for (i = 0; i < numberOfLights - 1; i++)
  {
    if (_byteArrayCompare(Lights[i].lamp.mac, mac, 6))
    {
      index = i;
    }
  }

  for (i = index - 1; i < numberOfLights - 1; i++)
  {
    Lights[i] = Lights[i + 1];
  }
  numberOfLights--;
}

void lifx::togglePower()
{
    Serial.printlnf("lifx togglePower...Number of Lights: %d", numberOfLights);
    _powerState = !_powerState;
    
    for(int i = 0; i < numberOfLights; i++)
    {
        Lights[i].setPower(_powerState);
    }
}

void lifx::toggleColor()
{

}

void lifx::cycleColor()
{

}

void lifx::dimLights()
{

}

void lifx::msgIn()
{
  /*
  is it a lifx msg
  is it for me (senders mac)
  Check for return type Status
  send to light status function

  */
}

boolean lifx::_byteArrayCompare(byte a[], byte b[], int arraySize)
{
  for (int i = 0; i < arraySize; i++)
  {
    if (a[i] != b[i])
    {
      return(false);
    }
  }
  return(true);
}

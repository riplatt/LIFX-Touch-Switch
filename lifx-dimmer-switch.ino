#include "common.h"
#include "button.h"
#include "lifx.h"

void funcEncoder();
//void funcEncoderA();
//void funcEncoderB();
//void encoderARising();
//void encoderAFalling();
//void encoderBRising();
//void encoderBFalling();

int _buttonA = D4;
int _buttonB = D1;
int _encoderPinA = D2;
int _encoderPinB = D3;

bool btnValueA = true;
bool btnValueB = true;
int _encoderValueA = 0;
int _encoderValueB = 0;

volatile bool buttonLastA = false;
volatile bool buttonLastB = false;
volatile bool _encoderLastA = false;
volatile bool _encoderLastB = false;
volatile int _encoderPos = 0;
volatile unsigned long _usEncoder;

//int prevPos = 0;
//int value = 0;
int _state = 0;
bool _msgSent = false;
bool _msgAction = false;
unsigned long _now = 0;
String _systemID;
uint32_t _myID = 7777;
bool _dimmMode = true;

//UDP Settings
IPAddress _broadcastIP;
int _remotePort = 56700;
uint16_t _udpPacketSize = 0;
uint32_t lastMsgTime = 0;
uint32_t _lastModeTime = 0;
uint32_t _lastEncoderTime = 0;
uint32_t _encoderSpeed = 0;
float _encoderPercent;
float _encoderSpeedMultiplier;

button btnA = button();
button btnB = button();
lifx LIFX = lifx();
lifxUDP _lifxUDP = lifxUDP();

void setup() {
  Time.zone(10.00);
  Time.setFormat(TIME_FORMAT_ISO8601_FULL);

  Serial.begin(115200);
  delay(1000);

  IPAddress _myIP = WiFi.localIP();
  Serial.printlnf(Time.timeStr() + ":" + millis() + " - My IP:%d.%d.%d.%d", _myIP[0], _myIP[1], _myIP[2], _myIP[3]);

  _systemID = System.deviceID();
  // Serial.print("System ID: ");
  // Serial.println(_systemID);
  Serial.printlnf(Time.timeStr() + ":" + millis() + " - EEPROM Length: %d", EEPROM.length());
  Serial.printlnf(Time.timeStr() + ":" + millis() + " - System version: %s", System.version().c_str());

  _myID = (_myIP[3] & 0x000000FF);
  // Serial.printlnf("My ID: %lu", (unsigned long)_myID);

  _broadcastIP = IPAddress(_myIP[0], _myIP[1], _myIP[2], 255);

  _lifxUDP.initialise(_broadcastIP, _remotePort);

  // Setup the LIFX object
  LIFX.setUDP(&_lifxUDP);
  LIFX.setBroadcastIP(_broadcastIP);
  LIFX.setRemotePort(_remotePort);
  LIFX.getStatus();

  // Pin Modes
  pinMode(_buttonA, INPUT_PULLDOWN);
  pinMode(_buttonB, INPUT_PULLDOWN);
  pinMode(_encoderPinA, INPUT);
  pinMode(_encoderPinB, INPUT);
  // Interrupts
  attachInterrupt(_encoderPinA, funcEncoder, CHANGE);
  //attachInterrupt(encoderA, funcEncoderA, CHANGE);
  //attachInterrupt(encoderB, funcEncoderB, CHANGE);
  //attachInterrupt(encoderA, encoderARising, RISING);
  //attachInterrupt(encoderB, encoderBRising, RISING);
}

void loop() {
  _now = millis();

  // check to see if there are UDP commands to Send
  if(_lifxUDP.available() == true)
  {
    //Serial.printlnf(Time.timeStr() + ":" + millis() + " - UDP message available for sending...");
    _lifxUDP.send();
    if(_msgAction == true)
    {
      _msgSent = true;
      _msgAction = false;
    }
    lastMsgTime = _now;
  }
  // Read buttons
  if( digitalRead(_buttonA) != buttonLastA ) {
    buttonLastA = !buttonLastA;
    btnValueA = !digitalRead(_buttonA);
    Serial.printlnf("Button A: %s",(btnValueA ? "True" : "False"));
  }

  if( digitalRead(_buttonB) != buttonLastB ) {
   buttonLastB = !buttonLastB;
   btnValueB = !digitalRead(_buttonB);
   Serial.printlnf("Button B: %s",(btnValueB ? "True" : "False"));
  }
  // Check for actions
  btnA.check(btnValueA, _now);
  // Switch On/Off
  if (btnA.getClick())
  {
    Serial.println("Button A Clicked...");
    LIFX.togglePower();
    btnA.setClick(false);
    _msgAction = true;
  }

  if (btnA.getDoubleClick())
  {
    Serial.println("Button A Double Clicked...");
    btnA.setDoubleClick(false);
  }

  if (btnA.getHold())
  {
    Serial.println("Button A Held...");
    LIFX.discover();
    btnA.setHold(false);
    _msgAction = true;
  }

  if (btnA.getLongHold())
  {
    Serial.println("Button A Held Longer...");
    btnA.setLongHold(false);
  }

  btnB.check(btnValueB, _now);

  if (btnB.getClick())
  {
    Serial.println("Button B Clicked...");
    _dimmMode = !_dimmMode;
    btnB.setClick(false);
    _lastModeTime = _now;
  }

  if (btnB.getDoubleClick())
  {
    Serial.println("Button B Double Clicked...");
    btnB.setDoubleClick(false);
  }
  // Toggle between colour and white
  if (btnB.getHold())
  {
    Serial.println("Button B Held...");
    LIFX.toggleColor();
    btnB.setHold(false);
    _msgAction = true;
  }

  if (btnB.getLongHold())
  {
    Serial.println("Button B Held Longer...");
    btnB.setLongHold(false);
  }

  // Mode fall back
  if((_now - _lastModeTime > 10000) && _dimmMode == false)
  {
    _dimmMode = true;
  }

  // Encodeer
  if(_encoderPercent != 0.00){
    if(_dimmMode == true)
    {
      LIFX.dimLights(_encoderPercent * 10.00);
    } else {
      LIFX.cycleColor(_encoderPercent * 10.00);
    }
    _lastEncoderTime = _now;
    _lastModeTime = _now;
    _encoderPercent = 0;
  }
  // get lamp status
  if(((_now - lastMsgTime > 500) && (_msgSent == true)) || (_now - lastMsgTime > 30000))
  {
    Serial.printlnf(Time.timeStr() + ":" + millis() + " - Getting status...");
    LIFX.getStatus();
    _msgSent = false;
  }
  //
  // Check if data has been received
  _udpPacketSize = _lifxUDP.parsePacket();
  if (_udpPacketSize > 0)
  {
    Serial.printlnf(Time.timeStr() + ":" + millis() + " - UDP packet size: %d", _udpPacketSize);
    byte _packetBuffer[128]; //buffer to hold incoming packet

    // Read first 128 of data received
    _lifxUDP.read(_packetBuffer, 128);

    // Ignore other chars
    _lifxUDP.flush();

    // Store sender ip and port
    IPAddress senderIP = _lifxUDP.remoteIP();
    int port = _lifxUDP.remotePort();
    // Serial.printlnf("%lu - IP:%d.%d.%d.%d:%d", now, senderIP[0], senderIP[1], senderIP[2], senderIP[3], port);

    // translate data
    LIFX.msgIn(_packetBuffer);
    // Set last time we saw a msg
    lastMsgTime = _now;
  }
}
//----------------------------------------------------------------
void funcEncoder()
{
  int _incrementor = 0;
  unsigned long _usEncoderDelta = 0;
  _encoderValueA = digitalRead(_encoderPinA);
  _encoderValueB = digitalRead(_encoderPinB);

  if(_encoderValueA != _encoderLastA)
  {
    int s = _state & 3;
		if (_encoderValueA) s |= 4;
		if (_encoderValueB) s |= 8;
		switch (s) {
			case 0: case 5: case 10: case 15:
				break;
			case 1: case 7: case 8: case 14:
				_incrementor = 2; break;
			case 2: case 4: case 11: case 13:
				_incrementor = -2; break;
			case 3: case 12:
				_incrementor = 1; break;
			default:
				_incrementor = -1; break;
		}
		_state = (s >> 2);

    _usEncoderDelta = millis() - _usEncoder;
    _usEncoderDelta = 2000 - _usEncoderDelta;
    constrain(_usEncoderDelta, 1500, 2000);
    //_usEncoderDelta = min(_usEncoderDelta, 2000); //2 Seconds
    //_usEncoderDelta = 2000 - _usEncoderDelta;
    //_usEncoderDelta = max(_usEncoderDelta, 1);
    _encoderSpeed += _usEncoderDelta * _incrementor;
    _encoderPercent = ((float)_usEncoderDelta - 1500.00) / 500.00;

    if (abs(_incrementor) == 1 && _encoderPercent >= 0.00 && _encoderPercent <= 1.00) {
      _encoderPercent = _encoderPercent * _incrementor;
      //Serial.printlnf(Time.timeStr() + ":" + millis() + " - Encoder Change - A: %s,\tB: %s, \tincrementor: %d, \tPercent: %.4f, \tValue: %d,\tTime: " + _usEncoderDelta, _encoderValueA ? "high" : "low", _encoderValueB ? "high" : "low", _incrementor, _encoderPercent, _encoderSpeed);
    } else {
      _encoderPercent = 0.00;
    }
    _usEncoder = millis();
    _encoderLastA = _encoderValueA;
    _encoderSpeed = 0;
  }
}

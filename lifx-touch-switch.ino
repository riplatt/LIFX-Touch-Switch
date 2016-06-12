// This #include statement was automatically added by the Particle IDE.
#include "stateEngine.h"

#include "common.h"
#include "gsl1680.h"
//#include "GestureRecognition.h"
#include "stateEngine.h"
#include "lifx.h"


int WAKE = D4;
int INTRPT = D2;
int LED = D7;

String myID;
uint16_t _waitingForMsg = 0;
uint32_t _myID = 69;
bool _msgSent = false;
uint32_t _msgSentTime = 0;
bool led = false;
bool Interrupt = false;
bool colorToggled = false;
uint32_t now;
uint32_t lastMsgTime = 0;
uint32_t lastInterrupt = 0;
uint32_t lastGetService = 0;
uint32_t lastStatus = 0;
uint32_t lastStep = 0;
/* UDP */
uint16_t localPort = 56701;
uint16_t remotePort = 56700;
uint16_t udpPacketSize = 0;
IPAddress broadcastIP;
// Objects
//GestureRecognition GR = GestureRecognition();
stateEngine SE = stateEngine();
gsl1680 GSL = gsl1680();
lifx LIFX;
UDP _udp;

void setup() {

    // Time setup for serial print
   Time.zone(10.00);
   Time.setFormat(TIME_FORMAT_ISO8601_FULL);

    Serial.begin(115200);
    delay(1000);

    IPAddress myIP = WiFi.localIP();
    Serial.printlnf(Time.timeStr() + "My IP:%d.%d.%d.%d", myIP[0], myIP[1], myIP[2], myIP[3]);

    myID = System.deviceID();
    // Serial.print("My ID: ");
    // Serial.println(myID);
    Serial.printlnf("EEPROM Length: %d", EEPROM.length());

    _myID = (myIP[3] & 0x000000FF);
    // Serial.printlnf("My ID: %lu", (unsigned long)_myID);

    broadcastIP = IPAddress(myIP[0], myIP[1], myIP[2], 255);

    Serial.printlnf(Time.timeStr() + " - Get Set...");
    // Setup IO
    pinMode(LED, OUTPUT);
    pinMode(WAKE, OUTPUT);
    pinMode(INTRPT, INPUT_PULLUP);

    digitalWrite(LED, HIGH);
    // Initialise Objects
    GSL.initialise(WAKE);
    //GR.initialise();
    SE.initialise();
    // Serial.printlnf("Broadcast IP:%d.%d.%d.%d", broadcastIP[0], broadcastIP[1], broadcastIP[2], broadcastIP[3]);
    // Start UDP
    _udp.begin(remotePort);
    _udp.joinMulticast(broadcastIP);

    // Setup the LIFX object
    LIFX = lifx();
    LIFX.setUDP(_udp);
    LIFX.setBroadcastIP(broadcastIP);
    LIFX.setRemotePort(remotePort);

    Serial.printlnf(Time.timeStr() + " - Go...");
    digitalWrite(LED, LOW);
    //get status on start up.
    LIFX.getStatus();
}

void loop() {
    now = millis();

    // check wifi is up
    if (WiFi.ready() == false)
    {
        _udp.begin(remotePort);
        _udp.joinMulticast(broadcastIP);
    }

    //update state engine
    if(now - SE.lastUpdate > 100)
    {
        SE.update(now);
    }

    // Status Updates
    if((now - lastStatus >= 30000) || ((_msgSent == true) && (now - _msgSentTime >= 1000)))
    {
        // Serial.printlnf("%lu - Status update...", now);
        LIFX.getStatus();
        lastStatus = now;
        _msgSent = false;
    }

    // ---- Actions ---- //
    // Check for claiming
    if (SE.getClaim() == true)
    {
        LIFX.discover();
        if (now - lastMsgTime > 1000)
        {
            SE.setClaim(false);
        }
    }

    // Check for tap
    if (SE.getTap())
    {
        // Tap action waiting
        LIFX.togglePower();
        SE.setTap(false);
    }

    // check for double tap
    if (SE.getDoubleTap())
    {
        SE.setDoubleTap(false);
    }

    // check for Moving in the Y axis
    if(SE.getMovingYAxis())
    {
        if(now - lastStep >= 50)
        {
            LIFX.dimLights(SE.getDeltaYPercent());
            lastStep = now;
        }
    }

    // check for Moving in the X axis
    if((SE.getMovingXAxis() == true) && (colorToggled == false))
    {
        LIFX.toggleColor();
        colorToggled = true;
    } else if ((SE.getMovingXAxis() == false) && (colorToggled == true)) {
        colorToggled = false;
    }

    // check for 2 fingers Moving in the Y axis
    if(SE.getMovingYAxis2())
    {
        if(now - lastStep >= 50)
        {
            LIFX.cycleColor(SE.getDeltaYPercent());
            lastStep = now;
        }
    }
    // ---- End Actions ---- //

    // Check if data has been received
    udpPacketSize = _udp.parsePacket();
    if (udpPacketSize > 0)
    {
        byte packetBuffer[128]; //buffer to hold incoming packet

        // Read first 128 of data received
        _udp.read(packetBuffer, 128);

        // Ignore other chars
        _udp.flush();

        // Store sender ip and port
        IPAddress senderIP = _udp.remoteIP();
        int port = _udp.remotePort();
        // Serial.printlnf("%lu - IP:%d.%d.%d.%d:%d", now, senderIP[0], senderIP[1], senderIP[2], senderIP[3], port);

        // translate data
        LIFX.msgIn(packetBuffer);
        // Set last time we saw a msg
        lastMsgTime = now;
    }

    // Check for touchscreen input data
    if(digitalRead(INTRPT) == HIGH)
    {

        led = !led; //flick led
        digitalWrite(LED, led);

        bool success = GSL.readData();
        if (success == 1)
        {
            int fCount = GSL.event.numberOfFingers;

            if(fCount > 0) {
                // Serial.printlnf("%lu - Touch%d:    ", now, fCount);
            } else {
                // Serial.printlnf("%lu - High Release: ", now);
            }
            Interrupt = true;
            lastInterrupt = now;
            SE.addEvent(GSL.event);
        } else {
            // Serial.println("Interrupt High: Failed Read...");
        }
    } else {
        if(Interrupt == true && (now - 100 > lastInterrupt)) {

            bool success = GSL.readData();
            if (success == 1)
            {
                int fCount = GSL.event.numberOfFingers;

                if(fCount > 0) {
                    Interrupt = true;
                    lastInterrupt = now;
                } else {
                    Interrupt = false;
                }
                // Serial.printlnf("%lu - Low Release: ", now);
                SE.addEvent(GSL.event);
            } else {
                // Serial.println("Interrupt Low: Failed Read...");
                Interrupt = true;
                lastInterrupt = now;
            }
        }
    }
}

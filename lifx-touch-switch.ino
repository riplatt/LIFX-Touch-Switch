#include "common.h"
#include "gsl1680.h"
#include "GestureRecognition.h"
#include "lifx.h"


int WAKE = D4;
int INTRPT = D2;
int LED = D7;

bool led = false;
bool Interrupt = false;
uint32_t now;
uint32_t lastInterrupt = 0;
uint32_t lastGetService = 0;
uint32_t lastStatus = 0;
/* UDP */
uint16_t localPort = 56701;
uint16_t remotePort = 56700;
uint16_t udpPacketSize = 0;
IPAddress broadcastIP;
// Objects
GestureRecognition GR = GestureRecognition();
gsl1680 GSL = gsl1680();
lifx LIFX;
UDP _udp;


void setup() {

    Serial.begin(115200);
    delay(1000);
    
    IPAddress myIP = WiFi.localIP();
    Serial.printlnf("My IP:%d.%d.%d.%d", myIP[0], myIP[1], myIP[2], myIP[3]);
    
    broadcastIP = IPAddress(myIP[0], myIP[1], myIP[2], 255);
   
    Serial.println("Get Set...");
    // Setup IO
    pinMode(LED, OUTPUT);
    pinMode(WAKE, OUTPUT);
    pinMode(INTRPT, INPUT_PULLUP);
    
    digitalWrite(LED, HIGH);
    // Initialise Objects
    GSL.initialise(WAKE);
    GR.initialise();
    Serial.printlnf("Broadcast IP:%d.%d.%d.%d", broadcastIP[0], broadcastIP[1], broadcastIP[2], broadcastIP[3]);
     // Start UDP
    _udp.begin(remotePort);
    _udp.joinMulticast(broadcastIP);
    
    // Setup the LIFX object
    LIFX = lifx();
    LIFX.setUDP(_udp);
    LIFX.setBroadcastIP(broadcastIP);
    LIFX.setRemotePort(remotePort);
    
    //Setup Test Lamp
    uint8_t mac[6];
    mac[0] = 0xD0;
    mac[1] = 0x73;
    mac[2] = 0xD5;
    mac[3] = 0x00;
    mac[4] = 0x07;
    mac[5] = 0x43;
    //HSBK hsbk = {42416, 49807, 25559, 9000};
    
    LIFX.addLight(mac, 56700);
    
    Serial.println("Go...");
    digitalWrite(LED, LOW);
}

void loop() {
  now = millis();

  if(now - GR.lastUpdate > 50)
  {
    GR.update(now);
  }
  
    // Status Updates
    if(now - lastStatus >= 300000)
    {
        Serial.println("Status update...");
        lastStatus = now;
        LIFX.getStatus();
    }
    
    // Toggle Test
    /*if(now - lastTest >= 17500)
    {
        Serial.println("Get Test...");
        lastTest = now;
        LIFX.getStatus();
    }*/
    
    // Discover Test
    /*if(now - lastGetService >= 30000)
    {
        Serial.printlnf("%d: Discovering...", now);
        lastGetService = now;
        uint8_t sendBuffer[36] = {0x24, 0x00, 0x00, 0x34, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00};
        
        LIFX.discover();
        
        _udp.beginPacket(broadcastIP, remotePort);
        _udp.write(sendBuffer, 36);
        _udp.endPacket();
    }*/
    // Check for Actions
    if (GR.getTap())
    {
        // Tap action waiting
        LIFX.togglePower();
        GR.setTap(false);
    }
    
    if (GR.getDoubleTap())
    {
        // Double Tap action waiting
        GR.setDoubleTap(false);
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
        Serial.printlnf("IP:%d.%d.%d.%d:%d", senderIP[0], senderIP[1], senderIP[2], senderIP[3], port);
        
        LIFX.msgIn(packetBuffer);
    
    }
    
    // Check for touch data
    if(digitalRead(INTRPT) == HIGH)
    {
        digitalWrite(LED, led);
        led = !led; //flick led 
        bool success = GSL.readData();
        if (success == 1)
        {
            int fCount = GSL.event.numberOfFingers;
            
            if(fCount > 0) {
                Serial.println("Touch:    ");
            } else {
                Serial.println("High Release: ");
            }
            Interrupt = true;
            lastInterrupt = now;
            GR.addEvent(GSL.event);
        } else {
            Serial.println("Interrupt High: Failed Read...");
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
                Serial.println("Low Release: ");
                GR.addEvent(GSL.event);
            } else {
                Serial.println("Interrupt Low: Failed Read...");
                Interrupt = true;
                lastInterrupt = now;
            }
        }
    }
}
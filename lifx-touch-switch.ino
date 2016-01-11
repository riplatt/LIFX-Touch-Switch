#include "common.h"
#include "gsl1680.h"
#include "GestureRecognition.h"
#include "lifx.h"

int WAKE = D4;
int INTRPT = D2;
int LED = D7;

bool led = false;
uint32_t now;
// Objects
GestureRecognition GR = GestureRecognition();
gsl1680 GSL = gsl1680();
lifx LIFX = lifx();

void setup() {
    Serial.begin(115200);
    delay(1000);
    //Serial.println(WiFi.localIP());
    Serial.println("Get Set...");
    // Setup IO
    pinMode(WAKE, OUTPUT);
    pinMode(INTRPT, INPUT_PULLUP);
    // Initialise Objects
    GSL.initialise(WAKE);
    GR.initialise();
    
    //Setup Test Lamp
    uint8_t mac[6];
    HSBK hsbk = {42416, 49807, 25559, 9000};
    
    mac[0] = 0xD0;
    mac[1] = 0x73;
    mac[2] = 0xD5;
    mac[3] = 0x00;
    mac[4] = 0x07;
    mac[5] = 0xBB;
    
    LIFX.addLight(mac, 56700);
    
    
    Serial.println("Go...");
}

void loop() {
  now = millis();

  if(now-20 > GR.lastUpdate)
  {
    GR.update();
  }

  if(digitalRead(INTRPT) == HIGH) {
    digitalWrite(LED, led);
    led= !led;
    bool success = GSL.readData();
    if (success == 1)
    {
     /* Serial.print("Finger Read... N# Finger: ");
      Serial.print(GSL.event.numberOfFingers);
      Serial.print(", @ ");
      Serial.print(GSL.event.timeStamp);
      Serial.println("");*/
      GR.addEvent(GSL.event);
    }
    if (!GR.tap && !GR.doubleTap && !GR.down && !GR.up && !GR.down2 && !GR.up2 && !GR.right && !GR.left && !GR.hold)
    {
        Serial.printlnf("up: %d, down: %d, right: %d, left: %d, up2: %d, down2: %d, hold: %d, tap: %d, doubleTap: %d %d", GR.up, GR.down, GR.right, GR.left, GR.up2, GR.down2, GR.hold, GR.tap, GR.doubleTap, now);
    }
  }
  if (GR.tap || GR.doubleTap || GR.down || GR.up || GR.down2 || GR.up2 || GR.right || GR.left || GR.hold)
  {
    Serial.printf("up: %d, down: %d, right: %d, left: %d, up2: %d, down2: %d, hold: %d, tap: %d, doubleTap: %d", GR.up, GR.down, GR.right, GR.left, GR.up2, GR.down2, GR.hold, GR.tap, GR.doubleTap);
    if (GR.tap)
    {
        Serial.print(" tap...");
        LIFX.togglePower();
        GR.tap = 0;
    };
    if (GR.doubleTap)
    {
        Serial.print(" doubleTap...");
        LIFX.togglePower();
        GR.doubleTap = 0;
    };
    if (GR.down)
    {
        Serial.printf(" down @ %dpix/s", GR.downSpeed);
        GR.down = 0;
    };

    if (GR.up)
    {
        Serial.printf(" up @ %dpix/s", GR.upSpeed);
        GR.up = 0;
    };

    if (GR.down2)
    {
        Serial.printf(" down2 @ %dpix/s", GR.down2Speed);
        GR.down2 = 0;
    };
    
    if (GR.up2)
    {
        Serial.printf(" up2 @ %dpix/s", GR.up2Speed);
        GR.up2 = 0;
    };
    
    if (GR.right)
    {
        Serial.printf(" right @ %dpix/s", GR.rightSpeed);
        GR.right = 0;
    };
    
    if (GR.left)
    {
        Serial.printf(" left @ %dpix/s", GR.leftSpeed);
        GR.left = 0;
    };
    
    if (GR.hold)
    {
        Serial.print(" holding...");
        GR.hold = 0;
    };
    Serial.printlnf(" %d", now);
  }
}

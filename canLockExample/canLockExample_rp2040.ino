#include <dummy_rp2040.h>
#include <CAN.h>
#include <Adafruit_NeoPixel.h>

/* 

Alernative code that runs on the Adafruit RP2040 CAN Feather for more info refer to: 
(https://learn.adafruit.com/adafruit-rp2040-can-bus-feather/)

*/
 

Adafruit_NeoPixel strip(1, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

#define MY_PACKET_ID 0xAF

uint32_t timestamp;

void setup() {  
  Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  pinMode(PIN_CAN_STANDBY, OUTPUT);
  pinMode(NEOPIXEL_POWER, OUTPUT);
  digitalWrite(NEOPIXEL_POWER, HIGH);
  digitalWrite(PIN_CAN_STANDBY, false); // turn off STANDBY
  pinMode(PIN_CAN_CS, OUTPUT);
  digitalWrite(PIN_CAN_CS, true); // turn on booster


  strip.begin();
  strip.setPixelColor(0,strip.Color(0,0,255));
  strip.setBrightness(100);
  strip.show();
 

  
  // start the CAN bus at 200 kbps
  if (!CAN.begin(200000)) {
    Serial.println("Starting CAN failed!");
    strip.setPixelColor(0,strip.Color(55,255,0));
    strip.show();
    while (1);
  } 

  timestamp = millis();
}

 

 

void loop() {
  // every 100 ms send out a packet
  if ((millis() - timestamp) > 100) {
    uint16_t pot = random();
    // send a packet with the potentiometer value  
    Serial.print("Sending packet with value ");
    Serial.print(pot);

    CAN.beginPacket(MY_PACKET_ID);
    CAN.write(pot >> 8);
    CAN.write(pot & 0xFF);
    CAN.endPacket();

    Serial.println("...sent!");
    timestamp = millis();
  }

  // try to parse any incoming packet
  int packetSize = CAN.parsePacket();

 

  if (packetSize) {
    // received a packet
    Serial.print("Received ");

 

    if (CAN.packetExtended()) {
      Serial.print("extended ");
    }

 

    if (CAN.packetRtr()) {
      // Remote transmission request, packet contains no data
      Serial.print("RTR ");
    }

 

    Serial.print("packet with id 0x");
    Serial.print(CAN.packetId(), HEX);

 

    if (CAN.packetRtr()) {
      Serial.print(" and requested length ");
      Serial.println(CAN.packetDlc());
    } else {
      Serial.print(" and length ");
      Serial.println(packetSize);

 

      uint8_t receivedData[packetSize];
      for (int i=0; i<packetSize; i++) {
        receivedData[i] = CAN.read();
        Serial.print("0x");
        Serial.print(receivedData[i], HEX);
        Serial.print(", ");
      }
      Serial.println();

 

      uint16_t value = (uint16_t)receivedData[0] << 8 | receivedData[1];
      if (value > 0xFF) {
        strip.setPixelColor(0,strip.Color(0,255,0));
        strip.show();  
      } 
      else {
        strip.setPixelColor(0,strip.Color(255,0,0));
        strip.show(); 
      }

      //strip.setPixelColor(0, Wheel(value / 4));
      //strip.show();
    }

 

 

    Serial.println();
  }
}

 

 

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
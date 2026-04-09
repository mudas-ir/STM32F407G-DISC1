/*
  WriteSingleField
  
  Description: Writes a value to a channel on ThingSpeak every 20 seconds.
  
  Hardware: ESP8266 based boards
  
  !!! IMPORTANT - Modify the secrets.h file for this project with your network connection and ThingSpeak channel details. !!!
  
  Note:
  - Requires ESP8266WiFi library and ESP8622 board add-on. See https://github.com/esp8266/Arduino for details.
  - Select the target hardware from the Tools->Board menu
  - This example is written for a network using WPA encryption. For WEP or WPA, change the WiFi.begin() call accordingly.
  
  ThingSpeak ( https://www.thingspeak.com ) is an analytic IoT platform service that allows you to aggregate, visualize, and 
  analyze live data streams in the cloud. Visit https://www.thingspeak.com to sign up for a free account and create a channel.  
  
  Documentation for the ThingSpeak Communication Library for Arduino is in the README.md folder where the library was installed.
  See https://www.mathworks.com/help/thingspeak/index.html for the full ThingSpeak documentation.
  
  For licensing information, see the accompanying license file.
  
  Copyright 2020, The MathWorks, Inc.
*/

// #include <ESP8266WiFi.h>
// #include <stdio.h>
// #include <stdint.h>
// #define LED 2
// #define RX_BUF_SIZE 10  //for UART
// #include "secrets.h"
// #include "ThingSpeak.h"  // always include thingspeak header file after other header files and custom macros


// volatile char rxData[RX_BUF_SIZE];
// char buffer[RX_BUF_SIZE];
// uint8_t idx = 0;

// char ssid[] = SECRET_SSID;  // your network SSID (name)
// char pass[] = SECRET_PASS;  // your network password
// int keyIndex = 0;           // your network key Index number (needed only for WEP)
// WiFiClient client;

// unsigned long myChannelNumber = SECRET_CH_ID;
// const char* myWriteAPIKey = SECRET_WRITE_APIKEY;

// int number = 0;

// void setup()
// {
//   Serial.begin(9600);  // Initialize serial
//   while (!Serial)
//   {
//     ;  // wait for serial port to connect. Needed for Leonardo native USB port only
//   }
//   Serial.println("ESP-01 Ready");
//   pinMode(LED, OUTPUT);

//   WiFi.mode(WIFI_STA);
//   ThingSpeak.begin(client);  // Initialize ThingSpeak
// }

// void loop()
// {

//   // Connect or reconnect to WiFi
//   if (WiFi.status() != WL_CONNECTED)
//   {
//     Serial.print("Attempting to connect to SSID: ");
//     Serial.println(SECRET_SSID);
//     while (WiFi.status() != WL_CONNECTED)
//     {
//       WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
//       digitalWrite(LED, LOW);
//       Serial.print(".");
//       delay(5000);
//     }
//     digitalWrite(LED, HIGH);
//     delay(3000);
//     for (int i = 0; i < 10; i++)
//     {
//       digitalWrite(LED, LOW);
//       delay(100);
//       digitalWrite(LED, HIGH);
//       delay(100);
//     }
//     digitalWrite(LED, HIGH);
//     Serial.println("\nConnected.");
//   }

//   while (Serial.available())
//   {
//     char c = Serial.read();

//     if (c == '\n' || c == '\r')
//     {
//       if (idx > 0)
//       {
//         rxData[idx] = '\0';

//         // Copy from volatile → normal buffer
//         memcpy(buffer, (const char*)rxData, idx + 1); // include '\0'

//         Serial.print("Received String: ");
//         Serial.println(buffer);
//         // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store up to 8 different
//         // pieces of information in a channel.  Here, we write to field 1.
//         int x = ThingSpeak.writeField(myChannelNumber, 1, String(buffer), myWriteAPIKey);
//         if (x == 200)
//         {
//           Serial.println("Channel update successful.");
//           digitalWrite(LED, LOW);
//           delay(100);
//           digitalWrite(LED, HIGH);
//           delay(100);
//         }
//         else
//         {
//           Serial.println("Problem updating channel. HTTP error code " + String(x));
//         }
//         idx = 0;
//       }
//       delay(20000);  // Wait 20 seconds to update the channel again
//     }
//     else
//     {
//       if (idx < RX_BUF_SIZE - 1)
//       {
//         rxData[idx++] = c;
//       }
//     }
//   }
// }

//01_START.----The below code is working-------------
/*
It uses if condition to send data to thinkspeak for every 20 seconds 
the 20 seconds are not alligned 
so im trying to allign in 02 code which i will write below
*/
// #include <ESP8266WiFi.h>
// #include <stdio.h>
// #include <stdint.h>
// #define LED 2
// #define RX_BUF_SIZE 64

// volatile char rxBuffer[RX_BUF_SIZE];
// volatile uint8_t head = 0;
// volatile uint8_t tail = 0;
// #include "secrets.h"
// #include "ThingSpeak.h"  // always include thingspeak header file after other header files and custom macros


// volatile char rxData[RX_BUF_SIZE];
// char buffer[RX_BUF_SIZE];
// uint8_t idx = 0;

// char ssid[] = SECRET_SSID;  // your network SSID (name)
// char pass[] = SECRET_PASS;  // your network password
// int keyIndex = 0;           // your network key Index number (needed only for WEP)
// WiFiClient client;

// unsigned long myChannelNumber = SECRET_CH_ID;
// const char* myWriteAPIKey = SECRET_WRITE_APIKEY;

// int number = 0;

// unsigned long lastUpdateTime = 0;
// const unsigned long updateInterval = 20000;  // 20 seconds

// void setup() 
// {
//   Serial.begin(9600);  // Debug (USB)
//                        // Serial.swap();          // Optional if using alternate pins
//   while (!Serial) 
//   {
//     ;  // wait for serial port to connect. Needed for Leonardo native USB port only
//   }
//   Serial.println("ESP-01 Ready");
//   pinMode(LED, OUTPUT);

//   WiFi.mode(WIFI_STA);
//   ThingSpeak.begin(client);  // Initialize ThingSpeak
// }

// void loop() 
// {

//   // Connect or reconnect to WiFi
//   if (WiFi.status() != WL_CONNECTED) 
//   {
//     Serial.print("Attempting to connect to SSID: ");
//     Serial.println(SECRET_SSID);
//     while (WiFi.status() != WL_CONNECTED) 
//     {
//       WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
//       digitalWrite(LED, LOW);
//       Serial.print(".");
//       delay(5000);
//     }
//     digitalWrite(LED, HIGH);
//     delay(3000);
//     for (int i = 0; i < 10; i++) 
//     {
//       digitalWrite(LED, LOW);
//       delay(100);
//       digitalWrite(LED, HIGH);
//       delay(100);
//     }
//     digitalWrite(LED, HIGH);
//     Serial.println("\nConnected.");
//   }

//   char data[RX_BUF_SIZE];
//   static char latestData[RX_BUF_SIZE];

//   if (getLine(data)) 
//   {
//     Serial.print("Received: ");
//     Serial.println(data);
//     strcpy(latestData, data);  // keep latest

//     // Check timing
//     if (millis() - lastUpdateTime >= updateInterval) 
//     {
//       int x = ThingSpeak.writeField(myChannelNumber, 1, String(latestData), myWriteAPIKey);

//       if (x == 200) 
//       {
//         Serial.println("Channel update successful.");
//       } 
//       else 
//       {
//         Serial.println("Error: " + String(x));
//       }

//       lastUpdateTime = millis();  // update time
//     } 
//     else 
//     {
//       Serial.println("Skipping upload (rate limit)");
//     }
//   }
// }
// void serialEvent() 
// {
//   while (Serial.available()) 
//   {
//     char c = Serial.read();

//     uint8_t next = (head + 1) % RX_BUF_SIZE;

//     if (next != tail)
//     {  // buffer not full
//       rxBuffer[head] = c;
//       head = next;
//     }
//   }
// }
// bool getLine(char* out) 
// {
//   static uint8_t idx = 0;

//   while (tail != head) 
//   {
//     char c = rxBuffer[tail];
//     tail = (tail + 1) % RX_BUF_SIZE;

//     if (c == '\n' || c == '\r') 
//     {
//       if (idx > 0) 
//       {
//         out[idx] = '\0';
//         idx = 0;
//         return true;  // full string ready
//       }
//     } 
//     else 
//     {
//       if (idx < RX_BUF_SIZE - 1) 
//       {
//         out[idx++] = c;
//       }
//     }
//   }
//   return false;
// }
//01_END.----The above code is working-------------


//02_START.----The below code is working-------------
/*
It uses if condition to send data to thinkspeak for every 20 seconds 
here im trying to allign the 20 seconds
*/
/*
The below code Receives data tru UART from STM32 for every 5 seconds and uploades the latest data for every 20 seconds to 
*/
#include <ESP8266WiFi.h>
#include <stdio.h>
#include <stdint.h>
#define LED 2 //  Onboard Blue LED
#define RX_BUF_SIZE 64

volatile char rxBuffer[RX_BUF_SIZE];
volatile uint8_t head = 0;
volatile uint8_t tail = 0;
#include "secrets.h"
#include "ThingSpeak.h"  // always include thingspeak header file after other header files and custom macros


volatile char rxData[RX_BUF_SIZE];
char buffer[RX_BUF_SIZE];
uint8_t idx = 0;

char ssid[] = SECRET_SSID;  // your network SSID (name)
char pass[] = SECRET_PASS;  // your network password
int keyIndex = 0;           // your network key Index number (needed only for WEP)
WiFiClient client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char* myWriteAPIKey = SECRET_WRITE_APIKEY;

int number = 0;

unsigned long lastUpdateTime = 0;
const unsigned long updateInterval = 20000;  // 20 seconds

  char data[RX_BUF_SIZE];
  static char latestData[RX_BUF_SIZE];

void setup() 
{
  Serial.begin(9600);  // Debug (USB)
                       // Serial.swap();          // Optional if using alternate pins
  while (!Serial) 
  {
    ;  // wait for serial port to connect. Needed for Leonardo native USB port only
  }
  Serial.println("ESP-01 Ready");
  pinMode(LED, OUTPUT);

  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);  // Initialize ThingSpeak
}

void loop() 
{

  // Connect or reconnect to WiFi
  if (WiFi.status() != WL_CONNECTED) 
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while (WiFi.status() != WL_CONNECTED) 
    {
      WiFi.begin(ssid, pass);  // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      digitalWrite(LED, LOW);
      Serial.print(".");
      delay(5000);
    }
    digitalWrite(LED, HIGH);
    delay(3000);
    for (int i = 0; i < 10; i++) 
    {
      digitalWrite(LED, LOW);
      delay(100);
      digitalWrite(LED, HIGH);
      delay(100);
    }
    digitalWrite(LED, HIGH);
    Serial.println("\nConnected.");
  }

  // char data[RX_BUF_SIZE];
  // static char latestData[RX_BUF_SIZE];

  if (getLine(data)) 
  {
    Serial.print("Received: ");
    Serial.println(data);
    strcpy(latestData, data);  // keep latest
  }
    // Check timing
    if (millis() - lastUpdateTime >= updateInterval) 
    {
      lastUpdateTime = millis();  // update time

      int x = ThingSpeak.writeField(myChannelNumber, 1, String(latestData), myWriteAPIKey);

      if (x == 200) 
      {
        Serial.print("Channel update successful. -> ");
        Serial.print("Uploaded Data: ");
        Serial.println(latestData);
      } 
      else 
      {
        Serial.println("Error: " + String(x));
      }
    } 
    // else 
    // {
    //   Serial.println("Skipping upload (rate limit)");
    // }
}
void serialEvent() 
{
  while (Serial.available()) 
  {
    char c = Serial.read();

    uint8_t next = (head + 1) % RX_BUF_SIZE;

    if (next != tail)
    {  // buffer not full
      rxBuffer[head] = c;
      head = next;
    }
  }
}
bool getLine(char* out) 
{
  static uint8_t idx = 0;

  while (tail != head) 
  {
    char c = rxBuffer[tail];
    tail = (tail + 1) % RX_BUF_SIZE;

    if (c == '\n' || c == '\r') 
    {
      if (idx > 0) 
      {
        out[idx] = '\0';
        idx = 0;
        return true;  // full string ready
      }
    } 
    else 
    {
      if (idx < RX_BUF_SIZE - 1) 
      {
        out[idx++] = c;
      }
    }
  }
  return false;
}
//02_END.----The above code is working-------------
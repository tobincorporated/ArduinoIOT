/*************************************************** 
  This sketch has the Arduino send a basic Get or Post request to a web page
  
 ****************************************************/
 
// Import libraries
// You will need to place the CC3000 and the Connector libraries 
//      in your Arduino/libraries folder
#include <ccspi.h>
#include <SPI.h>
#include <string.h>
#include "utility/debug.h"
#include <Connector.h>

/******************************************************************************
The following Digital Pins are used by the Wifi shield and Serial Monitor.
They should be considered unavailable for *any* other purpose:
0
1
3
5
10
11
12
13
******************************************************************************/

//Wifi Network credentials
#define WLAN_SSID       "H4HQP"           // Network name, cannot be longer than 32 characters!
#define WLAN_PASS       "54C8E356"        // Network password

// Type of Wifi Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
#define WLAN_SECURITY   WLAN_SEC_WPA2

// What page to connect to

#define WEBSITE      "pefarduino2.meteor.com" //domain the Arduino will access
#define GETPAGE      "/api/getDat"            // API Get page
#define POSTPAGE     "/api/postDat/"          // API Post Page

Connector wifiConn = Connector( WLAN_SSID,WLAN_PASS,WLAN_SECURITY, WEBSITE, GETPAGE,POSTPAGE); 
String dataStr = "1";
String getOrPost = "post";

void setup(void)
{
  
  //Setting output pins, to be controlled via Wifi
  pinMode(7, OUTPUT);
  pinMode(6, OUTPUT);
  
 /* Initialise the module */
  wifiConn.connectToSite();
 
}

void loop(void)
{
    
    if (getOrPost == "get")
    {
        dataStr = wifiConn.valueGet();
        if(dataStr.toInt()>6)
           { 
             digitalWrite(7,HIGH);
             digitalWrite(6,LOW);
           }
           else
           {
             digitalWrite(7,LOW);
             digitalWrite(6,HIGH);
           }
      }
         
    if (getOrPost == "post") {
      dataStr = String(  dataStr.toInt()+1 );
      wifiConn.valuePost(dataStr);
      
    }
}






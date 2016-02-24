#include "Arduino.h"
#include <Connector.h>
#include <Tinysine_CC3000.h>
#include <ccspi.h>
#include <SPI.h>
#include <string.h>
#include "utility/debug.h"


	String currentLine = "";
	long checkupTime = 10000;
	long lastTime = 0;
	long cycles = 1;   
    Tinysine_CC3000_Client www;
	char* WLAN_SSID;
	char* WLAN_PASS;
	int WLAN_SECURITY;
	char* WEBSITE;
	char* GETPAGE;
	char* POSTPAGE;
	uint32_t ip;
	long t = millis();
	long t_elapsed = 0;
	// These are the interrupt and control pins
	#define Tinysine_CC3000_IRQ   3  // MUST be an interrupt pin!
	// These can be any two pins
	#define Tinysine_CC3000_VBAT  5
	#define Tinysine_CC3000_CS    10
	
Connector::Connector(  char* SSID,  char* PASS,  int SEC,  char* SITE,  char* GPAGE, char* PPAGE) : cc3000 (Tinysine_CC3000_CS, Tinysine_CC3000_IRQ, Tinysine_CC3000_VBAT, SPI_CLOCK_DIV2)
{
	

	WLAN_SSID=SSID;
	WLAN_PASS=PASS;
	WLAN_SECURITY=SEC;
	WEBSITE=SITE;
	GETPAGE=GPAGE;
	POSTPAGE = PPAGE;
	}

void Connector::connectToSite()
{
    
  Serial.begin(115200);
  Serial.println(F("Hello, CC3000!\n")); 
  Serial.print("Free RAM: ");
  Serial.println(getFreeRam(), DEC);
  
  Serial.println(F("\nInitializing..."));
  if (!cc3000.begin())
  {
    Serial.println(F("Couldn't begin()! Check your wiring?"));
    while(1);
  }
    if (!cc3000.connectToAP(WLAN_SSID, WLAN_PASS, WLAN_SECURITY)) {
    Serial.println(F("Failed!"));
    while(1);
  }
  
  Serial.println(F("Connected to Wifi!"));
  
  /* Wait for DHCP to complete */
  Serial.println(F("Request DHCP"));
  while (!cc3000.checkDHCP())
  {
    delay(100); // ToDo: Insert a DHCP timeout!
  }  

  /* Display the IP address DNS, Gateway, etc. */  
  while (! displayConnectionDetails()) {
    delay(1000);
  }

  // Optional: Do a ping test on the website
  /*
  Serial.print(F("\n\rPinging ")); cc3000.printIPdotsRev(ip); Serial.print("...");  
  replies = cc3000.ping(ip, 5);
  Serial.print(replies); Serial.println(F(" replies"));
  */  

  /* Try connecting to the website */
  
  
  Serial.print(WEBSITE); Serial.println(F(" -> "));
  while (ip == 0) {
    if (! cc3000.getHostByName(WEBSITE, &ip)) {
      Serial.println(F("Couldn't resolve!"));
    }
    delay(500);
  }

  cc3000.printIPdotsRev(ip);
     

    
}

String Connector::valueGet()
{
  Serial.println("");
  Serial.println("New Get Value!");
  long t = millis();
  long t_elapsed = 0;
   do {    www = cc3000.connectTCP(ip, 80);
          t_elapsed = millis()-t; 
         } while((!www.connected()) && (t_elapsed < 5000));
     
  // do loop not in Adafruit

   


            if (www.connected()) {
            Serial.println("Entering Commands");
            www.fastrprint(F("GET "));
            www.fastrprint(GETPAGE);
            www.fastrprint(F(" HTTP/1.0\r\n"));
            www.fastrprint(F("Host: "));
            www.fastrprint(WEBSITE);
            www.fastrprint(F("\n"));
            www.fastrprint(F("Connection: close\n"));
           // www.fastrprint(F("Data Incoming:\n"));
            www.println();
          
        
          Serial.println(F("-------------------------------------\n"));
        
 //As long as there is output available, the Arduino will read output from the API

 



          while (www.connected()) {
            while (www.available()) {
              char c = www.read();  //Every line of data is downloaded one character at a time, into c
              currentLine += c; // Each character is added to the end of the currentLine variable
              Serial.print(c);
              if(c == '\n'){  
                currentLine = "";  //After each line, we want to clear the currentLine variable
              }
            }
          }
        
         } else {
          Serial.println(F("Connection failed"));    
           
          }
          Serial.println(F("\n----------------------+---------------"));
          


         
         
          Serial.println("Final Reading:");
          Serial.println(currentLine);


          Serial.print(cycles); //Number of times the loop has run
          Serial.println(" Cycles");
          
          delay(100);
          www.close();
          delay(200);
          cycles ++;

          lastTime = millis();

		return currentLine;

          //Now we decide what to do with our data

                   
     	
}

void Connector::valuePost(String postVal)
{
  Serial.println("");
  Serial.println("New Post Value!");
  long t = millis();
  long t_elapsed = 0;
  int numChar = 20;
  char postChar[ numChar ] ;
  postVal.toCharArray(postChar, numChar);
  
  
   do {    www = cc3000.connectTCP(ip, 80);
          t_elapsed = millis()-t; 
         } while((!www.connected()) && (t_elapsed < 5000));
     
  // do loop not in Adafruit

   


            if (www.connected()) {
            Serial.println("Entering Commands");
            www.fastrprint(F("POST "));
            www.fastrprint(POSTPAGE);
			www.fastrprint(postChar);
            www.fastrprint(F(" HTTP/1.0\r\n"));
            www.fastrprint(F("Host: "));
            www.fastrprint(WEBSITE);
            www.fastrprint(F("\n"));
            www.fastrprint(F("Connection: close\n"));
           // www.fastrprint(F("Data Incoming:\n"));
            www.println();
          
        
          Serial.println(F("-------------------------------------\n"));
        
 //As long as there is output available, the Arduino will read output from the API

 



          while (www.connected()) {
            while (www.available()) {
              char c = www.read();  //Every line of data is downloaded one character at a time, into c
              currentLine += c; // Each character is added to the end of the currentLine variable
              Serial.print(c);
              if(c == '\n'){  
                currentLine = "";  //After each line, we want to clear the currentLine variable
              }
            }
          }
        
         } else {
          Serial.println(F("Connection failed"));    
           
          }
          Serial.println(F("\n----------------------+---------------"));

          Serial.print(cycles); //Number of times the loop has run
          Serial.println(" Cycles");
          
          delay(100);
          www.close();
          delay(200);
          cycles ++;

          lastTime = millis();



          //Now we decide what to do with our data

                   
     	
}

void Connector::arduReset(){
  
  #ifndef _SOFT_RESTART_H
  #define _SOFT_RESTART_H
  
  #include <avr/wdt.h>
  
  #define soft_restart()        \
  do                          \
  {                           \
	  wdt_enable(WDTO_15MS);  \
	  for(;;)                 \
	  {                       \
	  }                       \
  } while(0)
  
  #endif
  
  delay(10000);
  soft_restart();
}

bool Connector::displayConnectionDetails(void)
{
  uint32_t ipAddress, netmask, gateway, dhcpserv, dnsserv;
  
  if(!cc3000.getIPAddress(&ipAddress, &netmask, &gateway, &dhcpserv, &dnsserv))
  {
    Serial.println(F("Unable to retrieve the IP Address!\r\n"));
    return false;
  }
  else
  {
    Serial.print(F("\nIP Addr: ")); cc3000.printIPdotsRev(ipAddress);
    Serial.print(F("\nNetmask: ")); cc3000.printIPdotsRev(netmask);
    Serial.print(F("\nGateway: ")); cc3000.printIPdotsRev(gateway);
    Serial.print(F("\nDHCPsrv: ")); cc3000.printIPdotsRev(dhcpserv);
    Serial.print(F("\nDNSserv: ")); cc3000.printIPdotsRev(dnsserv);
    Serial.println();
    return true;
  }
}
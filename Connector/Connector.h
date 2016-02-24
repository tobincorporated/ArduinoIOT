#ifndef Connector_h
#define Connector_h

#include "Arduino.h"
#include <Tinysine_CC3000.h>
#include <ccspi.h>
#include <SPI.h>
#include <string.h>
#include "utility/debug.h"
class Connector
{
  public:
    Connector( char* WLAN_SSID,  char* WLAN_PASS,  int WLAN_SECURITY,  char* WEBSITE,  char* GPAGE, char* PPAGE);
    void connectToSite();
    void arduReset();
	String valueGet();
	void valuePost(String postVal);
  private:
	bool displayConnectionDetails(void);
	Tinysine_CC3000 cc3000;
	String currentLine ;
	long checkupTime;
	long lastTime;
	long cycles;   
	Tinysine_CC3000_Client www;
	char* WLAN_SSID;
	char* WLAN_PASS;
	int WLAN_SECURITY;
	char* WEBSITE;
	char* _GETPAGE;
	char* _POSTPAGE;
	uint32_t ip;
	long t;
	long t_elapsed ;
};

#endif
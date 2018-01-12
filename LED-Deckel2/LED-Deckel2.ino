
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>

//NeoPixel
#include <Adafruit_NeoPixel.h>
#define PIN D1

int minLEDNum = 0;
int maxLEDNum = 39;

int animStartLEDNum = 1;

int ranLEDNum = 0;
int lastRanLEDNum = 0;

/* Set these to your desired credentials. */
const char *ssid = "ESP-Elbsegler";
const char *password = "ASViM1901";

ESP8266WebServer server(80);

String webPage = "";
Adafruit_NeoPixel strip = Adafruit_NeoPixel(39, PIN, NEO_GRB + NEO_KHZ800);
uint32_t cOff = strip.Color(0, 0, 0);
const int led = 13;


void setup() {

	Serial.begin(115200);
	Serial.println();
	Serial.print("Configuring access point...");
	/* You can remove the password parameter if you want the AP to be open. */
  WiFi.mode(WIFI_AP);
	WiFi.softAP(ssid, password)? "Ready" : "Failed!";

	IPAddress myIP = WiFi.softAPIP();
	Serial.print("AP IP address: ");
	Serial.println(myIP);
	server.on("/", handleRoot);

  server.begin();
  Serial.println("HTTP server started");
  
}

void loop() {

  Serial.printf("Stations connected = %d\n", WiFi.softAPgetStationNum());
  
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  delay(10);

  
  
	server.handleClient();
}


void handleRoot() {

  //////////////////////////
  /// building a website ///
  //////////////////////////
  //head
    webPage +="<!DOCTYPE html><html><head><title>ESP-LED Deckel</title><style>body { background-color: #cccccc; font-family: Arial; Color: #008; }</style><meta name=\"viewport\" content=\"width=device-width, height=device-height, initial-scale=1.0, user-scalable=0, minimum-scale=1.0, maximum-scale=1.0\" /></head><body>";
    webPage +="<title>ESP-LED Deckel</title>";
    webPage +="<h1>ESP-LED Deckel</h1>";
  //webPage +="<form action=\"\" name=\"pick\" method=\"post\"><input type=\"color\" name=\"c\" value=\"%02d\" onchange=\"document.forms['pick'].submit();\" />&nbsp;<span onclick=\"document.forms['pick'].submit();\" > Farbwahl </span></form>";
    webPage +="<p><a href=\"color\"><button>color: ON</button></a>";
    webPage +="<p><a href=\"blue\"><button>Blau: ON</button></a>";
    webPage +="<p><a href=\"red\"><button>Rot: ON</button></a>";
    webPage +="<p><a href=\"purple\"><button>Rosa: ON</button></a>";
    webPage +="<p><a href=\"navi\"><button>Navigationsbeleuchtung: ON</button></a>";
    webPage +="<p><a href=\"police\"><button>Police: ON</button></a>";
    webPage +="<p><a href=\"rainbow\"><button>Rainbow: ON</button></a>";
    webPage +="<p><a href=\"off\"><button>OFF</button></a>";
  
  //body
    webPage +="</body></html>";

    server.send(200, "text/plain", webPage);
}




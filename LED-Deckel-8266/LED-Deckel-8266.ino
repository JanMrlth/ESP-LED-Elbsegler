//Webserver
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

//NeoPixel
#include <Adafruit_NeoPixel.h>
#define PIN D1

int minLEDNum = 0;
int maxLEDNum = 39;

int animStartLEDNum = 1;

int ranLEDNum = 0;
int lastRanLEDNum = 0;



const char* ssid = "foo-ssid";
const char* password = "bar-pass";

ESP8266WebServer server(80);

String webPage = "";

Adafruit_NeoPixel strip = Adafruit_NeoPixel(39, PIN, NEO_GRB + NEO_KHZ800);

uint32_t cOff = strip.Color(0, 0, 0);

const int led = 13;

void handleRoot() {
  digitalWrite(led, 1);



  digitalWrite(led, 0);
}

void handleNotFound(){
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void){


  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");

  // building a website
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
  
  }

  server.on("/", [](){
    server.send(200, "text/html", webPage);
  });

    server.on("/rainbow", [](){
    server.send(200, "text/html", webPage);
    while(1){theaterChaseRainbow(80);}
  });

  server.on("/red", [](){
    server.send(200, "text/html", webPage);
    colorWipe(strip.Color(255, 0, 0), 20); // Red
  });

    server.on("/blue", [](){
    server.send(200, "text/html", webPage);
    colorWipe(strip.Color(0, 0, 255), 20); // Red
  });


  server.on("/purple", [](){
    server.send(200, "text/html", webPage);
    colorWipe(strip.Color(255, 0, 255), 20); // Red
  });


    server.on("/navi", [](){
    server.send(200, "text/html", webPage);
    Navi();
  });


// COLOR
    server.on("/color", [](){
    server.send(200, "text/html", webPage);
    String color = server.arg("c");

    int number = (int) strtol( &color[1], NULL, 16);
  
    // splitting into three parts
    int r = number >> 16;
    int g = number >> 8 & 0xFF;
    int b = number & 0xFF;

  
    animationOff();
    colorWipe(strip.Color(r,g,b), 20); // Blue

    for(int i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color( r, g, b ) );
        }
    // init
    strip.show();
  });


// POLICE
  
   server.on("/police", [](){
    server.send(200, "text/html", webPage);
    animationOff();
    killallpixels();
    pulseWave(strip.Color(0, 0, 255), 4,0.5 ,12 , 20);
    Navi();
  });


// OFF
   server.on("/off", [](){
    server.send(200, "text/html", webPage);
    animationOff();
    killallpixels();
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");

// INIT NEo Pixel
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop(void){
  server.handleClient();


  /*
    // data from the colorpicker (e.g. #FF00FF)
  String color = server.arg("c");
  Serial.println("Color: " + color);
  // setting the color to the strip 
  setNeoColor(color);*/

}

void Navi() {

    animationOff();
    killallpixels();

    strip.setPixelColor(12, 0,255,0,255);
    strip.setPixelColor(13, 0,255,0,255);



    strip.setPixelColor(25, 255,0,0,0); 
    strip.setPixelColor(26, 255,0,0,125); 

    
    strip.setPixelColor(0, 60,60,60); 
    strip.setPixelColor(38, 60,60,60); 

         
    strip.show();


}


// Bright head followed by a tail
void pulseWave(uint32_t c, uint8_t tailLength, double fadePercent, uint8_t repeatCount, uint8_t wait){
  for( uint8_t r=0; r<repeatCount; r++){  // how many times to cycle
    for( uint8_t i=animStartLEDNum; i<strip.numPixels(); i++){ // cycle through the strip
      animationOff(); // turn off animation pixels
      strip.setPixelColor(i, c); // set head color
      for( uint8_t z=1; z<tailLength; z++){
        if(i-z >= animStartLEDNum){
          strip.setPixelColor(i-z, strip.Color(  getPixelColorComponent(i-z+1,1) * fadePercent,
                                                 getPixelColorComponent(i-z+1,2) * fadePercent,
                                                 getPixelColorComponent(i-z+1,3) * fadePercent));
        }
      }
      strip.show();
      delay(wait);
    }
  }
}

// turn off the animation pixels
void animationOff(){
  for( uint8_t x=animStartLEDNum; x<strip.numPixels(); x++){
    strip.setPixelColor(x, cOff);
  }
  strip.show();
}


// Get a color value by segment (R=1, G=2, B=3) 
uint8_t getPixelColorComponent(uint8_t pixelNum, uint8_t segment){
  uint8_t segmentColorValue;
  uint32_t c = strip.getPixelColor(pixelNum);
  switch( segment ){
    case 1:
      segmentColorValue = ( c >> 16) & 255;
      break;
    case 2:
      segmentColorValue = ( c >> 8) & 255;
      break;
    case 3:
      segmentColorValue = c & 255;
      break;
  }
  return segmentColorValue;
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


void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void killallpixels(){
    
    for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, 0,0,0);
    }
    strip.show();
}


//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}



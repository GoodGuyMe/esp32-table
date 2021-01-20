// reddit user /u/Procupine 4/7/2019
// the time between transitions is hardcoded (sorry), but the color of the "rain" and speed of the rainbow should be easy to edit
// This code is intended for three rows of LEDs
// I tried my best to comment everything but feel free to pm if you have more questions
 
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

const char* ssid = "Sorry?";
const char* password = "watzeije?";

#include <FastLED.h>

// Definitions for the led strip
#define LED_PIN 5 //digital pin 5
#define NUM_LEDS 588
#define MAX_BRIGHTNESS 255  
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];
CRGB colorsArray[NUM_LEDS]; //pregenerated rainbow to make the acutal rainbow less laggy

int fps = 50; //it takes ~20ms to push all the data to all 579 LEDs so the hard limit is ~50fps
#define animationLength 15000 //milliseconds
int rainbowSpeed = 100; //bigger = faster rainbow

const int size = NUM_LEDS / 3; //size of the rainbow, divided by 3 for 3 rows of leds

int frequency = 10; //frequency of rain and glitter flashes
int fade = 20;

// Determine if the leds should be on or not
int on = 1;

// Determine the speed of the rain
int minRainSpeed = 0; // How many ticks + 1 the rain will minimally stay on 1 pixel
int maxRainSpeed = 3; // How many ticks + 1 the rain will maximally stay on 1 pixel

// Color of rain and glitter
CRGB ledColors[64];
// Counter to keep track of how large the color array is.
int ledColorsSize = 1;

// Program that is being run
int program = 0;
AsyncWebServer server(80);

String state = "On";

String processor(const String& var) {
  return String();
}

String colorProcessor(const String& var) {
  if (var == "RVALUE") {
    return String(ledColors[0][0]);
  }
  if (var == "GVALUE") {
    return String(ledColors[0][1]);
  }
  if (var == "BVALUE") {
    return String(ledColors[0][2]);
  }
  return String();
}

int brightness = 10;

String brightnessProcessor(const String &var) {
  if (var == "BRIGHTVALUE") {
    return String(brightness);
  }
  return String();
}

String speedProcessor(const String& var) {
  if (var == "FREQVALUE") {
    return String(frequency);
  }
  if (var == "FADEVALUE") {
    return String(fade);
  }
  if (var == "SPEEDVALUE") {
    return String(rainbowSpeed);
  }
  return String();
}

String r = "01";
String g = "01";
String b = "01";

void handleColor(AsyncWebServerRequest *req) {
  int id = 0;
  if (req->hasParam("del") && req->hasParam("id")) {
    // Delete the selected color.
    id = req->getParam("id")->value().toInt();
    for (int i = id; i < ledColorsSize - 1; i++) {
      ledColors[i] = ledColors[i + 1];
    }
    ledColorsSize--;
    return;
  } else if (req->hasParam("id")) {
    // Update color
    id = req->getParam("id")->value().toInt();
  } else if (req->hasParam("new")){
    // Add new color
    if (ledColorsSize >=64) {
      Serial.println("Cant add more then 64 colors");
      return;
    }
    id = ledColorsSize;
    ledColorsSize++;
  } else {
    // just /color was called, do nothing;
    return;
  }
  if (req->hasParam("r")) {
    ledColors[id].r = req->getParam("r")->value().toInt();
  }
  if (req->hasParam("g")) {
    ledColors[id].g = req->getParam("g")->value().toInt();
  }
  if (req->hasParam("b")) {
    ledColors[id].b = req->getParam("b")->value().toInt();
  }
}

void handleBrightness(AsyncWebServerRequest *req) {
  if (req->hasParam("b")) {
    brightness = req->getParam("b")->value().toInt();
    FastLED.setBrightness(brightness);
  }
}

void handleMode(AsyncWebServerRequest *req) {
  if(req->hasParam("m")) {
    String input = req->getParam("m")->value();
    if (input.equalsIgnoreCase("all")) {
      program = 0;
    } 
    else if (input.equalsIgnoreCase("rain")) {
      program = 1;
    }
    else if (input.equalsIgnoreCase("glitter")) {
      program = 2;
    }
    else if (input.equalsIgnoreCase("rainbow")) {
      program = 3;
    }
    else if (input.equalsIgnoreCase("solid")) {
      program = 4;
    }
  }
}

void handleSpeeds(AsyncWebServerRequest *req) {
  if (req->hasParam("freq")) {
    frequency = req->getParam("freq")->value().toInt();
  }
  if (req->hasParam("fade")) {
    fade = req->getParam("fade")->value().toInt();
  }
  if (req->hasParam("speed")) {
    rainbowSpeed = req->getParam("speed")->value().toInt();
  }
  if (req->hasParam("fps")) {
    fps = req->getParam("fps")->value().toInt();
  }
}

void handleOff(AsyncWebServerRequest *req) {
  on = 0;
}

void handleOn(AsyncWebServerRequest *req) {
  on = 1;
}

void setup() {
  delay(50); //because life sucks
  Serial.begin(115200);  

  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  WiFi.mode(WIFI_STA);
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

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *req){
    req->send(SPIFFS, "/index.html", String(), false, processor);
  });

  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *req){
    req->send(SPIFFS, "/style.css", "text/css");
  });

  server.on("/color", HTTP_GET, [](AsyncWebServerRequest *req) {
    handleColor(req);
    req->send(SPIFFS, "/color.html", String(), false, colorProcessor);
  });

  server.on("/color.js", HTTP_GET, [](AsyncWebServerRequest *req) {
    req->send(SPIFFS, "/color.js", "application/javascript");
  });

  server.on("/brightness", HTTP_GET, [](AsyncWebServerRequest *req) {
    handleBrightness(req);
    req->send(SPIFFS, "/brightness.html", String(), false, brightnessProcessor);
  });

  server.on("/brightness.js", HTTP_GET, [](AsyncWebServerRequest *req) {
    req->send(SPIFFS, "/brightness.js", "application/javascript");
  });

  server.on("/speed", HTTP_GET, [](AsyncWebServerRequest *req) {
    handleSpeeds(req);
    req->send(SPIFFS, "/speed.html", String(), false, speedProcessor);
  });

  server.on("/speed.js", HTTP_GET, [](AsyncWebServerRequest *req) {
    req->send(SPIFFS, "/speed.js", "application/javascript");
  });

  server.on("/mode", HTTP_GET, [](AsyncWebServerRequest *req) {
    handleMode(req);
    req->send(SPIFFS, "/index.html", String(), false, processor);
  });

  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *req) {
    handleOn(req);
    req->send(SPIFFS, "/index.html", String(), false, processor);
  });

  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *req) {
    handleOff(req);
    req->send(SPIFFS, "/index.html", String(), false, processor);
  });

  server.begin();
  Serial.println("HTTP server started");
  
  ledColors[0] = CRGB(1, 255, 1); //make sure each value is at least 1, that's how the program distinguishes between the rain and the rainbow

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  MAX_BRIGHTNESS ); // Set the maximum brightness
  FastLED.setBrightness( brightness ); // Set it to a more comfortable level :)
  //initial bias is arbitary and then normalized to the rainbow size. More bias will make the respective section of the rainbow larger
  int BRsize = 90; //section between 100% blue and 100% red
  int RGsize = 100; //section between 100% red and 100% green
  int GBsize = 80; //section between 100% green and 100% blue
  int biasSum = BRsize + RGsize + GBsize;
  BRsize = size * (float(BRsize) / float(biasSum)); //normalize bias
  RGsize = size * (float(RGsize) / float(biasSum)); //normalize bias
  GBsize = size * (float(GBsize) / float(biasSum)); //normalize bias
  biasSum = BRsize + RGsize + GBsize;
 
  //makes the sum of the bias the same size as the rainbow, fixes "holes" caused by rounding
  if (biasSum < size) {
    BRsize ++;
    biasSum ++;
    if (biasSum < size) {
      RGsize ++;
      biasSum ++;
      if (biasSum < size) {
        GBsize ++;
        biasSum ++;
      }
    }
  }
  else if (biasSum > size) {
    BRsize --;
    biasSum --;
    if (biasSum > size) {
      RGsize --;
      biasSum --;
      if (biasSum > size) {
        GBsize --;
        biasSum --;
      }
    }
  }
 
  //generate the rainbow. Uses linear interpolations between points set by the bias
  for (int i = 0; i < BRsize; i++) {
    colorsArray[i] = CRGB( int(round(255 * float(i) / float(BRsize))) ,
                           0 ,
                           int(round(255 - 255 * float(i) / (BRsize) )));
  }
  for (int i = 0; i < RGsize; i++) {
    colorsArray[i + BRsize] = CRGB(int(round(255 - 255 * float(i) / float(RGsize))) , int(round(255 * (float(i) / float(RGsize)))) , 0 );
 
  }
  for (int i = 0; i < GBsize; i++) {
    colorsArray[i + BRsize + RGsize] = CRGB(0, int(round(255 - 255 * float(i) / float(GBsize))) , int(round(255 * (float(i) / float(GBsize)))));
  }
 
  //copy first row onto next two rows
  for (int i = 0; i < NUM_LEDS / 3; i++) {
    colorsArray[i + NUM_LEDS / 3] = colorsArray[i];
    colorsArray[i + 2 * NUM_LEDS / 3] = colorsArray[i];
  }
}
 
void loop() {
  //a timer is used to set a maximum fps
  unsigned long start = millis(); //start timer
  FastLED.show();

  if (on) {
    switch(program) {
      case 0:
      default:
        if (millis() % animationLength < 5000) {
          rain(); //not really rain, just some LEDs that converge to the bottom and fade away
        }
        else if (millis() % animationLength < 10000) {
          glitter(); //randomly lights up LEDs in different colors
        }
        else {
          rainbow(); //generic scrolling rainbow
        }
        break;
      case 1:
        rainStable();
        break;
      case 2:
        glitterStable();
        break;
      case 3:
        rainbow();
        break;
      case 4:
        fill_solid(leds, NUM_LEDS, ledColors[0]);
        delay(1);
        fill_solid(leds, NUM_LEDS, ledColors[0]);
        delay(1);
        break;
    }
  }
  else {
    fill_solid(leds, NUM_LEDS, CRGB::Black);
  }

  unsigned long delta = millis() - start; //end of timer
  if ( 1000 / fps > delta) { //adds pause to ensure that fps is at  or below the defined fps
    FastLED.delay(1000 / fps - delta);
  }
}

int sumValues(CRGB led) {
  return led.r + led.g + led.b;
}

int multValues(CRGB led) {
  return led.r * led.g * led.b;
}
 
//not really rain, just some LEDs that converge to the bottom and fade away
void rain() {
  //generates random LEDs to turn on
  int time_frequency = 0.001 * (millis() % animationLength) + 7 ; //how many LEDs per update turn on, starts slow and builds up
  int topRow[time_frequency];
  for (int i = 0; i < time_frequency; i++) {
    topRow[i] = random16(2 * NUM_LEDS / 3, NUM_LEDS);
  }
 
  // fades bottom row
  for (int i = 0; i < NUM_LEDS / 3; i++) {
    if (sumValues(leds[i]) != 255 ) { // make the rainbow now fade initially
      leds[i].fadeToBlackBy( fade );
    }
  }
 
  //turns off middle row and moves LEDs that were on down
  for (int i = NUM_LEDS / 3; i < 2 * NUM_LEDS / 3; i++) {
    if (i < 2 * NUM_LEDS / 3 && multValues(leds[i]) > 0) {
      leds[i - NUM_LEDS / 3] = leds[i];
      leds[i] = CRGB(0, 0, 0);
    }
  }
 
  //turns off top row, moves LEDs that were on down, turns on new random LEDs
  for (int i = 2 * NUM_LEDS / 3; i < NUM_LEDS; i++) { //toprow, turns random LEDs on, turns other LEDs off
    //turns off LEDs in the top row that were not just turned on, turns on LEDs in the second row below the LEDs that are being turned off
    if (multValues(leds[i]) > 0) {
      leds[i - NUM_LEDS / 3] = leds[i];
      leds[i] = CRGB(0, 0, 0);
    }
  }

  for (int i = 0; i < time_frequency; i++) {
    leds[topRow[i]] = ledColors[random8(0, ledColorsSize)];
  }
}

void rainStable() {
  int topRow[frequency];
  for (int i = 0; i < frequency; i++) {
    topRow[i] = random16(2 * NUM_LEDS / 3, NUM_LEDS);
  }
 
  // fades bottom row
  for (int i = 0; i < NUM_LEDS / 3; i++) {
    if (sumValues(leds[i]) != 255 ) { // Don't fade the rainbow
      leds[i].fadeToBlackBy( fade );
    }
  }
 
  //turns off middle row and moves LEDs that were on down
  for (int i = NUM_LEDS / 3; i < 2 * NUM_LEDS / 3; i++) {
    if (i < 2 * NUM_LEDS / 3 && multValues(leds[i]) > 0) {
      leds[i - NUM_LEDS / 3] = leds[i];
      leds[i] = CRGB(0, 0, 0);
    }
  }
 
  //turns off top row, moves LEDs that were on down, turns on new random LEDs
  for (int i = 2 * NUM_LEDS / 3; i < NUM_LEDS; i++) { //toprow, turns random LEDs on, turns other LEDs off
    //turns off LEDs in the top row that were not just turned on, turns on LEDs in the second row below the LEDs that are being turned off
    if (multValues(leds[i]) > 0) {
      leds[i - NUM_LEDS / 3] = leds[i];
      leds[i] = CRGB(0, 0, 0);
    }
  }
  for (int i = 0; i < frequency; i++) {
    
    leds[topRow[i]] = ledColors[random8(0, ledColorsSize)];
  }
}
 
//randomly lights up LEDs in different colors
void glitter() {
  int time_frequency = 2 * pow(float(millis() % animationLength - 5000) / 1000, 4) + 10; //frequency is how much "glitter" there is. Starts out at basically 10 and then grows exponentially (^4)
  int ledOffset = millis() * rainbowSpeed / fps / 100 % NUM_LEDS ; // 0 - NUM_LEDS, calculates how far the generated table should be offset from the starting position for the given cycle count
 
  int lightToTurnOn[time_frequency]; //randomly generates which LEDs to turn on
  for (int i = 0; i < time_frequency; i++) {
    lightToTurnOn[i] = random16(0, NUM_LEDS);
  }
 
  int time_fade = 35 - pow((((millis() % animationLength) / 1000) - 5), 1.25); //lights fade less quickly over time to more smoothly transition to the full rainbow
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].fadeToBlackBy(time_fade);
  }
 
  for (int i = 0; i < time_frequency; i++) { //turns on LEDs that were selected
    if (random8() + 60 > (millis() % animationLength - 5000) * 0.09 ) { //initial glitter will be rain colored, then quickly become rainbow colored
      leds[lightToTurnOn[i]] = ledColors[random8(0, ledColorsSize)]; //rain colored
    }
    else {
      leds[lightToTurnOn[i]] = colorsArray[(lightToTurnOn[i] + ledOffset) % NUM_LEDS]; //rainbow colored
    }
  }
}

void glitterStable() {
  int lightToTurnOn[frequency]; //randomly generates which LEDs to turn on
  for (int i = 0; i < frequency; i++) {
    lightToTurnOn[i] = random16(0, NUM_LEDS);
  }
 
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].fadeToBlackBy(fade);
  }
 
  for (int i = 0; i < frequency; i++) { //turns on LEDs that were selected
    leds[lightToTurnOn[i]] = ledColors[random8(0, ledColorsSize)]; //rain colored
  }
}
 
//generic scrolling rainbow
void rainbow() {
  unsigned long ledOffset = millis() * rainbowSpeed / fps / 100 % NUM_LEDS ; // 0 - NUM_LEDS, calculates how far the generated table should be offset from the starting position for the given time
 
  //writes the led array based on the pre-generated table
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = colorsArray[(i + ledOffset) % NUM_LEDS];
  }
}

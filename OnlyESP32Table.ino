// reddit user /u/Procupine 4/7/2019
// the time between transitions is hardcoded (sorry), but the color of the "rain" and speed of the rainbow should be easy to edit
// This code is intended for three rows of LEDs
// I tried my best to comment everything but feel free to pm if you have more questions
 
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "Sorry?";
const char* password = "watzeije?";

#include <FastLED.h>

// Definitions for the led strip
#define LED_PIN 5 //digital pin 5
#define NUM_LEDS 585
#define MAX_BRIGHTNESS 255
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];
CRGB colorsArray[NUM_LEDS]; //pregenerated rainbow to make the acutal rainbow less laggy

int fps = 50; //it takes ~20ms to push all the data to all 579 LEDs so the hard limit is ~50fps
#define animationLength 15000 //milliseconds
int rainbowSpeed = 100; //bigger = faster rainbow

const int size_of_row = NUM_LEDS / 3; //size of the rainbow, divided by 3 for 3 rows of leds

int frequency = 10; //frequency of rain and glitter flashes
int fade = 20;

int deltaRainbow = 5; // For snake how short the rainbow should be.

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

int brightness = 10;

void handleColor(AsyncWebServerRequest *req) {
  int id = 0;
  if (req->hasParam("del") && req->hasParam("id")) {
    // Delete the selected color.
    id = req->getParam("id")->value().toInt();
    for (int i = id; i < ledColorsSize - 1; i++) {
      ledColors[i] = ledColors[i + 1];
    }
    ledColorsSize--;
    ledColors[ledColorsSize] = NULL;
    return;
  } else if (req->hasParam("del") && req->hasParam("all")) {
    // Remove all colors;
    for (int i = 0; i < ledColorsSize; i++) {
      ledColors[i] = NULL;
    }
    ledColorsSize = 0;
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
    
    if (req->hasParam("rgb")) {
      // Add 'rgb' color
      ledColors[id] = NULL;
      return;
    }
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

int snakeInitializationFlag = 1;
String mode = "all";

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
    else if (input.equalsIgnoreCase("snakeAI")) {
      snakeInitializationFlag = 1;
      program = 5;
    }
    mode = input;
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
  if (req->hasParam("delta")) {
    deltaRainbow = req->getParam("delta")->value().toInt();
  }
}

void handleOff(AsyncWebServerRequest *req) {
  on = 0;
}

void handleOn(AsyncWebServerRequest *req) {
  on = 1;
}

String colorsJsonArray() {
  String jsonArray = "[";
  for (int i = 0; i < ledColorsSize; i++) {
    jsonArray += "{ \"id\": \"";
    jsonArray += String(i);
    jsonArray += "\", \"r\": \"";
    CRGB color = ledColors[i];
    jsonArray += String(color.r);
    jsonArray += "\", \"g\": \"";
    jsonArray += String(color.g);
    jsonArray += "\", \"b\": \"";
    jsonArray += String(color.b);
    jsonArray += "\"}";
    if (i + 1 < ledColorsSize) {
      jsonArray += ", ";
    }
  }
  jsonArray += "]";
  return jsonArray;
}

void getColorsJsonArray(AsyncWebServerRequest *req) {
  String jsonArray = colorsJsonArray();
  req->send(200, "application/json", jsonArray);
}

String speedJson() {
  String jsonObject = "\"fps\": ";
  jsonObject += String(fps);
  jsonObject += ", \"freq\": ";
  jsonObject += String(frequency);
  jsonObject += ", \"speed\": ";
  jsonObject += String(rainbowSpeed);
  jsonObject += ", \"fade\": ";
  jsonObject += String(fade);
  jsonObject += ", \"delta\": ";
  jsonObject += String(deltaRainbow);
  return jsonObject;
}

void getSpeedJson(AsyncWebServerRequest *req) {
  String jsonObject = "{";
  jsonObject += speedJson();
  jsonObject += "\"}";
  req->send(200, "application/json", jsonObject);
}

String getMode() {
  
}

void getCurrentPreset(AsyncWebServerRequest *req) {
  String jsonObject = "{";
  jsonObject += speedJson();
  jsonObject += ", \"mode\": \"";
  jsonObject += mode;
  jsonObject += "\", \"brightness\": ";
  jsonObject += String(brightness);
  jsonObject += ", \"colors\":";
  jsonObject += colorsJsonArray();
  jsonObject += "}";
  req->send(200, "application/json", jsonObject);
}

void setup() {
  delay(50); //because life sucks
  Serial.begin(115200);

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

  server.on("/brightness", HTTP_GET, [](AsyncWebServerRequest *req){
    handleBrightness(req);
    req->send(200, "text/plain", "Set new brightness!");
  });

  server.on("/color", [](AsyncWebServerRequest *req) {
    handleColor(req);
    req->send(200, "text/plain", "Set new color");
  });

  server.on("/speed", [](AsyncWebServerRequest *req) {
    handleSpeeds(req);
    req->send(200, "text/plain", "Set new speed");
  });

  server.on("/mode", [](AsyncWebServerRequest *req) {
    handleMode(req);
    req->send(200, "text/plain", "Set new mode");
  });

  server.on("/on", [](AsyncWebServerRequest *req) {
    handleOn(req);
    req->send(200, "text/plain", "Table on");
  });

  server.on("/off", [](AsyncWebServerRequest *req) {
    handleOff(req);
    req->send(200, "text/plain", "Table off");
  });

  server.on("/getColorsArray", [](AsyncWebServerRequest *req) {
    getColorsJsonArray(req);
  });

  server.on("/getSpeeds", [](AsyncWebServerRequest *req) {
    getSpeedJson(req);
  });

  server.on("/getCurrentPreset", [](AsyncWebServerRequest *req) {
    getCurrentPreset(req);
  });

  server.begin();
  Serial.println("HTTP server started");

  ledColors[0] = CRGB(1, 255, 1); //make sure each value is at least 1, that's how the program distinguishes between the rain and the  rainbow

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  MAX_BRIGHTNESS ); // Set the maximum brightness
  FastLED.setBrightness( brightness ); // Set it to a more comfortable level :)
  //initial bias is arbitary and then normalized to the rainbow size. More bias will make the respective section of the rainbow larger
  int BRsize = 90; //section between 100% blue and 100% red
  int RGsize = 100; //section between 100% red and 100% green
  int GBsize = 80; //section between 100% green and 100% blue
  int biasSum = BRsize + RGsize + GBsize;
  BRsize = size_of_row * (float(BRsize) / float(biasSum)); //normalize bias
  RGsize = size_of_row * (float(RGsize) / float(biasSum)); //normalize bias
  GBsize = size_of_row * (float(GBsize) / float(biasSum)); //normalize bias
  biasSum = BRsize + RGsize + GBsize;

  //makes the sum of the bias the same size as the rainbow, fixes "holes" caused by rounding
  if (biasSum < size_of_row) {
    BRsize ++;
    biasSum ++;
    if (biasSum < size_of_row) {
      RGsize ++;
      biasSum ++;
      if (biasSum < size_of_row) {
        GBsize ++;
        biasSum ++;
      }
    }
  }
  else if (biasSum > size_of_row) {
    BRsize --;
    biasSum --;
    if (biasSum > size_of_row) {
      RGsize --;
      biasSum --;
      if (biasSum > size_of_row) {
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
unsigned long ledOffset;
void setLedOffset() {
  ledOffset = millis() * rainbowSpeed / fps / 100 % NUM_LEDS ; // 0 - NUM_LEDS, calculates how far the generated table should be offset from the starting position for the given time
}

CRGB getColor(int led, int useLedColors = 1) {
  if (useLedColors) {
    CRGB color = ledColors[random8(0, ledColorsSize)];
    if (color) {
      return color;
    }
  }
  return colorsArray[(led * deltaRainbow + ledOffset) % NUM_LEDS];
}

void loop() {
  //a timer is used to set a maximum fps
  unsigned long start = millis(); //start timer
  FastLED.show();

  if (snakeInitializationFlag) {
      initialize_snake();
      snakeInitializationFlag = 0;
  }

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
        fill_solid(leds, NUM_LEDS, getColor(0));
        break;
      case 5:
        snakeAI();
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
  setLedOffset();

  for (int i = 0; i < time_frequency; i++) {
    leds[topRow[i]] = getColor(i);
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
  setLedOffset();
  for (int i = 0; i < frequency; i++) {
    
    leds[topRow[i]] = getColor(i);
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
  setLedOffset();
 
  for (int i = 0; i < time_frequency; i++) { //turns on LEDs that were selected
    if (random8() + 60 > (millis() % animationLength - 5000) * 0.09 ) { //initial glitter will be rain colored, then quickly become rainbow colored
      leds[lightToTurnOn[i]] = getColor(0); //rain colored
    }
    else {
      leds[lightToTurnOn[i]] = getColor(lightToTurnOn[i], 0); //rainbow colored
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
  setLedOffset();
 
  for (int i = 0; i < frequency; i++) { //turns on LEDs that were selected
    leds[lightToTurnOn[i]] = getColor(0); //rain colored
  }
}
 
//generic scrolling rainbow
void rainbow() {
  setLedOffset();
 
  //writes the led array based on the pre-generated table
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = getColor(i, 0);
  }
}

// Array of length of the leds, every position correspons to what direction that pixels has to move (iff non black)
// 0 = No path found
// 1 = Left
// 2 = Up
// 3 = Right
// 4 = Down
int bfs[NUM_LEDS];

int apple = 500;

struct Node {
    Node *next;
    int led;
};

struct Node * create_node(int led) {
  Node *newNode = (Node*)malloc(sizeof(Node));
  if (!newNode) {
    Serial.println("No memory for Node");
  }
  newNode->next = NULL;
  newNode->led = led;
  return newNode;
}

struct NextIndices {
  int left;
  int up;
  int right;
  int down;
};

int mod(int a, int b) {
    int r = a % b;
    return r < 0 ? r + b : r;
}

struct NextIndices * get_next_indices (int from) {
  // First see from where 
  NextIndices *next = (NextIndices*)malloc(sizeof(NextIndices));
  if (!next) {
    Serial.println("No memory found for Indices");
  }
  if (from < (size_of_row)) {
    // from is on the first row
    next->left = mod(from + 1, size_of_row);
    next->up = -1;
    next->right = mod(from - 1, size_of_row);
    next->down = from + size_of_row;
  }
  else if (from < (size_of_row * 2)) {
    // from is on the second row
    next->left = mod(from + 1, size_of_row) + size_of_row;
    next->up = from - size_of_row;
    next->right = mod(from - 1, size_of_row) + size_of_row;
    next->down = from + size_of_row;
  }
  else {
    // from is on the last row
    next->left = mod(from + 1, size_of_row) + 2 * size_of_row;
    next->up = from - size_of_row;
    next->right = mod(from - 1, size_of_row) + 2 * size_of_row;
    next->down = -1;
  }
  return next;
}

struct Snake {
  Snake* next;
  int led;
  int* nextMoves;
};

int nextMoveCounter;

struct Snake *snakeHead = NULL;

void initialize_snake() {
  if (snakeHead) {
    free(snakeHead->nextMoves);
    delete_snake(snakeHead);
    snakeHead = NULL;
  }
  nextMoveCounter = 0;
  snakeHead = (Snake*)malloc(sizeof(Snake));
  snakeHead->next = NULL;
  snakeHead->led = 0;
  snakeHead->nextMoves = (int*)malloc(sizeof(int) * NUM_LEDS);
  grow_snake();
  grow_snake();
  memset(leds, 0, sizeof(leds));
  apple = random16(0, NUM_LEDS);
  leds[apple] = CRGB(255, 0, 0);
}

void grow_snake() {
  Snake *newSnakeBlock = (Snake*)malloc(sizeof(Snake));
  if (!newSnakeBlock) {
    Serial.println("No memory for snake!");
  }
  newSnakeBlock->next = NULL;
  Snake *tail = snakeHead;
  while (tail->next) {
    tail = tail->next;
  }
  newSnakeBlock->led = tail->led;
  tail->next = newSnakeBlock;
}

void createPath() {
  int from = apple;
  nextMoveCounter = 0;
  while(from != snakeHead->led) {
    snakeHead->nextMoves[nextMoveCounter] = from;
    nextMoveCounter++;
    from = bfs[from];
  }
}

void delete_snake(struct Snake *snake) {
  Serial.print("Deleting snake with led: ");
  Serial.println(snake->led);
  if (snake->next) {
    delete_snake(snake->next);
    snake->led = 0;
    snake->next = NULL;
  }
  free(snake);
  snake = NULL;
  Serial.println("Done deleting");
}

void moveSnake() {
  nextMoveCounter--;
  // Move the head 1 position, the goto place is at snakeHead.nextMove[nextMoveCounter];
  int lastLed = snakeHead->led;
  snakeHead->led = snakeHead->nextMoves[nextMoveCounter];
  if (snakeHead->led < 0 || leds[snakeHead->led] != CRGB(0,0,0)) {
    if (snakeHead->led == apple) {
      // Ate apple! grow and go on normally (will recalculate path)
      grow_snake();
      apple = random16(0, NUM_LEDS);
      while (leds[apple] != CRGB(0,0,0)) {
        apple = random16(0, NUM_LEDS);
      }
      leds[apple] = CRGB(255, 0, 0);
    } else {
      // Hit snake, restart!
      Serial.println("Hit the snake or wall: ");
      free(snakeHead->nextMoves);
      snakeHead->nextMoves = NULL;
      delete_snake(snakeHead);
      snakeHead = NULL;
      initialize_snake();
      return;
    }
  }
  setLedOffset();
  int counter = 0;
 
  leds[snakeHead->led] = getColor(counter);
  Snake *snake = snakeHead->next;
  while (snake) {
    int tempLed = snake->led;
    snake->led = lastLed;
    lastLed = tempLed;
    leds[snake->led] = getColor(++counter);
    snake = snake->next;
  }
  leds[lastLed] = CRGB(0,0,0);
}

//AI SNAKE!!!
void snakeAI() {
  if (!snakeHead) {
    initialize_snake();
  }
  if (nextMoveCounter > 0) {
    // Path was already found, move the snake (still check for collisions aswell)
    moveSnake();
    return;
  }
  // Perform breadth first search
  Node *node = (Node*)malloc(sizeof(Node));
  node->next = NULL; 
  node->led = snakeHead->led;
  Node *lastNode = node;

  // Reset the bfs array
  memset(bfs, -1, sizeof(bfs));
  bfs[node->led] = node->led;
  // Keep looping while node is not NULL
  while(node) {
    NextIndices *nextIndices = get_next_indices(node->led);
    if (nextIndices->left == apple || nextIndices->up == apple || nextIndices->right == apple || nextIndices->down == apple) {
      // Found the apple! now backtrack to the head of the snake, storing the path in reverse order in nextMoves
      bfs[apple] = node->led;
      free(node);
      free(nextIndices);
      createPath();
      moveSnake();
      return;
    }
    if (leds[nextIndices->left] == CRGB(0,0,0) && !(bfs[nextIndices->left] >= 0)) {
      // Check if to the left is a black led, and it hasn't been discovered yet.
      bfs[nextIndices->left] = node->led;
      lastNode->next = create_node(nextIndices->left);
      lastNode = lastNode->next;
    }
    if (leds[nextIndices->right] == CRGB(0,0,0) && !(bfs[nextIndices->right] >= 0)) {
      // Check if to the left is a black led, and it hasn't been discovered yet.
      bfs[nextIndices->right] = node->led;
      lastNode->next = create_node(nextIndices->right);
      lastNode = lastNode->next;
    }
    if (nextIndices->up >= 0 && leds[nextIndices->up] == CRGB(0,0,0) && !(bfs[nextIndices->up] >= 0)) {
      // Check if to the left is a black led, and it hasn't been discovered yet.
      bfs[nextIndices->up] = node->led;
      lastNode->next = create_node(nextIndices->up);
      lastNode = lastNode->next;
    }
    if (nextIndices->down >= 0 && leds[nextIndices->down] == CRGB(0,0,0) && !(bfs[nextIndices->down] >= 0)) {
      // Check if to the left is a black led, and it hasn't been discovered yet.
      bfs[nextIndices->down] = node->led;
      lastNode->next = create_node(nextIndices->down);
      lastNode = lastNode->next;
    }
    Node *tempNode = node;
    node = node->next;
    free(tempNode);
    free(nextIndices);
  }
  Serial.println("No path found");
  // Couldn't find a path! move to the first available position (order being left right up down) and try again, if completly stuck, it moves down (and gets killed)
  NextIndices *nextIndices = get_next_indices(snakeHead->led);
  if (leds[nextIndices->left] == CRGB(0,0,0)) {
    snakeHead->nextMoves[0] = nextIndices->left;
  }
  else if (leds[nextIndices->right] == CRGB(0,0,0)) {
    snakeHead->nextMoves[0] = nextIndices->right;
  }
  else if (nextIndices->up >= 0 && leds[nextIndices->up] == CRGB(0,0,0)) {
    snakeHead->nextMoves[0] = nextIndices->up;
  }
  else {
    snakeHead->nextMoves[0] = nextIndices->down;
  }
  free(nextIndices);
  nextMoveCounter = 1;
  moveSnake();
}

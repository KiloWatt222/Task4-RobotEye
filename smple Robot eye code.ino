#include <Adafruit_NeoPixel.h>
#include <LiquidCrystal.h>

#define LED 6       // input pin Neopixel is attached to
#define SRT_STP 3  // input pin for start and stopping light display. Pin 3 interrupt trigger
#define UP 4     // input pin for up screen display selector
#define DOWN 5     // input pin for down screen display selector
#define INCREMENT 2  // input pin for incrementing color value

#define NUMPIXELS 59 // number of neopixels in Ring

int delayVal = 2;    // timing delay

boolean isRunning = false;
boolean isLightsOn = false;
int upstate = 0;
int downstate = 0;
int redColor = 128;
int greenColor = 128;
int blueColor = 128;
int redLast = 0;
int greenLast = 0;
int blueLast = 0;
int redIncrement = 0;
int greenIncrement = 0;
int blueIncrement = 0;
int incrementValue = 32;
int pixelNumber = 0;
int positionCtr = 0;
int colorNumber = 0;
static int cycle1Count = 0;
static int cycle2Count = 0;
static int snowflakesCount = 0;
static int iceflakesCount = 0;
static int candyChaseCount = 0;
static int rainbowCycleCount = 0;

int seqNum = 0;
int seqNumLast = -1;
char seqArray[8][12] = {"CYCLE 1", "CYCLE 2", "SNOWFLAKE", "ICEFLAKE", "CANDY CHASE", "RAINBOW", "PICK COLOR", " "};

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, 12, 11, 10, 9);  //RS,EN,D4,D5,D6,D7
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LED, NEO_GRB + NEO_KHZ800);

void setup() {
    pixels.begin(); // Initializes the NeoPixel library.
    Serial.begin(9600);
    //TODO: setup LCD to select which routine to run
    lcd.begin(16, 2);  // set up the LCD's number of columns and rows: 
    //Buttons UP and DOWN menu buttons and STOP/START button.
    pinMode(UP, INPUT_PULLUP);
    pinMode(DOWN, INPUT_PULLUP);
    pinMode(SRT_STP, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(SRT_STP), Start_Stop, RISING);
    attachInterrupt(digitalPinToInterrupt(INCREMENT), Increment_Color, RISING);
}

void loop() {
    if (isRunning)
    {
      if (!isLightsOn)
      {
        pixels.setBrightness(128);
        pixels.show();
      }
      isLightsOn = true;

      switch (seqNum)
      {
          case 0:
              cycle1();
              break;
          case 1:
              cycle2();
              break;
          case 2:
              snowflakes(10);
              break;
          case 3:
              iceflakes(10);
              break;
          case 4:
              candyChase(10);
              break;
          case 5:
              rainbowCycle(2);
              break;
          case 6:
            pickColor();
            break;
      }
    }
    else
    {
      if (isLightsOn)
      {
        pixels.setBrightness(0);
        pixels.show();
      }
      isLightsOn = false;

      upstate = digitalRead(UP);
      downstate = digitalRead(DOWN);

      if (upstate == HIGH && seqNum > 0)
          {seqNum = seqNum - 1;}
      else if (downstate == HIGH && seqNum < 6)
          {seqNum = seqNum + 1;}

    if (seqNum != seqNumLast)
      {
        lcd.clear();
        lcd.setCursor(0,0);  // set the cursor to column 15, line 0
        lcd.print(seqArray[seqNum]); //strcat(seqArray[seqNum], "*"));  // Print a message to the LCD.

        lcd.setCursor(0,1);
        lcd.print(seqArray[seqNum + 1]);
    seqNumLast = seqNum;
      }
    }
}

// Play the recorded sequence once
void Start_Stop() {
    // For some reason, at least in the simulation, the interrupt is called twice on a single
    // button click.  Added the doubleClick flag to negate the second occurance here. Actual
    // circuit may not need this check and this should be removed./*
  isRunning = !isRunning;

//  Serial.print("isRunning = ");
//  Serial.println(isRunning);
}

// Cycle through the colors in the pickColor sequence
void Increment_Color() {
  colorNumber++;
  if (colorNumber > 7) {
    colorNumber = 0;
  }
}

void cycle1() {
    setColor();

    redColor = redColor + redIncrement;
    if (redColor < 0 || redColor > 255)
    {
      redColor = redColor - (2 * redIncrement);
    }

    greenColor = greenColor + greenIncrement;
    if (greenColor < 0 || greenColor > 255)
    {
      greenColor = greenColor - (2 * greenIncrement);
    }

    blueColor = blueColor + blueIncrement;
    if (blueColor < 0 || blueColor > 255)
    {
      blueColor = blueColor - (2 * blueIncrement);
    }
  
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.fill(pixels.Color(redColor, greenColor, blueColor), 0, pixels.numPixels()); // Moderately bright green color.
    pixels.show(); // This sends the updated pixel color to the hardware.

//  delay(delayVal); // Delay for a period of time (in milliseconds).
}

void cycle2() {
    uint16_t i, j, color;

    for(j = 0; j < 256; j = j + 1) { // cycle of all colors on wheel
      if (!isRunning) //stop has been pressed
      {
        break;
      }
    color = Wheel(j);

      pixels.fill(color, 0, pixels.numPixels());
      pixels.show(); // This sends the updated pixel color to the hardware.
  }
  
    if (pixelNumber > NUMPIXELS)
    {
      pixelNumber = 0;
    }
}

void snowflakes(uint8_t wait) {
    // Setup the pixel array
    int pixel[60];
    for(int p = 0; p < 60; p++){
      if (!isRunning) //stop has been pressed
      {
        break;
      }
      pixel[p] = random(0,255); 
    }

    // Run some snowflake cycles
    for (int j = 0; j < 200; j++) {
      if (!isRunning) //stop has been pressed
      {
        break;
      }

      // Every five cycles, light a new pixel
      if((j%5)==0){
        pixels.setPixelColor(random(0,60), 255,255,255);
      }

      // Dim all pixels by 10
      for(int p = 0; p < 60; p++){
        if (!isRunning) //stop has been pressed
        {
          break;
        }
        pixels.setPixelColor(p, pixel[p],pixel[p],pixel[p] );
        pixel[p] =  pixel[p] - 10;
      }
      pixels.show();
//      delay(wait);
    }

}

void iceflakes(uint8_t wait) {
    // Setup the pixel array
    int pixel[60];
    for(int p = 0; p < 60; p++){
      if (!isRunning) //stop has been pressed
      {
        break;
      }
      pixel[p] = random(0,255); 
    }

    // Run some snowflake cycles
    for (int j = 0; j < 200; j++) {
      if (!isRunning) //stop has been pressed
      {
        break;
      }

      // Every five cycles, light a new pixel
      if((j%5)==0){
        pixels.setPixelColor(random(0,60), 0,0,255);
      }

      // Dim all pixels by 10
      for(int p = 0; p < 60; p++){
        if (!isRunning) //stop has been pressed
        {
          break;
        }
        pixels.setPixelColor(p, 0,0,pixel[p] );
        pixel[p] =  pixel[p] - 10;
      }
      pixels.show();
//      delay(wait);
    }

}

void candyChase(uint8_t wait) {
    for (int j = 0; j < 10; j++) {  //do 10 cycles of chasing
      if (!isRunning) //stop has been pressed
      {
        break;
      }
      for (int q = 0; q < 3; q++) {
        if (!isRunning) //stop has been pressed
        {
          break;
        }
        for (uint16_t i = 0; i < pixels.numPixels(); i++) {
          pixels.setPixelColor(i+q, 255,255,255);    //turn every pixel white
          if (!isRunning) //stop has been pressed
          {
            break;
          }
        }
        for (uint16_t i = 0; i < pixels.numPixels(); i = i + 3) {
          pixels.setPixelColor(i+q, 255,0,0);    //turn every third pixel red
          if (!isRunning) //stop has been pressed
          {
            break;
          }
        }
        pixels.show();

        delay(wait);

        for (uint16_t i = 0; i < pixels.numPixels(); i = i + 3) {
          pixels.setPixelColor(i+q, 0);        //turn every third pixel off
          if (!isRunning) //stop has been pressed
          {
            break;
          }
        }
      }
    }
}

void rainbowCycle(uint8_t wait) {
    uint16_t i, j;

    for(j = 0; j < 256; j = j + 10) { // cycle of all colors on wheel
      for(i = 0; i < pixels.numPixels(); i++) {
        pixels.setPixelColor(i, Wheel(((i * 256 / pixels.numPixels()) + j) & 255));
        if (!isRunning) //stop has been pressed
        {
          break;
        }
      }
      pixels.show();
      if (!isRunning) //stop has been pressed
      {
        break;
      }
//      delay(wait);
    }
}

void pickColor() {
    switch(colorNumber){
      case 0:       // Red
          redColor = 255;
          greenColor = 0;
          blueColor = 0;
          break;
      case 1:       // Orange
          redColor = 255;
          greenColor = 70;
          blueColor = 0;
          break;
      case 2:       // Yellow
          redColor = 255;
          greenColor = 255;
          blueColor = 0;
          break;
      case 3:       // Green
          redColor = 0;
          greenColor = 255;
          blueColor = 0;
          break;
      case 4:       // Blue
          redColor = 0;
          greenColor = 0;
          blueColor = 255;
          break;
      case 5:       // Indigo
          redColor = 30;
          greenColor = 0;
          blueColor = 100;
          break;
      case 6:       // Violet
          redColor = 255;
          greenColor = 0;
          blueColor = 255;
          break;
      case 7:       // White
          redColor = 255;
          greenColor = 255;
          blueColor = 255;
          break;
    }
  
    // pixels.Color takes RGB values, from 0,0,0 up to 255,255,255
    pixels.fill(pixels.Color(redColor, greenColor, blueColor), 0, pixels.numPixels()); // Moderately bright.
    pixels.show(); // This sends the updated pixel color to the hardware.
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
    WheelPos = 255 - WheelPos;
    if(WheelPos < 85) {
      return pixels.Color(255 - WheelPos * 3, 0, WheelPos * 3);
    }
    if(WheelPos < 170) {
      WheelPos -= 85;
      return pixels.Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
    WheelPos -= 170;
    return pixels.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

//picks random values to set for RGB
void setColor(){
    redColor = random(0, 255);
    greenColor = random(0,255);
    blueColor = random(0, 255);
  //  Serial.print("red: ");
  //  Serial.println(redColor);
  //  Serial.print("green: ");
  //  Serial.println(greenColor);
  //  Serial.print("blue: ");
  //  Serial.println(blueColor);

}

//picks random values to set for RGB incremnt
void setIncrement(){
    redIncrement = random(-10, 10);
    greenIncrement = random(-10,10);
    blueIncrement = random(-10, 10);
}

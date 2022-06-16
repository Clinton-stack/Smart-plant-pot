#include <Adafruit_GFX.h>    // Core graphics library
#include "Adafruit_SSD1351.h" //Includes hardware specific library
#include <SPI.h>             // Arduino SPI library
#include <DHT.h>
#include <gfxfont.h>
#include <Fonts/FreeMonoBoldOblique12pt7b.h>
#include <Fonts/FreeSerif9pt7b.h>
#include "SD.h"
#define SD_ChipSelectPin 53
#include "TMRpcm.h"
#include "muMatrix8ptRegular.h"


// Swap these definitions if using a common anode RGB LED
#define LED_ON    HIGH
#define LED_OFF   LOW

// Arduino digital pins that control red, green and blue colors:
const int LED_R = 5;
const int LED_B = 3;
const int LED_G = 4;


#define DHTPIN 11     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino

TMRpcm tmrpcm;


//Variables
int chk;
float hum;  //Stores humidity value
float temp; //Stores temperature value
int trans_flag = 0;
const int buttonPin = 39;     // the number of the pushbutton pin
int buttonState;         // variable for reading the pushbutton status
int heatwaveTemp = 19;
int outsideTemp = 29;


extern const uint16_t happyface[];
extern const uint16_t insidehot[];
extern const uint16_t water[];
extern const uint16_t heatwave[];
extern const uint16_t outsidehot[];


//screen dimensions
#define SCREEN_WIDTH 128 //pixel width
#define SCREEN_HEIGHT 128 //pixel height

//pin definitions
#define SCLK_PIN 52 //defines s clock pin
#define MOSI_PIN 51 //defines master-out slave-in SDA pin
#define RST_PIN   8 //defines reset pin
#define DC_PIN    7 //defines master-in slave-out pin
#define CS_PIN    10 //defines chip select pin

// Colour definitions
#define BLACK           0x0000
#define BLUE            0x001F
#define RED             0xF800
#define GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0
#define WHITE           0xFFFF
#define GREY            0x8410
#define ORANGE          0xE880


Adafruit_SSD1351 display = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, CS_PIN, DC_PIN, RST_PIN);

const int dry = 595; // value for dry sensor
const int wet = 239; // value for wet sensor

void setup(void) {

// Configure pins connected to LED R, G and B as outputs
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);
  digitalWrite(46, LOW);


  
  Serial.begin(9600);
  display.begin();
  display.fillScreen(WHITE);
  dht.begin();
  
  //display.setFont(&muMatrix8ptRegular);  // choose font

  display.setTextSize(3);          // text size
  display.setTextColor(BLACK);     // text color
  display.setCursor(15, 50);       // position to display

  pinMode(buttonPin, INPUT_PULLUP);
  Serial.println(" button state at setup:" + buttonState);


  tmrpcm.speakerPin = 46;
  if (!SD.begin(SD_ChipSelectPin)) {
    Serial.println("SD fail");
    return;
  }
  Serial.println("SD card is working");
  tmrpcm.setVolume(5);
}




void loop() {


  //Read data and store it to variables hum and temp
  hum = dht.readHumidity();
  temp = dht.readTemperature();
  int sensorVal = analogRead(A0);
  const int dry = 480; // value for dry sensor
  const int wet = 236; // value for wet sensor
  int percentageHumidity = map(sensorVal, wet, dry, 100, 0);
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin); //off:0, on:1 

      Serial.println(" button state:" + buttonState);


  if (temp > 28) {
    display.drawRGBBitmap(15, 15, insidehot, 100, 100);
      digitalWrite(LED_R, LED_ON);
  } else if (percentageHumidity < 20) {
    display.drawRGBBitmap(15, 15, water, 100, 100);
     // Blue on
  //digitalWrite(LED_G, LED_OFF);
  digitalWrite(LED_B, LED_ON);
  }
  else if (outsideTemp > 29 ) {
     digitalWrite(LED_R, LED_ON);
    display.drawRGBBitmap(15, 15, outsidehot, 100, 100);
  }
  else if (heatwaveTemp > 19 ) {
      digitalWrite(LED_R, LED_ON);
    display.drawRGBBitmap(15, 15, heatwave, 100, 100);
  }
  else {
     // Green on
  digitalWrite(LED_R, LED_OFF);
  digitalWrite(LED_B, LED_OFF);
  digitalWrite(LED_G, LED_ON);
    display.drawRGBBitmap(15, 15, happyface, 100, 100);
  }



  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == 0) {
    Serial.println(" button was pushed");
    Serial.println(" button state:" + buttonState);
    Serial.println("button pin " + buttonPin);


    tmrpcm.stopPlayback();
    
    
    if ( percentageHumidity < 20) {
      display.fillScreen(WHITE);
      oledDisplayCenter("Please water me!");
//      delay(4000);
//      oledDisplayCenter("Mag ik wat water, alsjeblieft?");
      tmrpcm.play((char*)"3.wav");
      delay(8000);
      tmrpcm.play((char*)"4.wav");
      delay(10000); //Delay 10 sec.
      display.fillScreen(WHITE);
    }
    else if ( temp > 28) {
      // turn LED on:
      display.fillScreen(WHITE);
      oledDisplayCenter("It’s cooler outside now");
      tmrpcm.play((char*)"7.wav");
      delay(8000);
      tmrpcm.play((char*)"8.wav");

      delay(10000); //Delay 2 sec.
      display.fillScreen(WHITE);
    }
    else if (outsideTemp > 29 ) {
      display.fillScreen(WHITE);
      oledDisplayCenter("It’s getting hot outside!");
      tmrpcm.play((char*)"9.wav");
      delay(8000);
      tmrpcm.play((char*)"10.wav");

      delay(10000); //Delay 2 sec.
      display.fillScreen(WHITE);
    }
    else if (heatwaveTemp > 19 ) {
      display.fillScreen(WHITE);
      oledDisplayCenter("There is a heat wave expected!");
      tmrpcm.play((char*)"5.wav");
      delay(33000);
      tmrpcm.play((char*)"6.wav");

      delay(33000); //Delay 2 sec.
      display.fillScreen(WHITE);
    }
    else {
      display.fillScreen(WHITE);
      oledDisplayCenter("Everything is Good!!");
      tmrpcm.play((char*)"1.wav");
      delay(8000);
      tmrpcm.play((char*)"2.wav");
      delay(10000); //Delay 10 sec.
    
      display.fillScreen(WHITE);
    }



    //    tmrpcm.play((char*)"3.wav");
    //    display.fillScreen(WHITE);
    //    oledDisplayCenter((String)"Humidity: " + hum + "% , Temp: " + temp + " Celsuis, " + "Soil Moisture: "+ percentageHumidity);
    //    delay(8000); //Delay 2 sec.
    //    display.fillScreen(WHITE);
  } else{

    digitalWrite(46, LOW);
//   tmrpcm.play((char*)"11.wav", 50);
//        Serial.println(" silence playing");
//
//    delay(12000); //Delay 1 min.

  }
      
//      Serial.print("Humidity: ");
//      Serial.print(hum);
//      Serial.print(" %, Temp: ");
//      Serial.print(temp);
//      Serial.println(" Celsius");
//       delay(3000); //Delay 1min.
//
//    Serial.print(percentageHumidity);
//    Serial.println("%");

}

void oledDisplayCenter(String text) {
  int16_t x1;
  int16_t y1;
  uint16_t width;
  uint16_t height;

  display.getTextBounds(text, 0, 0, &x1, &y1, &width, &height);

  display.setCursor((SCREEN_WIDTH - width) / 2, (SCREEN_HEIGHT - height) / 2);
  display.println(text); // text to display
}

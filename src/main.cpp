#include <Arduino.h>

#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>
#include <TinyGPS++.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <ESP32Servo.h>

using namespace std;

#define S0 32
#define S1 33
#define Out 27
#define S2 26
#define S3 25
#define IR 35
#define RXD2 16
#define TXD2 17
#define TFT_DC      2     
#define TFT_RST     4     
#define TFT_CS     23

HardwareSerial neogps(1);
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// setup the neo-gps
TinyGPSPlus gps;

float pi = 3.1415926;

int pos = 0;

int on = false;

int sensor = 1;

const char *ssid = "realme 9 Pro 5G";
const char *password = "z833cw4w";
// const char *ssid = "Haleluya";
// const char *password = "bismillah";


// Google script ID and required credentials
String GOOGLE_SCRIPT_ID = "AKfycbyr48bsFpnYg2lPdlOwnD0SrerXouHf7-vivWSiGKyiz6M3GeN74LbnubV3T1QFKFG5wg"; // change Gscript ID

// Stores frequency read by the photodiodes
int red = 0;
int green = 0;
int blue = 0;
int freq = 0;

// put function declarations here:
int getRed();
int getGreen();
int getBlue();
int sendHttpRequest(int red, int green, int blue);
uint16_t* sendRequestImage(bool hasPython);
void sendHTTPRequestGPS(double latitude, double longitude);
void visualisasi_GPS_lcd (void);
void visualisasi_GPS_Serial (void);
void testlines(uint16_t color);
void testdrawtext(char *text, uint16_t color);
void testfastlines(uint16_t color1, uint16_t color2);
void testdrawrects(uint16_t color);
void testfillrects(uint16_t color1, uint16_t color2);
void testfillcircles(uint8_t radius, uint16_t color);
void testdrawcircles(uint8_t radius, uint16_t color);
void testtriangles();
void testroundrects();
void tftPrintTest();
void mediabuttons();
void move(int pos1, int pos2);

// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);
Servo myServo;

char units[5];
unsigned long last_time = 0;
unsigned long current_time = 0;

bool isCollectingData = false;
bool isGetGPS = true;

// uint16_t pict[200];

// StaticJsonDocument<200> jsonDocument;
// StaticJsonDocument<288> jsonResponse;

unsigned long startTime;

int servoPos = 0;
void setup()
{
  Serial.begin(115200);
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);

  // =========== Setting the sensorOut as an input ===========
  pinMode(Out, INPUT);
  pinMode(IR, INPUT);
  // ============ Setting frequency scaling to 20% =================
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);

  
  Serial.println(String(ESP.getFreeHeap())+" Bytes");
  delay(10);

  Wire.begin();

  // begin serial of gps
  neogps.begin(9600);

  // initialize LCD
  lcd.init();
  // turn on LCD backlight
  lcd.backlight();

  // //Begin serial communication Neo6mGPS
  neogps.begin(9600, SERIAL_8N1, RXD2, TXD2);

  // setup servo
  myServo.attach(13);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");

  // initialize tft
  tft.init(240, 240, SPI_MODE3);
  tft.setRotation(2);

  // Display a QR Code
  // QRCode qrcode;
  // uint8_t qrcodeData[qrcode_getBufferSize(3)]; // Use larger ECC level and size
  // qrcode_initText(&qrcode, qrcodeData, 3, ECC_MEDIUM, "https://www.example.com"); // Change this URL

  tft.fillScreen(ST77XX_BLACK);
  startTime = millis();
}

String value = "";
void loop()
{
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }
  if (isGetGPS){
    if (neogps.available()>0){
      if (gps.encode(neogps.read())){
        if (gps.location.isValid()){
          // Serial.print("Latitude: ");
          // Serial.println(gps.location.lat(), 6);
          // Serial.print("Longitude: ");
          // Serial.println(gps.location.lng(), 6);
          unsigned long currentMillis = millis();
          if (currentMillis - startTime >2000){
            startTime = currentMillis;
            sendHTTPRequestGPS(gps.location.lat(), gps.location.lng());
          }
        }
      }
    }
  }
  

  // put your main code here, to run repeatedly:
  sensor = digitalRead(IR);
  if (sensor == 0)
  {
    on = true;

    red = getRed();
    delay(100); /*wait a 200mS*/
    green = getGreen();
    delay(100); /*wait a 200mS*/
    blue = getBlue();
    delay(100);        /*wait a 200mS*/
    Serial.print(red); /*Print Red Color Value on Serial Monitor*/
    Serial.print(",");
    Serial.print(green); /*Print Green Color Value on Serial Monitor*/
    Serial.print(",");
    Serial.println(blue); /*Print Blue Color Value on Serial Monitor*/
    
    lcd.clear();
    String rgb = String(red) + "," + String(green) + "," + String(blue);
    lcd.setCursor(0, 0);
    lcd.print(rgb);
    if (isCollectingData){
      // String urlFinal = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?" + "red=" + String(red) + "&green=" + String(green) + "&blue=" + String(blue);
      // HTTPClient http;
      // http.begin(urlFinal.c_str());
      // http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
      // int httpCode = http.GET();

      // String payload;
      // if (httpCode > 0)
      // {
      //   payload = http.getString();
      //   Serial.println("Payload: " + payload);
      // }
      // //---------------------------------------------------------------------
      // http.end();
      // Serial.println();
      
      Serial.println(rgb);
    } else{
      int httpResp = sendHttpRequest(red, green, blue);
      if (httpResp > 0){
        move(-50, 150);
        delay(2000);
        move(150, -50);
      }
      delay(500);
    }
    
  }
}

void move(int pos1,int pos2){
  if (pos2-pos1 >= 0){
    for (int i = pos1; i <= pos2; i++){
      myServo.write(i);
      delay(15);
    }
  } else{
    for (int i = pos1; i >= pos2; i--){
      myServo.write(i);
      delay(15);
    }
  }
}

int getRed()
{
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  freq = pulseIn(Out, LOW); /*Get the Red Color freq*/
  return freq;
}

int getGreen()
{
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  freq = pulseIn(Out, LOW); /*Get the Green Color freq*/
  return freq;
}

int getBlue()
{
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  freq = pulseIn(Out, LOW); /*Get the Blue Color freq*/
  return freq;
}

void sendHTTPRequestGPS(double latitude, double longitude)
{
  HTTPClient http1;
  String url = "https://infastq-api-production.up.railway.app/api/location/";
  http1.begin(url);
  http1.addHeader("Content-Type", "application/json");

  String jsonString;
  DynamicJsonDocument jsonReq(50);
  jsonReq["latitude"] = latitude;
  jsonReq["longitude"] = longitude;
  serializeJson(jsonReq, jsonString);
  int httpResponseCode = http1.POST(jsonString);
  if (httpResponseCode > 0)
  {
    String response = http1.getString();
    Serial.println("HTTP Response: " + response);
    // Parse the JSON response
    DynamicJsonDocument jsonResp(50);
    deserializeJson(jsonResp, response);

    // Extract the "data" value
    String latitude = jsonResp["latitude"];
    String longitude = jsonResp["longitude"];
    // Serial.print("Latitude: ");
    // Serial.println(latitude);
    // Serial.print("Longitude: ");
    // Serial.println(longitude);
    jsonResp.clear();
  }
  else
  {
    Serial.print("HTTP Error: ");
    Serial.println(httpResponseCode);
  }
  http1.end();
  jsonReq.clear();
}

int sendHttpRequest(int red, int green, int blue)
{
  HTTPClient http;
  http.begin("https://infastq-api-production.up.railway.app/api/calculate/"); // Change this to your server URL
  http.addHeader("Content-Type", "application/json");

  // Create JSON payload
  DynamicJsonDocument jsonDoc(200);
  jsonDoc["red"] = red;
  jsonDoc["green"] = green;
  jsonDoc["blue"] = blue;
  String jsonString;
  serializeJson(jsonDoc, jsonString);

  // Send POST request
  int httpResponseCode = http.POST(jsonString);

  if (httpResponseCode > 0)
  {
    String response = http.getString();
    Serial.println("HTTP Response: " + response);
    // Parse the JSON response
    DynamicJsonDocument jsonResp(200);
    deserializeJson(jsonResp, response);

    // Extract the "data" value
    String value = jsonResp["data"];
    Serial.print("Data Value: ");
    Serial.println(value);
    lcd.setCursor(0,1);
    lcd.print(value);
    jsonResp.clear();
  }
  else
  {
    Serial.print("HTTP Error: ");
    Serial.println(httpResponseCode);
  }

  jsonDoc.clear();
  http.end();
  return httpResponseCode;
}

//------------------------------------------------------------------------------------------------------------------------------------------

void visualisasi_GPS_lcd (void)
{
 lcd.clear();
 lcd.print("LAT: ");
 lcd.print(gps.location.lat(),6);
 lcd.setCursor (0, 1);
 lcd.print("LON: "); 
 lcd.print(gps.location.lng(),6);  
 delay (2000);
 lcd.clear();
 lcd.print("VEL: ");
 lcd.print(gps.speed.kmph());
 lcd.print("Km/h");
 lcd.setCursor (0, 1);
 lcd.print("SAT: "); 
 lcd.print(gps.satellites.value());
 delay (2000);
 lcd.clear();
 lcd.print("ALT: ");
 lcd.print(gps.altitude.meters(), 0); 
 delay (2000);
 lcd.clear();
 lcd.print("HOR: "); 
 lcd.print(gps.time.hour());
 lcd.print(":");
 lcd.print(gps.time.minute());
 lcd.print(":");
 lcd.print(gps.time.second());
 lcd.setCursor (0, 1);
 lcd.print("FEC: "); 
 lcd.print(gps.date.day());
 lcd.print("/");
 lcd.print(gps.date.month());
 lcd.print("/");
 lcd.print(gps.date.year()); 
 delay (2000);
}

//------------------------------------------------------------------------------------------------------------------------------------------

void Visualisasi_GPS_Serial(void)
{ 
   
  if (gps.location.isUpdated())
  {  
    Serial.print("Lat: ");
    Serial.println(gps.location.lat(),6);
    Serial.print("Lng: ");
    Serial.println(gps.location.lng(),6);  
    Serial.print("Speed: ");
    Serial.println(gps.speed.kmph());    
    Serial.print("SAT:");
    Serial.println(gps.satellites.value());
    Serial.print("ALT:");   
    Serial.println(gps.altitude.meters(), 0);     

    Serial.print("Date: ");
    Serial.print(gps.date.day()); Serial.print("/");
    Serial.print(gps.date.month()); Serial.print("/");
    Serial.println(gps.date.year());

    Serial.print("Hour: ");
    Serial.print(gps.time.hour()); Serial.print(":");
    Serial.print(gps.time.minute()); Serial.print(":");
    Serial.println(gps.time.second());
    Serial.println("---------------------------");
  }
  else
  {
    Serial.println("No GPS Signal");  
  }  
}

void testlines(uint16_t color) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x = 0; x < tft.width(); x += 6) {
    tft.drawLine(0, 0, x, tft.height() - 1, color);
    delay(0);
  }
  for (int16_t y = 0; y < tft.height(); y += 6) {
    tft.drawLine(0, 0, tft.width() - 1, y, color);
    delay(0);
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x = 0; x < tft.width(); x += 6) {
    tft.drawLine(tft.width() - 1, 0, x, tft.height() - 1, color);
    delay(0);
  }
  for (int16_t y = 0; y < tft.height(); y += 6) {
    tft.drawLine(tft.width() - 1, 0, 0, y, color);
    delay(0);
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x = 0; x < tft.width(); x += 6) {
    tft.drawLine(0, tft.height() - 1, x, 0, color);
    delay(0);
  }
  for (int16_t y = 0; y < tft.height(); y += 6) {
    tft.drawLine(0, tft.height() - 1, tft.width() - 1, y, color);
    delay(0);
  }

  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x = 0; x < tft.width(); x += 6) {
    tft.drawLine(tft.width() - 1, tft.height() - 1, x, 0, color);
    delay(0);
  }
  for (int16_t y = 0; y < tft.height(); y += 6) {
    tft.drawLine(tft.width() - 1, tft.height() - 1, 0, y, color);
    delay(0);
  }
}

void testdrawtext(char *text, uint16_t color) {
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.setTextWrap(true);
  tft.print(text);
}

void testfastlines(uint16_t color1, uint16_t color2) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t y = 0; y < tft.height(); y += 5) {
    tft.drawFastHLine(0, y, tft.width(), color1);
  }
  for (int16_t x = 0; x < tft.width(); x += 5) {
    tft.drawFastVLine(x, 0, tft.height(), color2);
  }
}

void testdrawrects(uint16_t color) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x = 0; x < tft.width(); x += 6) {
    tft.drawRect(tft.width() / 2 - x / 2, tft.height() / 2 - x / 2 , x, x, color);
  }
}

void testfillrects(uint16_t color1, uint16_t color2) {
  tft.fillScreen(ST77XX_BLACK);
  for (int16_t x = tft.width() - 1; x > 6; x -= 6) {
    tft.fillRect(tft.width() / 2 - x / 2, tft.height() / 2 - x / 2 , x, x, color1);
    tft.drawRect(tft.width() / 2 - x / 2, tft.height() / 2 - x / 2 , x, x, color2);
  }
}

void testfillcircles(uint8_t radius, uint16_t color) {
  for (int16_t x = radius; x < tft.width(); x += radius * 2) {
    for (int16_t y = radius; y < tft.height(); y += radius * 2) {
      tft.fillCircle(x, y, radius, color);
    }
  }
}

void testdrawcircles(uint8_t radius, uint16_t color) {
  for (int16_t x = 0; x < tft.width() + radius; x += radius * 2) {
    for (int16_t y = 0; y < tft.height() + radius; y += radius * 2) {
      tft.drawCircle(x, y, radius, color);
    }
  }
}

void testtriangles() {
  tft.fillScreen(ST77XX_BLACK);
  int color = 0xF800;
  int t;
  int w = tft.width() / 2;
  int x = tft.height() - 1;
  int y = 0;
  int z = tft.width();
  for (t = 0 ; t <= 15; t++) {
    tft.drawTriangle(w, y, y, x, z, x, color);
    x -= 4;
    y += 4;
    z -= 4;
    color += 100;
  }
}

void testroundrects() {
  tft.fillScreen(ST77XX_BLACK);
  int color = 100;
  int i;
  int t;
  for (t = 0 ; t <= 4; t += 1) {
    int x = 0;
    int y = 0;
    int w = tft.width() - 2;
    int h = tft.height() - 2;
    for (i = 0 ; i <= 16; i += 1) {
      tft.drawRoundRect(x, y, w, h, 5, color);
      x += 2;
      y += 3;
      w -= 4;
      h -= 6;
      color += 1100;
    }
    color += 100;
  }
}

void tftPrintTest() {
  tft.setTextWrap(false);
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 30);
  tft.setTextColor(ST77XX_RED);
  tft.setTextSize(1);
  tft.println("Hello World!");
  tft.setTextColor(ST77XX_YELLOW);
  tft.setTextSize(2);
  tft.println("Hello World!");
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(3);
  tft.println("Hello World!");
  tft.setTextColor(ST77XX_BLUE);
  tft.setTextSize(4);
  tft.print(1234.567);
  delay(1500);
  tft.setCursor(0, 0);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(0);
  tft.println("Hello World!");
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_GREEN);
  tft.print(pi, 6);
  tft.println(" Want pi?");
  tft.println(" ");
  tft.print(8675309, HEX); // print 8,675,309 out in HEX!
  tft.println(" Print HEX!");
  tft.println(" ");
  tft.setTextColor(ST77XX_WHITE);
  tft.println("Sketch has been");
  tft.println("running for: ");
  tft.setTextColor(ST77XX_MAGENTA);
  tft.print(millis() / 1000);
  tft.setTextColor(ST77XX_WHITE);
  tft.print(" seconds.");
}

void mediabuttons() {
  // play
  tft.fillScreen(ST77XX_BLACK);
  tft.fillRoundRect(25, 10, 78, 60, 8, ST77XX_WHITE);
  tft.fillTriangle(42, 20, 42, 60, 90, 40, ST77XX_RED);
  delay(500);
  // pause
  tft.fillRoundRect(25, 90, 78, 60, 8, ST77XX_WHITE);
  tft.fillRoundRect(39, 98, 20, 45, 5, ST77XX_GREEN);
  tft.fillRoundRect(69, 98, 20, 45, 5, ST77XX_GREEN);
  delay(500);
  // play color
  tft.fillTriangle(42, 20, 42, 60, 90, 40, ST77XX_BLUE);
  delay(50);
  // pause color
  tft.fillRoundRect(39, 98, 20, 45, 5, ST77XX_RED);
  tft.fillRoundRect(69, 98, 20, 45, 5, ST77XX_RED);
  // play color
  tft.fillTriangle(42, 20, 42, 60, 90, 40, ST77XX_GREEN);
}

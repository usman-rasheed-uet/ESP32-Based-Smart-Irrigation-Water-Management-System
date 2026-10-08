#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <WebServer.h>

// =====================================================
// OLED
// =====================================================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// =====================================================
// SENSOR PINS
// =====================================================
#define SOIL_PIN 34
#define WATER_PIN 35

// =====================================================
// OUTPUT PINS
// =====================================================
#define RELAY_PIN 26
#define BUZZER_PIN 27

#define GREEN_LED 25
#define BLUE_LED 32
#define RED_LED 33

// =====================================================
// SENSOR THRESHOLDS
// =====================================================
// Your actual readings:
//
// Soil:
// Dry ≈ 4000
// Wet ≈ 1000
//
// Water:
// Empty ≈ 0
// Full ≈ 1870

#define SOIL_DRY_THRESHOLD 3000
#define SOIL_WET_THRESHOLD 2400

#define WATER_LOW_THRESHOLD 500

// =====================================================
// RELAY LOGIC
// =====================================================
// Most relay modules are ACTIVE LOW.
// If your relay works opposite, change these.

#define RELAY_ON LOW
#define RELAY_OFF HIGH

// =====================================================
// WIFI ACCESS POINT
// =====================================================
const char* apSSID = "My_Irrigation_System";
const char* apPassword = "12345678";

WebServer server(80);

// =====================================================
// VARIABLES
// =====================================================
bool pumpRunning = false;

unsigned long previousBuzzerTime = 0;
bool buzzerState = false;

const unsigned long buzzerInterval = 500;

// =====================================================
// WEB PAGE
// =====================================================
void handleRoot() {

  int soilValue = analogRead(SOIL_PIN);
  int waterValue = analogRead(WATER_PIN);

  bool soilDry = soilValue > SOIL_DRY_THRESHOLD;
  bool soilWet = soilValue < SOIL_WET_THRESHOLD;
  bool waterLow = waterValue < WATER_LOW_THRESHOLD;

  String pumpStatus = pumpRunning ? "ON" : "OFF";
  String soilStatus = soilDry ? "DRY" : "WET";
  String waterStatus = waterLow ? "LOW" : "OK";

  String html = "";

  html += "<!DOCTYPE html>";
  html += "<html>";
  html += "<head>";

  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";

  html += "<meta http-equiv='refresh' content='2'>";

  html += "<title>Smart Irrigation</title>";

  html += "<style>";

  html += "body{";
  html += "font-family:Arial;";
  html += "background:#f2f2f2;";
  html += "text-align:center;";
  html += "margin:0;";
  html += "padding:20px;";
  html += "}";

  html += ".container{";
  html += "max-width:500px;";
  html += "margin:auto;";
  html += "}";

  html += "h1{";
  html += "font-size:28px;";
  html += "}";

  html += ".card{";
  html += "background:white;";
  html += "padding:18px;";
  html += "margin:12px 0;";
  html += "border-radius:15px;";
  html += "box-shadow:0 3px 8px rgba(0,0,0,0.15);";
  html += "}";

  html += ".value{";
  html += "font-size:25px;";
  html += "font-weight:bold;";
  html += "}";

  html += ".on{color:green;}";
  html += ".off{color:red;}";
  html += ".warning{color:red;font-weight:bold;}";

  html += "</style>";

  html += "</head>";

  html += "<body>";

  html += "<div class='container'>";

  html += "<h1>SMART IRRIGATION</h1>";

  // Soil
  html += "<div class='card'>";
  html += "<h2>Soil Moisture</h2>";
  html += "<div class='value'>";
  html += String(soilValue);
  html += "</div>";
  html += "<p>Status: <b>";
  html += soilStatus;
  html += "</b></p>";
  html += "</div>";

  // Tank
  html += "<div class='card'>";
  html += "<h2>Water Tank</h2>";
  html += "<div class='value'>";
  html += String(waterValue);
  html += "</div>";
  html += "<p>Status: <b>";

  if (waterLow) {
    html += "<span class='warning'>LOW WATER</span>";
  } else {
    html += "OK";
  }

  html += "</b></p>";
  html += "</div>";

  // Pump
  html += "<div class='card'>";
  html += "<h2>Water Pump</h2>";

  if (pumpRunning) {
    html += "<div class='value on'>ON</div>";
    html += "<p>Watering plants</p>";
  } 
  else {
    html += "<div class='value off'>OFF</div>";
    html += "<p>Pump stopped</p>";
  }

  html += "</div>";

  // System
  html += "<div class='card'>";

  if (waterLow) {

    html += "<h2 class='warning'>⚠ LOW WATER</h2>";
    html += "<p>Pump automatically stopped.</p>";

  } 
  else if (pumpRunning) {

    html += "<h2>💧 WATERING</h2>";
    html += "<p>Soil is dry.</p>";

  } 
  else {

    html += "<h2>✓ SYSTEM NORMAL</h2>";
    html += "<p>No watering required.</p>";
  }

  html += "</div>";

  html += "<p>ESP32 Smart Irrigation System</p>";

  html += "</div>";

  html += "</body>";
  html += "</html>";

  server.send(200, "text/html", html);
}

// =====================================================
// SETUP
// =====================================================
void setup() {

  // Serial
  Serial.begin(115200);

  // ===================================================
  // OLED
  // ===================================================

  Wire.begin(21, 22);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {

    Serial.println("OLED NOT FOUND!");

    while (1);
  }

  // ===================================================
  // OUTPUTS
  // ===================================================

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  // Initial states

  digitalWrite(RELAY_PIN, RELAY_OFF);

  digitalWrite(BUZZER_PIN, LOW);

  digitalWrite(GREEN_LED, LOW);
  digitalWrite(BLUE_LED, LOW);
  digitalWrite(RED_LED, LOW);

  // ===================================================
  // ESP32 WIFI ACCESS POINT
  // ===================================================

  WiFi.mode(WIFI_AP);

  WiFi.softAP(apSSID, apPassword);

  Serial.println();
  Serial.println("==============================");
  Serial.println("SMART IRRIGATION WIFI");
  Serial.println("==============================");

  Serial.print("WiFi Name: ");
  Serial.println(apSSID);

  Serial.print("WiFi Password: ");
  Serial.println(apPassword);

  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  // ===================================================
  // WEB SERVER
  // ===================================================

  server.on("/", handleRoot);

  server.begin();

  Serial.println("Web server started!");

  // ===================================================
  // STARTUP OLED
  // ===================================================

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);

  display.setCursor(0, 0);
  display.println("SMART IRRIGATION");

  display.setCursor(0, 15);
  display.println("WIFI READY");

  display.setCursor(0, 30);
  display.println("ESP32_Irrigation");

  display.setCursor(0, 45);
  display.println("192.168.4.1");

  display.display();

  delay(3000);
}

// =====================================================
// MAIN LOOP
// =====================================================
void loop() {

  // Handle web requests
  server.handleClient();

  // ===================================================
  // READ SENSORS
  // ===================================================

  int soilValue = analogRead(SOIL_PIN);
  int waterValue = analogRead(WATER_PIN);

  // ===================================================
  // DETERMINE CONDITIONS
  // ===================================================

  bool soilDry = soilValue > SOIL_DRY_THRESHOLD;

  bool soilWet = soilValue < SOIL_WET_THRESHOLD;

  bool waterLow = waterValue < WATER_LOW_THRESHOLD;

  // ===================================================
  // LOW WATER CONDITION
  // ===================================================

  if (waterLow) {

    // Pump OFF
    pumpRunning = false;

    digitalWrite(RELAY_PIN, RELAY_OFF);

    // Red LED ON
    digitalWrite(RED_LED, HIGH);

    // Green OFF
    digitalWrite(GREEN_LED, LOW);

    // Blue OFF
    digitalWrite(BLUE_LED, LOW);

    // Buzzer
    if (millis() - previousBuzzerTime >= buzzerInterval) {

      previousBuzzerTime = millis();

      buzzerState = !buzzerState;

      digitalWrite(BUZZER_PIN, buzzerState);
    }
  }

  // ===================================================
  // WATER AVAILABLE
  // ===================================================

  else {

    // Red OFF
    digitalWrite(RED_LED, LOW);

    // Green ON
    digitalWrite(GREEN_LED, HIGH);

    // Buzzer OFF
    digitalWrite(BUZZER_PIN, LOW);

    buzzerState = false;

    // =================================================
    // START PUMP
    // =================================================

    if (soilDry) {

      pumpRunning = true;

      digitalWrite(RELAY_PIN, RELAY_ON);

      digitalWrite(BLUE_LED, HIGH);
    }

    // =================================================
    // STOP PUMP
    // =================================================

    if (soilWet) {

      pumpRunning = false;

      digitalWrite(RELAY_PIN, RELAY_OFF);

      digitalWrite(BLUE_LED, LOW);
    }
  }

  // ===================================================
  // OLED
  // ===================================================

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);

  // Title
  display.setCursor(0, 0);
  display.println("SMART IRRIGATION");

  // Soil
  display.setCursor(0, 13);

  display.print("Soil: ");
  display.print(soilValue);

  if (soilDry) {
    display.println(" DRY");
  }
  else {
    display.println(" WET");
  }

  // Water
  display.setCursor(0, 25);

  display.print("Tank: ");
  display.print(waterValue);

  if (waterLow) {
    display.println(" LOW");
  }
  else {
    display.println(" OK");
  }

  // Pump
  display.setCursor(0, 37);

  display.print("Pump: ");

  if (pumpRunning) {
    display.println("ON");
  }
  else {
    display.println("OFF");
  }

  // Status
  display.setCursor(0, 49);

  if (waterLow) {

    display.println("WARNING: LOW WATER");

  }
  else if (pumpRunning) {

    display.println("WATERING PLANTS");

  }
  else {

    display.println("SYSTEM NORMAL");
  }

  display.display();

  // ===================================================
  // SERIAL MONITOR
  // ===================================================

  Serial.print("Soil = ");
  Serial.print(soilValue);

  Serial.print(" | Water = ");
  Serial.print(waterValue);

  Serial.print(" | Pump = ");

  if (pumpRunning)
    Serial.print("ON");
  else
    Serial.print("OFF");

  Serial.print(" | Water Low = ");

  if (waterLow)
    Serial.println("YES");
  else
    Serial.println("NO");

  delay(500);
}
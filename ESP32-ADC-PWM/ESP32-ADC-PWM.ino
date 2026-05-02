// GROUP LEADER: Ashley Ambas
// * MEMBERS:
//    Exequiel John Gicana
//    Jovan Laguitao
//    Aliyah Macy Lara
//    Mark Daryl Sablon
// ===================== LIBRARIES =====================
// Library for DHT temperature & humidity sensor
#include <DHT.h>

// Library for I2C communication (used by OLED)
#include <Wire.h>

// Graphics libraries for OLED display
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ===================== PIN DEFINITIONS =====================
// LED output (controlled using PWM)
#define LED_PIN 2

// Potentiometer (analog input for brightness control)
#define POT_PIN 34

// LDR (light sensor analog input)
#define LDR_PIN 35

// DHT sensor data pin
#define DHT_PIN 32

// Sensor type
// NOTE:
// - Use DHT11 for real hardware
// - Use DHT22 for Wokwi simulation
#define DHT_TYPE DHT11

// ===================== OLED CONFIGURATION =====================
#define SCREEN_WIDTH 128     // OLED width in pixels
#define SCREEN_HEIGHT 64     // OLED height in pixels
#define OLED_RESET -1        // No reset pin used

// ===================== PWM CONFIGURATION =====================
// PWM frequency for LED
#define PWM_FREQ 5000

// PWM resolution (8-bit = values from 0 to 255)
#define PWM_RESOLUTION 8

// ===================== LDR THRESHOLD =====================
// Used to determine if environment is DARK or BRIGHT
// Adjust this based on your testing environment
#define LDR_THRESHOLD 1500

// ===================== OBJECT CREATION =====================
// Create DHT sensor object
DHT dht(DHT_PIN, DHT_TYPE);

// Create OLED display object (I2C)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


// ===================== SETUP FUNCTION =====================
void setup() {
  // Start serial communication (for debugging and output)
  Serial.begin(115200);

  // Initialize DHT sensor
  dht.begin();

  // Initialize PWM for LED (ESP32 Arduino Core 3.x style)
  // This attaches PWM control directly to LED pin
  ledcAttach(LED_PIN, PWM_FREQ, PWM_RESOLUTION);

  // Initialize I2C communication for OLED
  // SDA = GPIO 21, SCL = GPIO 22
  Wire.begin(21, 22);

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    // If OLED is not detected, print error
    Serial.println("OLED not found. Check wiring.");
  }

  // Confirmation message
  Serial.println("ESP32 ADC + PWM + DHT + OLED Started");
}


// ===================== LOOP FUNCTION =====================
void loop() {

  // ===================== READ SENSOR VALUES =====================
  // Read potentiometer value (range: 0 - 4095)
  int potValue = analogRead(POT_PIN);

  // Read LDR value (range: 0 - 4095)
  int ldrValue = analogRead(LDR_PIN);

  // Read temperature in Celsius
  float temperature = dht.readTemperature();

  // Read humidity percentage
  float humidity = dht.readHumidity();

  // ===================== PROCESS DATA =====================
  // Convert potentiometer value (0-4095) into PWM range (0-255)
  int brightness = map(potValue, 0, 4095, 0, 255);

  // Determine if environment is DARK or BRIGHT
  // Most LDR modules:
  // HIGH value = DARK
  // LOW value = BRIGHT
  bool isDark = ldrValue > LDR_THRESHOLD;

  // ===================== LED CONTROL =====================
  // If DARK → LED turns ON with adjustable brightness
  // If BRIGHT → LED turns OFF
  if (isDark) {
    ledcWrite(LED_PIN, brightness);   // Apply PWM brightness
  } else {
    ledcWrite(LED_PIN, 0);            // Turn LED OFF
  }

  // ===================== SERIAL MONITOR OUTPUT =====================
  // Print all values for debugging and demonstration
  Serial.print("Potentiometer: ");
  Serial.print(potValue);

  Serial.print(" | LDR: ");
  Serial.print(ldrValue);

  Serial.print(" | Status: ");
  Serial.print(isDark ? "DARK" : "BRIGHT");

  Serial.print(" | Temperature: ");
  if (isnan(temperature)) {
    Serial.print("Error");  // Handle sensor error
  } else {
    Serial.print(temperature);
    Serial.print(" C");
  }

  Serial.print(" | Humidity: ");
  if (isnan(humidity)) {
    Serial.print("Error");  // Handle sensor error
  } else {
    Serial.print(humidity);
    Serial.print(" %");
  }

  Serial.print(" | LED Brightness: ");
  Serial.println(isDark ? brightness : 0);

  // ===================== OLED DISPLAY OUTPUT =====================
  // Clear previous screen content
  display.clearDisplay();

  // Set text size and color
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Set starting position (top-left corner)
  display.setCursor(0, 0);

  // Display system title
  display.println("ESP32 ADC + PWM");

  // Display potentiometer value
  display.print("POT: ");
  display.println(potValue);

  // Display LDR value
  display.print("LDR: ");
  display.println(ldrValue);

  // Display environment status
  display.print("Status: ");
  display.println(isDark ? "DARK" : "BRIGHT");

  // Display LED brightness
  display.print("LED PWM: ");
  display.println(isDark ? brightness : 0);

  // Display temperature
  display.print("Temp: ");
  if (isnan(temperature)) {
    display.println("Error");
  } else {
    display.print(temperature);
    display.println(" C");
  }

  // Display humidity
  display.print("Hum: ");
  if (isnan(humidity)) {
    display.println("Error");
  } else {
    display.print(humidity);
    display.println(" %");
  }

  // Send all data to OLED screen
  display.display();

  // Delay to avoid flickering and allow readable output
  delay(1000);
}
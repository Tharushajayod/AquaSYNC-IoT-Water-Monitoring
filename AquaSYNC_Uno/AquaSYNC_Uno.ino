/**
 * Project: AquaSYNC - Smart Water Tank Monitoring System
 * Component: Arduino Uno (Display & Sensor Master)
 * Description:
 * Reads ultrasonic levels and analog pH values. Communicates with the ESP32 via UART 
 * to exchange sensor data and receive pump statuses. Implements an EMI noise 
 * suppression mechanism (value freezing) to prevent corrupted pH readings 
 * during high-voltage pump operation.
 */

#include <LiquidCrystal.h>

// Initialize LCD display (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

// Hardware Pin Definitions
const int PIN_TRIG_MAIN = 9;
const int PIN_ECHO_MAIN = 10;
const int PIN_TRIG_SOURCE = 11;
const int PIN_ECHO_SOURCE = 12;
const int PH_PIN = A0; 

// System Variables
float mainDist = 0;
float sourceDist = 0;
float phValue = 7.0;
float phVoltage = 0.0;

// Variables for EMI Noise Suppression (Value Freezing)
float lastGoodPH = 7.0;
float lastGoodPHVoltage = 0.0;

// Data received from ESP32
int pump1Status = 0;
int pump2Status = 0;
float tempValue = 0.0;

// Tank Dimensions (in cm)
const float TANK_HEIGHT_CM = 16.3;
const float SOURCE_TANK_HEIGHT_CM = 16.0; 
const float SENSOR_DEADZONE_CM = 4.50;

void setup() {
  // Initialize Serial Communication (UART to ESP32)
  Serial.begin(9600); 
  
  // Initialize 20x4 LCD
  lcd.begin(20, 4);
  
  // Configure Sensor Pins
  pinMode(PIN_TRIG_MAIN, OUTPUT); pinMode(PIN_ECHO_MAIN, INPUT);
  pinMode(PIN_TRIG_SOURCE, OUTPUT); pinMode(PIN_ECHO_SOURCE, INPUT);
  
  // --- Boot Sequence UI ---
  lcd.setCursor(5, 0); lcd.print("AQUA SYNC");
  lcd.setCursor(5, 1); lcd.print("Water Tank");
  lcd.setCursor(1, 2); lcd.print("Monitoring System");
  delay(3000); 
  lcd.clear();

  // --- Network Connection UI ---
  lcd.setCursor(5, 1); lcd.print("Loading...");
  lcd.setCursor(2, 2); lcd.print("connecting wifi");
  delay(4000); // Allow time for ESP32 to establish Wi-Fi connection
  lcd.clear();
}

/**
 * Calculates distance using HC-SR04 Ultrasonic Sensor.
 * Returns distance in centimeters.
 */
float readDistance(int trig, int echo) {
  digitalWrite(trig, LOW); delayMicroseconds(2);
  digitalWrite(trig, HIGH); delayMicroseconds(10);
  digitalWrite(trig, LOW);
  
  long duration = pulseIn(echo, HIGH, 30000UL); // 30ms timeout
  if(duration == 0) return 0; // Return 0 if no echo received
  
  return (duration * 0.0343) / 2.0;
}

/**
 * Reads analog pH sensor values over 10 iterations to average out 
 * minor fluctuations and converts it to a standard pH value (0-14).
 */
void readPH() {
  int avgval = 0;
  for(int i = 0; i < 10; i++) { 
    avgval += analogRead(PH_PIN); 
    delay(10); 
  }
  phVoltage = (avgval / 10.0) * (5.0 / 1023.0); 
  phValue = -5.70 * phVoltage + 21.34; // Calibration formula
}

void loop() {
  // 1. Read distance sensors
  mainDist = readDistance(PIN_TRIG_MAIN, PIN_ECHO_MAIN); delay(50);
  sourceDist = readDistance(PIN_TRIG_SOURCE, PIN_ECHO_SOURCE); delay(50);
  
  // 2. Read pH Sensor with EMI Noise Suppression (Freezing Logic)
  // Only read new pH values if the main high-voltage pump is OFF
  if (pump1Status == 0) {
    readPH();
    lastGoodPH = phValue;
    lastGoodPHVoltage = phVoltage;
  } else {
    // Hold the last known good value to prevent display fluctuations
    phValue = lastGoodPH;
    phVoltage = lastGoodPHVoltage;
  }

  // 3. Transmit Data to ESP32 (Format: MainDist,SourceDist,pHValue,pHVoltage\n)
  Serial.print(mainDist); Serial.print(",");
  Serial.print(sourceDist); Serial.print(",");
  Serial.print(phValue); Serial.print(",");
  Serial.println(phVoltage); 

  // 4. Receive and Parse Data from ESP32
  if (Serial.available()) {
    String incoming = Serial.readStringUntil('\n');
    int comma1 = incoming.indexOf(',');
    int comma2 = incoming.indexOf(',', comma1 + 1);

    if (comma1 > 0 && comma2 > 0) {
      pump1Status = incoming.substring(0, comma1).toInt();
      pump2Status = incoming.substring(comma1 + 1, comma2).toInt();
      tempValue = incoming.substring(comma2 + 1).toFloat();
    }
  }

  // 5. Calculate Fill Percentages (Constrained to 0-100%)
  float mainPct = 0;
  if (mainDist > 0) {
    mainPct = constrain(((TANK_HEIGHT_CM - max(0.0f, mainDist - SENSOR_DEADZONE_CM)) / TANK_HEIGHT_CM) * 100.0f, 0, 100);
  }
  
  float sourcePct = 0;
  if (sourceDist > 0) {
    sourcePct = constrain(((SOURCE_TANK_HEIGHT_CM - max(0.0f, sourceDist - SENSOR_DEADZONE_CM)) / SOURCE_TANK_HEIGHT_CM) * 100.0f, 0, 100);
  }

  // Constrain pH and Temp specifically for LCD display boundaries
  float displayPH = constrain(phValue, 0.0, 14.0);
  float displayTemp = constrain(tempValue, 0.0, 80.0);

  // 6. Update LCD Display UI
  
  // Row 1: Tank Levels
  lcd.setCursor(0, 0); 
  lcd.print("Main:"); lcd.print((int)mainPct); lcd.print("%  ");
  lcd.print("Src:"); lcd.print((int)sourcePct); lcd.print("%   ");
  
  // Row 2: Water Quality (pH & Temp)
  lcd.setCursor(0, 1); 
  lcd.print("pH: "); lcd.print(displayPH, 1); lcd.print("  ");
  lcd.print("Temp: "); lcd.print(displayTemp, 1); lcd.print("C  ");
  
  // Row 3: Pump Statuses
  lcd.setCursor(0, 2);
  lcd.print("P1:"); lcd.print(pump1Status == 1 ? "ON " : "OFF");
  lcd.print("    P2:"); lcd.print(pump2Status == 1 ? "ON " : "OFF");

  // Row 4: System Status Footer
  lcd.setCursor(0, 3);
  lcd.print("System Active       ");

  delay(1000); // 1-second refresh rate
}
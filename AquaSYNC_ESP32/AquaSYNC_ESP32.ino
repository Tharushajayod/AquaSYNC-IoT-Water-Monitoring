/**
 * Project: AquaSYNC - Smart Water Tank Monitoring System
 * Component: ESP32 Main Controller
 * Description: 
 * Handles Wi-Fi connectivity, Blynk IoT dashboard synchronization, 
 * logic processing for dual pumps (Water & pH), and alarms.
 */

#define BLYNK_TEMPLATE_ID "TMPL6q0KLYKJ-"
#define BLYNK_TEMPLATE_NAME "AquaSYNC"
#define BLYNK_AUTH_TOKEN "d_pc3sjj0FWw_DeXmtwfgfVvxo7658mU"

// Wi-Fi Credentials
const char* WIFI_SSID = "Galaxy_S25_FE";
const char* WIFI_PASS = "Tharu2004";

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Serial2 Pins for Uno Communication
#define RX_FROM_UNO 35
#define TX_TO_UNO 17

// Tank Dimensions (in cm)
const float TANK_HEIGHT_CM = 16.0;
const float SOURCE_TANK_HEIGHT_CM = 50.0; 
const float SENSOR_DEADZONE_CM = 4.50;

// Hardware Pin Definitions
const int TEMP_PIN = 13;
const int PUMP1_PIN = 27;   // Main Water Pump
const int PUMP2_PIN = 14;   // pH Dosing Pump
const int SWITCH1_PIN = 4;  // Physical Override Switch for Pump 1
const int SWITCH2_PIN = 5;  // Physical Override Switch for Pump 2

// LED and Buzzer Pin Definitions
const int LED_GREEN=22, LED_RED=23, LED_BLUE=19, LED_YELLOW=25, LED_WHITE=32, LED_ORANGE=33; 
const int BUZZER_WATER=21, BUZZER_TEMP=26;

// Object Initializations
OneWire oneWire(TEMP_PIN);
DallasTemperature sensors(&oneWire);
BlynkTimer timer;

// System Variables
float mainDist = 0, sourceDist = 0, phValue = 7.0, phVoltage = 0.0, tempC = 0.0;

// Manual Control Flags (Blynk Integration)
bool pump1ManualMode = false; 
bool pump1ManualState = false; 
bool pump2ManualMode = false; 
bool pump2ManualState = false; 
bool sourceLowAlertSent = false; 

/**
 * Reads and parses comma-separated sensor data coming from the Arduino Uno via UART.
 * Format: MainDist,SourceDist,pHValue,pHVoltage\n
 */
void readDataFromUno() {
  if (Serial2.available()) {
    String incomingData = Serial2.readStringUntil('\n');
    int c1 = incomingData.indexOf(',');
    int c2 = incomingData.indexOf(',', c1 + 1);
    int c3 = incomingData.indexOf(',', c2 + 1);
    
    if (c1 > 0 && c2 > 0 && c3 > 0) {
      mainDist = incomingData.substring(0, c1).toFloat();
      sourceDist = incomingData.substring(c1 + 1, c2).toFloat();
      phValue = incomingData.substring(c2 + 1, c3).toFloat();
      phVoltage = incomingData.substring(c3 + 1).toFloat(); 
    }
  }
}

// Relay Control Helpers (Active LOW Logic)
void setPump1(bool on) { digitalWrite(PUMP1_PIN, on ? LOW : HIGH); }
void setPump2(bool on) { digitalWrite(PUMP2_PIN, on ? LOW : HIGH); } 

/**
 * Core system logic executed periodically by the Blynk timer.
 * Handles level calculations, pump automation, safety interlocks, and alarms.
 */
void processSystemLogic() {
  readDataFromUno(); 

  // Calculate Tank Percentages (0-100% constrained)
  float mainLevelPct = (mainDist > 0) ? constrain(((TANK_HEIGHT_CM - max(0.0f, mainDist - SENSOR_DEADZONE_CM)) / TANK_HEIGHT_CM) * 100.0f, 0, 100) : 0;
  float sourceLevelPct = (sourceDist > 0) ? constrain(((SOURCE_TANK_HEIGHT_CM - max(0.0f, sourceDist - SENSOR_DEADZONE_CM)) / SOURCE_TANK_HEIGHT_CM) * 100.0f, 0, 100) : 0;

  // Check current status of Pump 1
  bool isPump1On = (digitalRead(PUMP1_PIN) == LOW);

  // Read current Temperature continuously
  sensors.requestTemperatures();
  tempC = sensors.getTempCByIndex(0);

  // --- 1. Main Water Pump (Pump 1) Logic ---
  bool physicalSw1 = (digitalRead(SWITCH1_PIN) == LOW);
  if (physicalSw1) { 
    setPump1(true); // Physical hardware override
  } else if (pump1ManualMode) { 
    setPump1(pump1ManualState); // Blynk manual control
  } else { 
    // Automated filling logic based on tank levels
    if (mainLevelPct < 20 && sourceLevelPct >= 10) {
      setPump1(true); 
    } else if (mainLevelPct >= 80 || sourceLevelPct < 10) {
      setPump1(false);
    }
  }

  // --- 2. pH Dosing Pump (Pump 2) Logic ---
  // Safety Interlock: Force Pump 2 OFF if Pump 1 is running to prevent dosing errors
  if (isPump1On) {
    setPump2(false); 
  } else {
    bool physicalSw2 = (digitalRead(SWITCH2_PIN) == LOW);
    if (physicalSw2) { 
      setPump2(true); // Physical hardware override
    } else if (pump2ManualMode) { 
      setPump2(pump2ManualState); // Blynk manual control
    } else { 
      // Automated pH correction logic
      setPump2(phValue < 6 || phValue > 8); 
    }
  }
  
  // --- 3. Push Updates to Blynk Dashboard ---
  Blynk.virtualWrite(V0, round(mainLevelPct)); 
  Blynk.virtualWrite(V1, round(sourceLevelPct)); 
  Blynk.virtualWrite(V2, tempC); 
  Blynk.virtualWrite(V3, phValue); 
  Blynk.virtualWrite(V4, isPump1On ? 255 : 0); 
  Blynk.virtualWrite(V5, digitalRead(PUMP2_PIN) == LOW ? 255 : 0); 

  // --- 4. Send Sync Data back to Arduino Uno ---
  int p1_val = isPump1On ? 1 : 0;
  int p2_val = (digitalRead(PUMP2_PIN) == LOW) ? 1 : 0;
  Serial2.print(p1_val); Serial2.print(","); 
  Serial2.print(p2_val); Serial2.print(",");
  Serial2.println(tempC);

  // --- 5. System Alarms & LED Indicators ---
  if(sourceLevelPct < 10) {
    digitalWrite(LED_RED, HIGH); digitalWrite(LED_GREEN, LOW); 
    digitalWrite(BUZZER_WATER, (millis()/300) % 2); // Fast beep
    
    // Send push notification once per event
    if (!sourceLowAlertSent) {
      Blynk.logEvent("source_low", "Warning: Source Tank water is below 10%! Please refill.");
      sourceLowAlertSent = true; 
    }
  } 
  else if(mainLevelPct < 5) {
    digitalWrite(LED_RED, HIGH); digitalWrite(LED_GREEN, LOW); 
    digitalWrite(BUZZER_WATER, (millis()/700) % 2); // Slow beep
    sourceLowAlertSent = false; 
  } 
  else {
    digitalWrite(BUZZER_WATER, LOW); 
    sourceLowAlertSent = false; 
    if (mainLevelPct >= 80) {
      digitalWrite(LED_GREEN, HIGH); digitalWrite(LED_RED, LOW);
    } else {
      digitalWrite(LED_GREEN, LOW); digitalWrite(LED_RED, LOW);
    }
  }

  // Temperature & pH Status Indication
  digitalWrite(LED_YELLOW, tempC > 30); digitalWrite(LED_BLUE, tempC <= 30);
  digitalWrite(BUZZER_TEMP, tempC > 30 ? (millis()/700 % 2) : LOW);
  digitalWrite(LED_WHITE, (phValue >= 6 && phValue <= 8));
  digitalWrite(LED_ORANGE, (phValue < 6 || phValue > 8));
}

// --- Blynk App Handlers ---
BLYNK_WRITE(V6) { pump1ManualMode = param.asInt(); } 
BLYNK_WRITE(V7) { pump1ManualState = param.asInt(); } 
BLYNK_WRITE(V8) { pump2ManualMode = param.asInt(); } 
BLYNK_WRITE(V9) { pump2ManualState = param.asInt(); } 

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RX_FROM_UNO, TX_TO_UNO); 

  // Initialize Output Pins
  pinMode(LED_GREEN, OUTPUT); pinMode(LED_RED, OUTPUT); pinMode(LED_BLUE, OUTPUT); 
  pinMode(LED_YELLOW, OUTPUT); pinMode(LED_WHITE, OUTPUT); pinMode(LED_ORANGE, OUTPUT);
  pinMode(BUZZER_WATER, OUTPUT); pinMode(BUZZER_TEMP, OUTPUT);
  
  pinMode(PUMP1_PIN, OUTPUT_OPEN_DRAIN); pinMode(PUMP2_PIN, OUTPUT_OPEN_DRAIN);
  
  // Initialize Input Pins
  pinMode(SWITCH1_PIN, INPUT_PULLUP); pinMode(SWITCH2_PIN, INPUT_PULLUP);

  // Set default states
  setPump1(false); setPump2(false);
  sensors.begin();
  
  // Initialize Wi-Fi Connection
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  int attempts = 0;
  while(WiFi.status() != WL_CONNECTED && attempts < 10) { delay(500); attempts++; }

  // Initialize Blynk Connection
  Blynk.begin(BLYNK_AUTH_TOKEN, WIFI_SSID, WIFI_PASS);
  
  // Setup logic processing loop to run every 1 second
  timer.setInterval(1000L, processSystemLogic);
}

void loop() {
  Blynk.run();
  timer.run();
}

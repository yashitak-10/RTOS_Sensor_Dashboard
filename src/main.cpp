#include <Arduino.h> //core functions
#include <DHT.h> // DHT sensor library
#include <Wire.h> // I2C library
#include <Adafruit_GFX.h> // Graphics library
#include <Adafruit_SSD1306.h> // OLED display library

// Pin definitions
#define DHTPIN 15
#define DHTTYPE DHT22
#define POT_PIN 34
#define BUZZER_PIN 26

// OLED setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

DHT dht(DHTPIN, DHTTYPE);

// Shared variables
float batteryTemp = 0; //temp in Celsius 
float batteryHumidity = 0; // humidity in percentage
int motorSpeed = 0; //Potentiometer value (0-4095)
int thermalRisk = 0; // 0=SAFE 1=WARNING 2=CRITICAL

// TinyML model — learned thresholds from RandomForest training
// Temperature drives 98.23% of prediction
int predictThermalRisk(float temp, float humidity, int motorSpd) {

  float motorHeat = (motorSpd / 4095.0) * 15.0; // max +15°C contribution
  float effectiveTemp = temp + motorHeat; // temp + heat means harderpush the motor, more the heat builds up in battery

  if (effectiveTemp < 45.0) {
    return 0; // SAFE
  } else if (effectiveTemp < 60.0) {
    if (humidity > 75.0) return 2;
    return 1; // WARNING
  } else {
    return 2; // CRITICAL
  }
}
// Task 1: Read DHT22 battery sensor
void TaskReadBatterySensor(void *pvParameters) {
  while(1) {
    #ifdef WOKWI_SIM
      // Simulate increasing temperature to test AI
      batteryTemp += 1.0;
      if (batteryTemp > 85.0) batteryTemp = 20.0; // reset
      batteryHumidity = 60.0;
    #else
      float t = dht.readTemperature();
      float h = dht.readHumidity();
      if (!isnan(t)) batteryTemp = t;
      if (!isnan(h)) batteryHumidity = h;
    #endif
    vTaskDelay(2000 / portTICK_PERIOD_MS); // convert milliseconds to ticks for FreeRTOS delay function
  }
}

// Task 2: Read motor speed from potentiometer
void TaskReadMotorSpeed(void *pvParameters) {
  while(1) {
    motorSpeed = analogRead(POT_PIN); // read potentiometer value (0-4095)
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

// Task 3: Run TinyML thermal prediction
void TaskRunAI(void *pvParameters) {
  while(1) {
    thermalRisk = predictThermalRisk(batteryTemp, batteryHumidity, motorSpeed);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

// Task 4: Update OLED display
void TaskUpdateDisplay(void *pvParameters) {
  while(1) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);

    display.setCursor(0, 0);
    display.print("Temp:  ");
    display.print(batteryTemp);
    display.println(" C");

    display.print("Humid: ");
    display.print(batteryHumidity);
    display.println(" %");

    display.print("Motor: ");
    display.println(motorSpeed);

    display.println("----------------");
    display.print("Risk: ");

    if(thermalRisk == 0) {
      display.println("SAFE");
      digitalWrite(BUZZER_PIN, LOW);
    } else if(thermalRisk == 1) {
      display.println("WARNING");
      digitalWrite(BUZZER_PIN, LOW);
    } else {
      display.println("CRITICAL!");
      digitalWrite(BUZZER_PIN, HIGH);
    }

    display.display();// send buffer to display
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200); //open serial monitor for debugging
  pinMode(BUZZER_PIN, OUTPUT); // buzzer for critical alert
  dht.begin();//start communication with DHT22 sensor

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { //connect OLED to I2C address 0x3C
    Serial.println("OLED not found"); // check wiring
    while(1);
  }

  // Create FreeRTOS tasks with priorities
  xTaskCreate(TaskReadBatterySensor, "Battery Sensor", 2048, NULL, 1, NULL);
  xTaskCreate(TaskReadMotorSpeed,    "Motor Speed",    1024, NULL, 1, NULL);
  xTaskCreate(TaskRunAI,             "TinyML AI",      2048, NULL, 2, NULL);
  xTaskCreate(TaskUpdateDisplay,     "Display",        2048, NULL, 3, NULL);
}

void loop() {
  // FreeRTOS handles everything
}
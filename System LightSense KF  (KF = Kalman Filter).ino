#include <Wire.h>
#include <BH1750.h>
#include <LiquidCrystal_I2C.h>


BH1750 lightMeter;
LiquidCrystal_I2C lcd(0x27, 16, 2);

float processNoise = 1.0;
float measurementNoise = 1.0;
float estimatedError = 1.0;
float kalmanGain = 0.0;
float currentEstimate = 0.0;

unsigned long previousMillis = 0;
const unsigned long interval = 120000;
int dataCount = 0;
const int maxData = 30;  

void setup() {
  Wire.begin();
  Serial.begin(9600);
  
  Serial.println("No,Waktu(menit),Cahaya Mentah(lx),Cahaya Terfilter(lx)");
  
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Alat Ukur Cahaya");
  delay(2000);
  
  if (lightMeter.begin()) {
    Serial.println("Sensor BH1750 siap");
    lcd.clear();
    lcd.print("Sensor Siap!");
    delay(1000);
  } else {
    Serial.println("Error inisialisasi BH1750!");
    lcd.clear();
    lcd.print("Error Sensor!");
    while (1);
  }
}

float applyKalmanFilter(float measurement) {
  kalmanGain = estimatedError / (estimatedError + measurementNoise);
  currentEstimate = currentEstimate + kalmanGain * (measurement - currentEstimate);
  estimatedError = (1.0 - kalmanGain) * estimatedError + abs(currentEstimate - measurement) * processNoise;
  return currentEstimate;
}

void loop() {
  unsigned long currentMillis = millis();
  
  float lux = lightMeter.readLightLevel();
  float filteredLux = applyKalmanFilter(lux);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Mentah: ");
  lcd.print(lux, 1);
  lcd.print(" lx");
  
  lcd.setCursor(0, 1);
  lcd.print("Filter: ");
  lcd.print(filteredLux, 1);
  lcd.print(" lx");
  
  if (currentMillis - previousMillis >= interval && dataCount < maxData) {
    previousMillis = currentMillis;
    dataCount++;
    
    Serial.print(dataCount);
    Serial.print(",");
    Serial.print((currentMillis / 60000)); 
    Serial.print(",");
    Serial.print(lux);
    Serial.print(",");
    Serial.println(filteredLux);
  }
  
  delay(1000); 
}

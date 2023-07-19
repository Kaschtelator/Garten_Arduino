/*
Project:  Garten Arduino Uno
Author:   Basiert auf Code von Thomas Edlinger for www.edistechlab.com, inspiriert und weiter entwickelt für Arduino Uno von Kaschtelator
Date:     Created 19.07.2023
Version:  V5.0
IDE:      Arduino IDE 1.8.19
 
Required Board (Tools -> Board -> Boards Manager...)
 - Arduino Uno 
*/


const int sensorTaktSek = 300;         // alle x Sekunden wird der Sensor ausgelesen
const int soilMoistLevelLow = 516;    // Wert ohne Kontakt zum Boden (trocken)
const int soilMoistLevelHigh = 200;   // Wert im Wasser
const int startWatering = 80;         // Schwellwert Bewässerung ein in %
const int wateringDuration = 2;       // Bewässerungszeit in Sekunden
const int soilMoistPin = A0;          // Analoger Pin für Bodenfeuchtigkeitssensor
const int pumpPin = 9;                // Digitaler Pin für Wasserpumpe

int lastSoilMoist = 0;
int waterPerDayCount = 0;
int sensorTakt = sensorTaktSek * 1000;   // Umrechnung von ms auf Sekunden
long lastMsg = 0;
bool sensorConnected = true; // Variable zur Erkennung, ob der Sensor angeschlossen ist

void setup() {
  Serial.begin(115200);
  pinMode(soilMoistPin, INPUT);
  pinMode(pumpPin, OUTPUT);
  digitalWrite(pumpPin, HIGH); // Stellt sicher, dass die Wasserpumpe aus ist
}

void loop() {
  long now = millis();

  if (now - lastMsg > sensorTakt) {
    lastMsg = now;
    getSoilMoistValues();
  }
}

void getSoilMoistValues() {
  // Verzögerung vor der Auslesung des Sensors
  delay(1000); // 1 Sekunde Verzögerung
  int newSoilMoist = analogRead(soilMoistPin);

  if (newSoilMoist != lastSoilMoist) {
    // Überprüfen, ob der Sensorwert im erwarteten Bereich liegt
    if (newSoilMoist > 1000 || newSoilMoist < 0) {
      Serial.println("Sensor nicht angeschlossen oder defekt!");
      sensorConnected = false;
    } else {
      sensorConnected = true;
      Serial.print("Analogwert: ");
      Serial.println(newSoilMoist);

      // Auswertung der Bodenfeuchtigkeit in Prozent
      int percentSoilMoist = map(newSoilMoist, soilMoistLevelHigh, soilMoistLevelLow, 100, 0);
      Serial.print("Prozent Bodenfeuchtigkeit: ");
      Serial.print(percentSoilMoist);
      Serial.println(" %");

      if (percentSoilMoist <= startWatering) {
        Serial.println("Boden zu trocken, Bewässerung aktivieren");
        plantWatering();
      }

      lastSoilMoist = newSoilMoist;
    }
  }
}

void plantWatering() {
  // Nur wenn der Sensor angeschlossen ist, Bewässerung durchführen
  if (sensorConnected) {
    digitalWrite(pumpPin, LOW); // Schaltet die Wasserpumpe ein
    Serial.println("Bewässerung START");
    delay(wateringDuration * 1000);

    digitalWrite(pumpPin, HIGH); // Schaltet die Wasserpumpe aus
    Serial.println("Bewässerung STOPP");
    ++waterPerDayCount;
    Serial.print("Bewässerung heute: ");
    Serial.println(waterPerDayCount);
    delay(1000);
  }
}

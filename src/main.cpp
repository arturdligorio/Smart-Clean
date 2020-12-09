#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#define LED_SENSOR D0
#define ANALO A0
#define BLYNK_PRINT Serial
#define PIN_RELE D1

int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;

int pinBlynkValue = 0;

float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;

int VALUE_ON_OFF_RELE = 80;

char auth[] = "kxzy0rpp7_lBDcbjpZZTyXcf6nivZhxd";

char ssid[] = "OI";
char pass[] = "10203040";

void setup()
{
  Serial.begin(9600);
  pinMode(LED_SENSOR, OUTPUT);
  pinMode(PIN_RELE, OUTPUT);

  digitalWrite(PIN_RELE, LOW);

  Blynk.begin(auth, ssid, pass);
}

void loop()
{

  Blynk.run();
  if (pinBlynkValue == 1)
  {
    digitalWrite(LED_SENSOR, LOW);
    delayMicroseconds(samplingTime);

    voMeasured = analogRead(ANALO);

    delayMicroseconds(deltaTime);
    digitalWrite(LED_SENSOR, HIGH);
    delayMicroseconds(sleepTime);

    // 0 - 5V mapped to 0 - 1023 integer values
    // recover voltage
    calcVoltage = voMeasured * (3.0 / 1024.0);

    // linear eqaution taken from http://www.howmuchsnow.com/arduino/airquality/
    // Chris Nafis (c) 2012
    dustDensity = 170 * calcVoltage - 0.1;
    String r = "Result: " + String(dustDensity) + "\t Value:" + String(voMeasured);
    //Serial.println(r); // unit: ug/m3

    if (dustDensity > VALUE_ON_OFF_RELE)
    {
      digitalWrite(PIN_RELE,HIGH);
    }else
    {
      digitalWrite(PIN_RELE,LOW);
    }
    
    Blynk.virtualWrite(V1, dustDensity);
  }else{
    Blynk.virtualWrite(V1, 0);
  }
  delay(1000);
}

BLYNK_WRITE(V0)
{
  pinBlynkValue = param.asInt();
  if (pinBlynkValue == 1)
    Serial.println("ON");
  if (pinBlynkValue == 0)
    Serial.println("OFF");
}

BLYNK_WRITE(V2)
{
  VALUE_ON_OFF_RELE = param.asInt();
}
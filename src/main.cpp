// for I2C Communication
#include <WiFi.h>
#include <Wire.h>
#include <HX711.h>
#include <PN532.h>
#include <PN532_I2C.h>
#include <HTTPClient.h>
#include <NfcAdapter.h>

int SDA_PIN = 21, SCL_PIN = 22, DOUT = 32, CLK = 33;

PN532_I2C pn532_i2c(Wire);
NfcAdapter nfc = NfcAdapter(pn532_i2c);
String tagId = "None";

HX711 hx711;
HX711 scale(DOUT, CLK);
float weight;
float calibration_factor = 211000;

void setup(void)
{
  Serial.begin(115200);
  Serial.println("System initialized");

  WiFi.begin("LOUREIRO17_2G", "114326PR");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to Wifi...");
  }

  Serial.println("Connected to wifi");

  Wire.begin(SDA_PIN, SCL_PIN);
  nfc.begin();

  scale.set_scale();
  hx711.tare(2000);
  long zero_factor = scale.read_average();
  Serial.print("Zero factor: ");
  Serial.println(zero_factor);

  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, HIGH);
}

void loop()
{
  if (nfc.tagPresent())
  {
    NfcTag tag = nfc.read();
    tag.print();
    tagId = tag.getUidString();
    Serial.println("Tag id");
    Serial.println(tagId);

    digitalWrite(BUILTIN_LED, LOW);
    delay(500);
    digitalWrite(BUILTIN_LED, HIGH);
    delay(500);
    tagId = "";

    scale.set_scale(calibration_factor);
    weight = scale.get_units(5);

    if (weight < 0)
    {
      weight = 0.00;
    }
    Serial.print("Kilogram:");
    Serial.print(weight);
    Serial.print(" Kg");
    Serial.print(" calibration_factor: ");
    Serial.print(calibration_factor);
    Serial.println();

    if (WiFi.status() == WL_CONNECTED)
    {
      HTTPClient http;
      // sprintf(REQ_URL, "http://jsonplaceholder.typicode.com/todos/1", SERVER_BASE_URL);
      Serial.println("http://ip:3000/item");
      http.begin("http://ip:3000/item");
      int http_code = http.GET();
      if (http_code > 0)
      {
        String payload = http.getString();
        Serial.println(http_code);
        Serial.println(payload);
      }
      else
      {
        Serial.println("Error on HTTP request");
      }
      delay(3000);
    }
  }
}
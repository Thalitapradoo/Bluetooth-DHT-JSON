#include <Arduino.h>
#include <BluetoothSerial.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>

BluetoothSerial BT;
LiquidCrystal_I2C lcd(0x27, 16, 2);

unsigned long ultimoUpdate = 0;
const unsigned long intervalo = 300; // Atualiza LCD a cada 2 segundos

float umid = 0, tempC = 0, tempF = 0;

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  Serial.println("=== ESP32 SLAVE (LCD) ===");
  Serial.println("Inicializando Bluetooth...");

  if (!BT.begin("ESP32_Slave")) {
    Serial.println(" Erro ao iniciar Bluetooth Slave!");
    while (1);
  }

  Serial.println(" Bluetooth Slave iniciado!");
  lcd.setCursor(0, 0);
  lcd.print("Aguardando dados");
}

void loop() {
  // Recebe dados via Bluetooth
  if (BT.available()) {
    String jsonString = BT.readStringUntil('\n');
    jsonString.trim();

    if (jsonString.length() > 0) {
      StaticJsonDocument<128> doc;
      DeserializationError erro = deserializeJson(doc, jsonString);

      if (!erro) {
        umid = doc["umidade"];
        tempC = doc["tempC"];
        tempF = doc["tempF"];

        Serial.printf(" Recebido JSON | Umidade: %.1f%% TempC: %.1f°C TempF: %.1f°F\n",
                      umid, tempC, tempF);
      } else {
        Serial.print(" Erro ao interpretar JSON: ");
        Serial.println(erro.f_str());
      }
    }
  }

  // Atualiza LCD a cada 2 segundos, mesmo que o valor não tenha mudado
  if (millis() - ultimoUpdate >= intervalo) {
    ultimoUpdate = millis();

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.printf("Umi:%.1f%%", umid);
    lcd.setCursor(0, 1);
    lcd.printf("T:%.1fC %.1fF", tempC, tempF);
  }
}
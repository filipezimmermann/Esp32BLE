#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

const char* ssid = "Teste ESP32";
const char* password = "teste123";
const char* mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;
const char* mqtt_topic = "esp32/localizadorBLE_FILIPE";

WiFiClient espClient;
PubSubClient client(espClient);
BLEScan* pBLEScan;
int scanTime = 5;

// CONSUMO DE ENERGIA //
const float correnteBLE = 0.040;   // 40 mA durante BLE ativo
const float correnteIDLE = 0.010;  // 10 mA durante IDLE
const float duracaoScanner = 1.5;  // 30% do tempo
const float duracaoIDLE = 3.5;     // 70% do tempo
const float tensao = 3.3;

float calcularConsumo() {
  float consumoEnergia = tensao * ((correnteBLE * duracaoScanner) + (correnteIDLE * duracaoIDLE));
  Serial.print("Consumo de energia total (W): ");
  Serial.println(consumoEnergia, 4);
  return consumoEnergia;
}

float calcularDistancia(int rssi) {
  int txPower = -70;  // RSSI esperado a 1 metro
  float n = 4.0;      // fator de propagação

  if (rssi == 0) return -1.0;
  float exponent = (txPower - rssi) / (10.0 * n);
  return pow(10.0, exponent);
}

uint16_t getLocalMQTTPort() {
  return espClient.localPort();
}


class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    StaticJsonDocument<512> doc;

    float distancia = calcularDistancia(advertisedDevice.getRSSI());

    //if (distancia < 0.5) {
      float consumo = calcularConsumo();

      String nome = advertisedDevice.getName();
      if (nome.isEmpty()) nome = "sem_nome";

      // String ipv6_esp32 = WiFi.localIP6().toString();

      doc["nome"] = nome;
      doc["mac"] = advertisedDevice.getAddress().toString().c_str();
      doc["rssi"] = advertisedDevice.getRSSI();
      doc["distancia_m"] = distancia;
      doc["ipv4_esp32"] = WiFi.localIP().toString();
      doc["porta_esp32"] = getLocalMQTTPort();
      doc["consumo_W"] = consumo;

      char buffer[512];
      serializeJson(doc, buffer);
      client.publish(mqtt_topic, buffer);
      Serial.println(buffer);
    //}
  }
};

void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando ao MQTT...");
    if (client.connect("ESP32Localizador")) {
      Serial.println(" conectado!");
    } else {
      Serial.print(" falhou, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5s...");
      delay(5000);
    }
  }
}

void reconectarWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("🔄 Wi-Fi desconectado! Tentando reconectar...");
    WiFi.disconnect();
    WiFi.begin(ssid, password);

    unsigned long inicioTentativa = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - inicioTentativa < 5000) {
      delay(500);
      Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\n✅ Wi-Fi reconectado!");
      Serial.print("IP do ESP32: ");
      Serial.println(WiFi.localIP());
      piscarRapido();
    } else {
      Serial.println("\n❌ Não reconectou ao Wi-Fi...");
    }
  }
}


void piscarRapido() {
  for (int i = 0; i < 5; i++) {
    digitalWrite(2, 1);
    delay(100);
    digitalWrite(2, 0);
    delay(100);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(2, OUTPUT);
  Serial.println("\nIniciando...");
  piscarRapido();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.printf("Conectando ao Wi-Fi: %s\n", ssid);

  unsigned long startAttemptTime = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 15000) {
    digitalWrite(2, !digitalRead(2));
    delay(500);
  }
  digitalWrite(2, 0);

  if (WiFi.status() == WL_CONNECTED) {
    piscarRapido();
    Serial.println("\n✅ Wi-Fi conectado!");
    Serial.print("IP do ESP32: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n❌ Falha ao conectar no Wi-Fi!");
    Serial.println("Verifique SSID, senha ou rede 2.4GHz.");
  }

  client.setServer(mqtt_server, mqtt_port);

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
}

void loop() {
  // Verifica e tenta reconectar Wi-Fi
  reconectarWiFi();

  // Verifica e reconecta MQTT
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  Serial.println("🔍 Varredura BLE iniciada...");
  BLEScanResults* foundDevices = pBLEScan->start(scanTime, false);

  int k = foundDevices->getCount();
  Serial.printf("Dispositivos encontrados: %d\n", k);

  pBLEScan->clearResults();

  // Pisca LED para cada dispositivo encontrado
  for (int j = 1; j <= k; j++) {
    digitalWrite(2, 1);
    delay(100);
    digitalWrite(2, 0);
    delay(100);
  }

  delay(10000);  // Novo ciclo após 10 segundos
}


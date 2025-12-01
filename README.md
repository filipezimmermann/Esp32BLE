# 🛰️ Localizador BLE com ESP32 + MQTT

Este projeto implementa um **localizador de dispositivos Bluetooth Low Energy (BLE)** utilizando o **ESP32**, enviando as informações detectadas para um broker **MQTT**.  
O código também realiza **cálculo estimado de consumo de energia** e **estimativa de distância** com base no RSSI.

---

## 🚀 Funcionalidades

- 🔍 Varredura de dispositivos BLE próximos  
- 📏 Estimativa de distância pelo RSSI  
- 📡 Envio de dados em JSON via MQTT  
- 🔁 Reconexão automática ao Wi-Fi e MQTT  
- 💡 Indicação por LED  
- 🔋 Cálculo aproximado de consumo energético  

---

## 📡 Tecnologias Utilizadas

- ESP32  
- Bluetooth Low Energy (BLE)  
- MQTT (Mosquitto)  
- ArduinoJson  
- PubSubClient  
- WiFi  

---

## 💾 Como utilizar a aplicação

---

## 1. ⚙️ Pré-requisitos

### Hardware
* **ESP32** (placa com Wi-Fi e BLE).
* **Cabo USB**.
* **Dispositivo BLE** para ser rastreado (ex: fone de ouvido Bluetooth).

### Software
* **Arduino IDE** (ou PlatformIO).
* **Bibliotecas Essenciais:** `PubSubClient` e `ArduinoJson`.

---

## 2. 📝 Configuração Rápida

Antes de carregar, ajuste apenas as credenciais de rede no início do código (`.ino`):

| Variável | Descrição | Valor Padrão |
| :--- | :--- | :--- |
| `ssid` | Nome da sua rede Wi-Fi 2.4 GHz. | `"Teste ESP32"` |
| `password` | Senha da sua rede Wi-Fi. | `"teste123"` |
| `mqtt_server` | Endereço do broker. | `"test.mosquitto.org"` |
| `mqtt_topic` | Tópico de envio. | `"esp32/localizadorBLE_FILIPE"` |

---

## 3. 🚀 Execução

1.  **Conecte** o ESP32 ao computador.
2.  **Carregue** o código na placa (selecionando a porta COM e o modelo do ESP32 corretos).
3.  **Abra o Monitor Serial** (configurado para **115200 baud**) para confirmar que o Wi-Fi e o MQTT se conectaram com sucesso.
4.  O ESP32 começará a escanear e, ao encontrar um dispositivo BLE, enviará uma mensagem JSON para o tópico MQTT a cada ciclo. 

### 3.1. Visualização dos Dados (Dashboard)

Para ver os resultados em tempo real:

1.  Use um **Cliente MQTT** (como MQTT Explorer ou um *dashboard* web).
2.  Conecte ao broker: **`test.mosquitto.org`** (Porta 1883).
3.  **Assine (Subscribe)** o tópico: **`esp32/localizadorBLE_FILIPE`**.

Você receberá dados como MAC, nome, RSSI e a distância estimada (`distancia_m`).

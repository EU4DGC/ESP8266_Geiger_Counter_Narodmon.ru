#include <ESP8266WiFi.h>
#include <OneWire.h>

const char* ssid     = "******"; // название и пароль точки доступа
const char* password = "******";

const char* host = "narodmon.ru";
const int httpPort = 8283;


void setup() {
  Serial.begin(115200);
  delay(10);

}

void loop() {
  // Подключаемся к wifi
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");  
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC address: ");
  Serial.println(WiFi.macAddress());
  Serial.println();
  
  // подключаемся к серверу 
  Serial.print("connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  // отправляем данные  
  Serial.println("Sending..."); 
      // заголовок
  client.print("#");
  client.print(WiFi.macAddress()); // отправляем МАС нашей ESP8266
  client.print("#");
  client.print("ESP8266"); // название устройства
  client.println();
  
  
    client.print("#"); 
    for(int i = 0; i < 8; i++) client.print(addr[i], HEX); // номер 18b20 
    client.print("#");
    client.print(temperature);
    client.print("#temp");  // название датчика
 
   client.println("##");
    
  delay(10);

  // читаем ответ с и отправляем его в сериал
  Serial.print("Requesting: ");  
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line); // хотя это можно убрать
  }
  
  client.stop();
  Serial.println();
  Serial.println();
  Serial.println("Closing connection");

  WiFi.disconnect(); // отключаемся от сети
  Serial.println("Disconnect WiFi.");
  
  delay(1000*60*10); // перекур 10 минут
}

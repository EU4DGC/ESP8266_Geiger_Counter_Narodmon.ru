#include <ESP8266WiFi.h>
#include <SPI.h>

#define LOG_PERIOD 60000  //Период регистрации в миллисекундах, рекомендуемое значение 15000-60000.
#define MAX_PERIOD 60000  //Максимальный период регистрации.
#define postingInterval  300000 // Интервал между отправками данных на Narodmonitor.ru

unsigned long counts;     //
unsigned long cpm;        //
unsigned int multiplier;  //
unsigned long previousMillis;  //
unsigned long previousMillis1;  //
float uSv;            // Переменная для перевода в микроЗиверты
float ratio = 151.0; // Коофициент для перевода импульсов в микроЗиверты
float uP; // Переменная для перевода в микроРентгены
const byte interruptPin = D2; // Порт ESP к которому подключен счетчик

// Настройки WiFi
const char* ssid     = ""; // название и пароль точки доступа
const char* password = "";

const char* host = "narodmon.ru"; // адрес и порт
const int httpPort = 8283;

void tube_impulse(){       //Функция подсчета имульсов
  counts++;
}

void setup() {
  counts = 0;
  cpm = 0;
  multiplier = MAX_PERIOD / LOG_PERIOD;  
  Serial.begin(9600);
  interrupts();
  pinMode(interruptPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin), tube_impulse, FALLING); //Определяем количество импульсов через внешнее прерывание на порту  
  delay(10);

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
  Serial.println(WiFi.macAddress()); // MAC адресс нашей ESP8266, он нам понадобиться для добавления датчика на Narodmon.ru 
  Serial.println();
  

}

void loop() {
  
  unsigned long currentMillis = millis();
  unsigned long currentMillis1 = millis();
  if(currentMillis - previousMillis > LOG_PERIOD){
    previousMillis = currentMillis;
    cpm = counts * multiplier;
    //Serial.println(cpm);
    uSv = cpm / ratio ;
    //Serial.println(uSv);
    uP = uSv * 100 ;
    counts = 0; 
  }

  if((currentMillis1 - previousMillis1) + LOG_PERIOD > postingInterval){
    previousMillis1 = currentMillis1;
    Connect();
    Send(); 
    Serial.println("Каждые 5 минут");
    Serial.println(uP);
  }
      
}

void Send(){
  // WiFiClient создаем TCP соединение
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
  client.print("\n");
  
      // отправляем данные
    client.print("#"); 
    client.print("RAD"); // название датчика 
    client.print("#");
    client.print(uP);
    client.print("\n");
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
  //Serial.println("Closing connection");

}

void Connect(){
  
  // подключаемся к серверу 
  Serial.print("connecting to ");
  Serial.println(host);
}

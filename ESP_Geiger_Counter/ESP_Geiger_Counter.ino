
#include <SPI.h>
#define LOG_PERIOD 15000  //Период регистрации в миллисекундах, рекомендуемое значение 15000-60000.
#define MAX_PERIOD 60000  //Максимальный период регистрации.

unsigned long counts;     //
unsigned long cpm;        //
unsigned int multiplier;  //
unsigned long previousMillis;  //



void tube_impulse(){       //Функция подсчета имульсов
  counts++;
}

void setup(){             
  counts = 0;
  cpm = 0;
  multiplier = MAX_PERIOD / LOG_PERIOD;  
  Serial.begin(9600);
  interrupts();
  pinMode(interruptPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin), tube_impulse, FALLING); //Определяем количество импульсов через внешнее прерывание на порту  
  delay(10);
}

void loop(){                                 
  unsigned long currentMillis = millis();
  if(currentMillis - previousMillis > LOG_PERIOD){
    previousMillis = currentMillis;
    cpm = counts * multiplier;
    
    Serial.print(cpm);
    counts = 0;
  }
  
}






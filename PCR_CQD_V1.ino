#include "max6675.h"

#define  trans_pin  3

int soPin = 4;// SO = Serial Out
int csPin = 5;// CS = chip select CS pin
int sckPin = 6;// SCK = Serial Clock pin 
MAX6675 k_type_module(sckPin, csPin, soPin);// create instance object of MAX6675                             
double T = 0;
float t = 0;
float Z = 1.0206;
float  X = 8.4271;

unsigned long process_start_time;
unsigned long initial_denat_time;
unsigned long final_exte_time;
unsigned long denat_time;
unsigned long anne_time;
unsigned long exte_time;
int i = 1;
int j = 1;
int k = 1;
int a = 0;
int b = 0;
int c = 0;
int cycle = 0;

void setup() {
pinMode(trans_pin, OUTPUT);
Serial.begin(9600);
}


void loop(){
  while (T < 55){
    for (int i = 1; i<=6; i++){
     analogWrite(trans_pin, i);
     delay(60000);
     }
   update_temperature();
   display_results();
  }
  a = i*10; //55
  while(T > 26){
    analogWrite(trans_pin, 0);
    update_temperature();
    display_results();
    }
    //****
    //****
    while (T < 72){
    for (int j = 1; j<=6; j++){
     analogWrite(trans_pin, j*10);
     delay(60000);
     }
   update_temperature();
   display_results();
  }
  b = j*10; //72
  while(T > 26){
    analogWrite(trans_pin, 0);
    update_temperature();
    display_results();
    }
    //****
    //****
    while (T < 95){
    for (int k = 1; k<=6; k++){
     analogWrite(trans_pin, k*10);
     delay(60000);
     }
   update_temperature();
   display_results();
  }
  c = k*10;
  while(T > 26){
    analogWrite(trans_pin, 0);
    update_temperature();
    display_results();
    }
    //****Initial Denaturing
    while (T < 95){
     analogWrite(trans_pin, c);
     update_temperature();
     display_results();
     }
initial_denat_time = millis();
while (((millis()-initial_denat_time)/1000) < 90){
  if (T > 96){
    analogWrite(trans_pin, (c-10));
     update_temperature();
     display_results();
    }
    if (T < 94){
    analogWrite(trans_pin, (c));
     update_temperature();
     display_results();
    }
     update_temperature();
     display_results();
}
//**** Start the Cycle 
while (cycle <= 35){
denat_time = millis();
while(((millis()-denat_time)/1000) < 30){
    if (T > 96){
    analogWrite(trans_pin, (c-10));
     update_temperature();
     display_results();
    }
    if (T < 94){
    analogWrite(trans_pin, (c));
     update_temperature();
     display_results();
    }
     update_temperature();
     display_results();
   }
anne_time = millis();
while(((millis()-anne_time)/1000) < 30){
    if (T > 56){
    analogWrite(trans_pin, (a-10));
     update_temperature();
     display_results();
    }
    if (T < 54){
    analogWrite(trans_pin, (a));
     update_temperature();
     display_results();
    }
     update_temperature();
     display_results();
   }
exte_time = millis();
while(((millis()-exte_time)/1000) < 60){
    if (T > 73){
    analogWrite(trans_pin, (b-10));
     update_temperature();
     display_results();
    }
    if (T < 71){
    analogWrite(trans_pin, (b));
     update_temperature();
     display_results();
    }
     update_temperature();
     display_results();
   }
  cycle++;
}
 //**** Final Extenstion
while (T > 73 & T < 71){
     analogWrite(trans_pin, b);
     update_temperature();
     display_results();
     }
final_exte_time = millis();
while (((millis()-final_exte_time)/1000) < 600){
  if (T > 73){
    analogWrite(trans_pin, (b-10));
     update_temperature();
     display_results();
    }
    if (T < 71){
    analogWrite(trans_pin, (b));
     update_temperature();
     display_results();
    }
     update_temperature();
     display_results();
}
}


void update_temperature(){
  t = k_type_module.readCelsius();
  T = Z*t - X;  
}

void display_results(){
          Serial.print(millis()/1000); 
          Serial.print(" , ");
          Serial.println(T); 
          delay(1000);
}

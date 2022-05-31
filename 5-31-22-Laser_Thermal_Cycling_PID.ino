// This code is prepared to perfrom PCR using the photon-to-heat conversion property of Carbon Qunatum Dots //
// Designed by S.Balou 10/11/2021 //

#include "max6675.h" // Include max6675 library to read MX6675 k-type thermocouple module

// Relay PIN //
int trans_pin = 3;


// Laser Operating temperature:(55-95) for PCR and (65) for LAMP + Number of PCR cycles (35)
int pcrCycle = 0;
unsigned long current_millis;
unsigned long init_denat_time;
unsigned long denat_start_time;
unsigned long exte_start_time;

//PCR variables
int denat_time = 15;//Duration of Denaturing
int exte_time = 30;//Duration of Extention
int int_denat_duration = 60;//Duration of initial Denaturing
int maxcycle = 31; //Number of PCR Cycles
int denat_T = 95; // Min Denaturing temperature
int exte_T = 60;// Min Extention temperature

// K type Thermistor Variables and Pins //
int soPin = 4;// SO = Serial Out
int csPin = 5;// CS = chip select CS pin
int sckPin = 6;// SCK = Serial Clock pin 
MAX6675 k_type_module(sckPin, csPin, soPin);// create instance object of MAX6675 
double temperature_read = 0;      // Intantaneous temperature value
double final_temperature_read = 0;   // Variable used to calculate the final temperature
double temperature_num_samples = 5; // Number of temperatures values sampled for averaging  
double temperature_offset = 5; // temperature offset to calibrate the temperature sensor                              
double temperature = 0;
float t = 0;
float A = 1.0206;
float  B = 8.4271;


// PID Controller Parameters: 
float kp = 150;// Proportional Constant can be 150, 1500 according to the literature. 
float ki = 1; // Intehrated constant is choosen to be 1. PID gain values
float error_denat;
float error_exte; 
float I_value;

//Duty Cycle values obtained by doing steadtstate analysis
float a;
float b;

int a_max  = 250;
int a_min = a_max * 0;

int b_max = a_max/2;
int b_min = a_max/2 * 0;


void setup()
{
  Serial.begin(9600);
  pinMode(trans_pin, OUTPUT);
  delay(10);
  
  // We have to wait for the temperature to reach 95 first:
  Serial.println("Please wait for the temperature to reach 95");
  while(temperature < denat_T){
    error_denat = denat_T - temperature; // calculate the T error
    
    I_value += (ki * error_denat); // Integrated value
    if (I_value > a_max){
      I_value = a_max;
    }
    if (I_value < a_min){
      I_value = a_min;
    }
    a = kp * error_denat + I_value; // Set the PID Output accordingly
    
    if (a > a_max){
      a = a_max;
    }
    if (a < a_min){
      a = a_min;
    }
    
    analogWrite(trans_pin, a);
    update_temperature();
    display_results();
  }

  Serial.println("Temperature has reached 95, please wait 60s for the initial denaturing to complete");
  init_denat_time = millis()/1000;
  while((millis()/1000)-init_denat_time <= int_denat_duration){
  
    error_denat = denat_T - temperature; // calculate the T error
    
    I_value += (ki * error_denat);
    if (I_value > a_max){
      I_value = a_max;
    }
    if (I_value < a_min){
      I_value = a_min;
    }
    a = kp * error_denat + I_value;
    
    if (a > a_max){
      a = a_max;
    }
    if (a < a_min){
      a = a_min;
    }
    
    analogWrite(trans_pin, a);
    update_temperature();
    display_results();
  }
 
  Serial.println("Initial Denaturing has completed, Amplification Loop will start now");
  pcrCycle++;
}

void loop()
{
while (pcrCycle <= maxcycle){  // While we are within the 36 cycles (TRUE) arduino will do as follows:

Serial.println("Going to Denaturing Temperature");
    
    while(temperature < denat_T){ // Make sure temperature is 95 to begin the loop
    error_denat = denat_T - temperature; // calculate the T error
    
    I_value += (ki * error_denat);
    if (I_value > a_max){
      I_value = a_max;
    }
    if (I_value < a_min){
      I_value = a_min;
    }
    a = kp * error_denat + I_value;
    
    if (a > a_max){
      a = a_max;
    }
    if (a < a_min){
      a = a_min;
    }
    
    analogWrite(trans_pin, a);
    update_temperature();
    display_results();
    }

      
      
    Serial.println("Denaturing DNA Now");
    denat_start_time = millis()/1000;
    while((millis()/1000)- denat_start_time < denat_time){ // As long as we are within the 15s time frame
        
    error_denat = denat_T - temperature; // calculate the T error
    
    I_value += (ki * error_denat);
    if (I_value > a_max){
      I_value = a_max;
    }
    if (I_value < a_min){
      I_value = a_min;
    }
    a = kp * error_denat + I_value;
    
    if (a > a_max){
      a = a_max;
    }
    if (a < a_min){
      a = a_min;
    }
    
    analogWrite(trans_pin, a);
    update_temperature();
    display_results();
    }
      
Serial.println("Going to Extention Temperature");
      while(temperature > exte_T){ // Make sure the temperature has reached 60 to begin the second step (Extention)
        analogWrite(trans_pin, 0);//Laser Off
        update_temperature();
        display_results();
      }
      
Serial.println("Extending DNA Now");
    exte_start_time = millis()/1000;
    while ((millis()/1000)-exte_start_time < exte_time){ // As long as we are in 40s time frame
    
    error_exte = exte_T - temperature; // calculate the T error
    
    I_value += (ki * error_exte);
    if (I_value > b_max){
      I_value = b_max;
    }
    if (I_value < b_min){
      I_value = b_min;
    }
    b = kp * error_exte + I_value;
    
    if (b > b_max){
      b = b_max;
    }
    if (b < b_min){
      b = b_min;
    }
    
    analogWrite(trans_pin, a);
    update_temperature();
    display_results();
    
    }
        
     update_temperature();
     display_results();
     pcrCycle++;
    }
    
while (pcrCycle > maxcycle){
analogWrite(trans_pin, 0);
Serial.println("Process Ended, GoodBye");
}
}

  
void update_temperature(){
  t = k_type_module.readCelsius();
  temperature = A*t - B;  
}

void display_results(){
          Serial.print(millis()/1000); 
          Serial.print(" , ");
          Serial.print(temperature); 
          Serial.print(" , ");
          Serial.println(pcrCycle); 
          delay(1000);
}

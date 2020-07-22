#include <SoftwareSerial.h>
 #include <avr/wdt.h>
 #include <math.h>
SoftwareSerial mySerial(0, 1); // RX, TX

#define sensorPin0  A0    // select the input pin for the CHANNEL

#define sensorPin1  A1    // select the input pin for the CHANNEL

#define sensorPin2  A2    // select the input pin for the CHANNEL

#define sensorPin3  A3   // select the input pin for the CHANNEL

#define sensorPin4  A4    // select the input pin for the pCHANNEL

#define motor10  8    // //  digital output
#define motor11  9    // //   digital  output

#define motor20  10    // //  digital  output
#define motor21  11    // //  digital output

#define motor1PWM  7    // //  PWM output
#define motor2PWM  6    // //  PWM output


int sensorValue0 = 0;  // variable to store the value coming from the sensor
int sensorValue1 = 0;  // variable to store the value coming from the sensor
int sensorValue2 = 0;  // variable to store the value coming from the sensor
int sensorValue3 = 0;  // variable to store the value coming from the sensor
int sensorValue4 = 0;  // variable to store the value coming from the sensor
int Power = 0;
int dir = 10;
int esq = 10;
int tempo1 = 0;
int tempo2 = 0;

void setup() {

  TCCR2A = 0X00;
  TCCR2B = 0X07;
  TCNT2 = 100;
  TIMSK2 = 0X01;
  
  pinMode(A0, INPUT);
   pinMode(A1, INPUT);
    pinMode(A2, INPUT);
     pinMode(A3, INPUT);
      pinMode(A4, INPUT);
  pinMode(motor2PWM ,OUTPUT);
  pinMode(motor20,OUTPUT);
  pinMode(motor21,OUTPUT);
  pinMode(motor1PWM,OUTPUT);
  pinMode(motor10,OUTPUT);
  pinMode(motor11,OUTPUT);
   Serial.setTimeout(500);
  Serial.begin(9600);
  Serial.flush();
  Serial.println("<Arduino is ready>");
}


void establishContact() {
  if (Serial.available() >= 0) {
    Serial.print('Projeto Robo_Batalha');   // send a capital A
    delay(300);
  }
}


void loop() {
tempo1 = millis(); // RC read channels routine
if (tempo1 >= tempo2 ){
readknobs();
tempo2 = tempo1 + 500;}

//////////////////////////////////////////////////////////////////////////////////////////
  
  Power= sensorValue3/1500;
  dir = (sensorValue0-1400) * Power;
  esq = (sensorValue1-1400) * Power;
  
}//end loop


void readknobs(){
  sensorValue0 = pulseIn(sensorPin0,HIGH,100000);
   sensorValue1 = pulseIn(sensorPin1,HIGH,100000);
    sensorValue2 = pulseIn(sensorPin2,HIGH,100000);
     sensorValue3 = pulseIn(sensorPin3,HIGH,100000);
      sensorValue4 = pulseIn(sensorPin4,HIGH,100000);

 Serial.print(sensorValue0);    
  Serial.print(" ");    
    Serial.print(sensorValue1); 
    Serial.print(" ");   
      Serial.print(sensorValue2);
      Serial.print(" ");    
        Serial.print(sensorValue3);
        Serial.print(" ");
          Serial.print(sensorValue4);
          Serial.println(" "); 
        Serial.println(" "); 
          Serial.print(dir);
          Serial.print(" "); 
          Serial.print(esq);
          Serial.println(" ");

}


///////////////////////////////////////////////////////////////////////////////////


ISR(TIMER2_OVF_vect) /// motors control routine
{
  TCNT2=100;
  if ( sensorValue3 <=  1050 ){
  digitalWrite(motor10,LOW);
digitalWrite(motor11,LOW);
digitalWrite(motor20,LOW);
digitalWrite(motor21,LOW);}


else {
  
  if(dir>=0){
  digitalWrite(motor10, HIGH);
   digitalWrite(motor11, LOW);
   analogWrite(motor1PWM,dir);}
  if(dir<=0){
  digitalWrite(motor10, LOW);
  digitalWrite(motor11, HIGH);
  analogWrite(motor1PWM,dir);}
    if(esq>=0){
  digitalWrite(motor20,HIGH );
  digitalWrite(motor21,LOW );
   analogWrite(motor2PWM,esq);}
  if(esq<=0){
  digitalWrite(motor21,LOW);
  digitalWrite(motor20,HIGH);
   analogWrite(motor2PWM,esq);}
  
}
}


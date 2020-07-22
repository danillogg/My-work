#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include "HX711.h"
#include <EEPROM.h>
#include <ArduinoOTA.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"
#include "math.h"
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "index.h"  //Our HTML webpage contents with javascripts

///////////////////////////////////////////////////////////////////////////////////////////////////////////

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 14;
const int LOADCELL_SCK_PIN = 12;
HX711 scale;
/////////////////////////////////////////////////////////////////////////////////////////////////////////
File entry;
///////////////////////////////////////////////////////////////////////////////////////////////////////// 
LiquidCrystal_I2C lcd(0x3f,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
//LiquidCrystal lcd(36,39,34,35,32,33);
/////////////////////////////////////////////////////////////////////////////////////////////////////////

TaskHandle_t Task1;
TaskHandle_t Task2;



#define TWDT_TIMEOUT_S          3
#define TASK_RESET_PERIOD_S     2

#define CHECK_ERROR_CODE(returned, expected) ({                        \
            if(returned != expected)         {                         \
                printf("TWDT ERROR\n");                                \
                abort();                                               \
                                             }                         \
                                             })

const int L = 4;
const int ena = 25; //habilita o motor
const int dir = 27; //determina a direção
const int pul = 26; //executa um passo
long  contapasso = 0;

int numero = 0 ;
String tamanho;
String nome;

String SDselect = "UpLoad";
String Tela = "Tela1";
bool bot17 = false;
bool bot16 = false;
bool bot4  = false;
bool bot2  = false;
bool bot15  = false;
const int Pin17 = 17;
const int Pin16 = 16;
const int Pin4 = 4;
const int Pin2 = 2;
const int Pin15 = 15;



//byte Read;
//String part1;
//String part2;
//byte bytes = { 23 }; // A byte array contains non-ASCII (or non-readable) characters
//String command;

int tela6 = 0;

long media  ;
int voltas ;                                               
float tempowrite = 0;
float tempobot = 0;
float tempo = 0;
float tempoLeitura = 0;
float tempoTelaEnsaio = 0;
float TempoEnsaio = 0;
float TempoStart = 0;

long i;

//long total ;
long tara ; 
long taxa ;
long mediaf ;
long mediaAnterior;

//long reading ;

   

int  tempoPasso = 5208;//intervalo entre as  mudanças de estado do pulso//
int tempoMicros = 0;
int  tempoloop = 0;


bool Pulso = LOW;//estado do pulso
bool ajusteVelocidade = false;
bool ajustePosicao = false;
bool tarando = false;
bool avanco = false;
bool Ensaio = false;


//////////////////////////////////////////variaveis que indicam o núcleo///////////////////////////////////////////////////////////
static uint8_t taskCoreZero = 0;
static uint8_t taskCoreOne  = 1;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////.


//SSID and Password of your WiFi router
const char* ssid = "Vivo-Fita";
const char* password = "amarra3573";

WebServer server(80); //Server on port 80


////////////////////////////////////////////////////////////////////////////////////////////////////////////


void setup() 
                                                    {
                                      
enableCore0WDT();
enableCore1WDT();

lcd.begin();
lcd.backlight();
lcd.setCursor(0,0);
//lcd.begin(16, 2);// set up the LCD's number of columns and rows: 
lcd.print("Starting...");// Print a message to the LCD.
Serial.begin(230400);
Serial.println();
Serial.println("Starting...");
delay(10);

      Serial.print("Initializing SD card...");
     
      if (!SD.begin())                               {
        Serial.println("initialization failed!");
        Serial.println("No SD;");
        
                                                     }
      Serial.println("initialization done.");
     
      //entry = SD.open("/");
 
    


pinMode (Pin17, INPUT);
pinMode (Pin16, INPUT);
pinMode (Pin4, INPUT);
pinMode (Pin2, INPUT);
pinMode (Pin15, INPUT);
//pinMode(button, INPUT);
pinMode(ena, OUTPUT);
pinMode(dir, OUTPUT);
pinMode(pul, OUTPUT);
digitalWrite(ena, LOW); //habilita em low invertida 
digitalWrite(dir, HIGH); // low CW / high CCW 
digitalWrite(pul, LOW); //borda de descida
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
 
  server.on("/", handleRoot);      //Which routine to handle at root location. This is display page
  server.on("/readADC", handleADC); //This page is called by java Script AJAX
 
  server.begin();                  //Start server
  Serial.println("HTTP server started");
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

delay(1000); 
tarar();

//cria uma tarefa que será executada na função coreTaskZero, com prioridade 1 e execução no núcleo 0
  //coreTaskZero: piscar LED e contar quantas vezes
  xTaskCreatePinnedToCore(
                      coreTaskZero,   /* função que implementa a tarefa */
                      "coreTaskZero", /* nome da tarefa */
                      4000,      /* número de palavras a serem alocadas para uso com a pilha da tarefa */
                      NULL,       /* parâmetro de entrada para a tarefa (pode ser NULL) */
                      1,          /* prioridade da tarefa (0 a N) */
                      NULL,       /* referência para a tarefa (pode ser NULL) */
                      taskCoreOne);         /* Núcleo que executará a tarefa */                
                    delay(100); //tempo para a tarefa iniciar
    
  //cria uma tarefa que será executada na função coreTaskOne, com prioridade 2 e execução no núcleo 1
  //coreTaskOne: atualizar as informações do display
  xTaskCreatePinnedToCore(
                      coreTaskOne,   /* função que implementa a tarefa */
                      "coreTaskOne", /* nome da tarefa */
                      3000,      /* número de palavras a serem alocadas para uso com a pilha da tarefa */
                      NULL,       /* parâmetro de entrada para a tarefa (pode ser NULL) */
                      0,          /* prioridade da tarefa (0 a N) */
                      NULL,       /* referência para a tarefa (pode ser NULL) */
                      taskCoreZero);         /* Núcleo que executará a tarefa */
                    delay(100); //tempo para a tarefa iniciar
    
                                                 }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop() { }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void coreTaskZero( void * pvParameters )        
                                                 {
    while(true)                                  
                                                 {
      TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;


         
tempo = millis() ;

if( tempowrite < tempo)
                                                 {
      tempowrite = tempo + 333;
      Escrita();                                  
                                                 }


 if(tarando == true)                             {
  delay(10);  
        tarar();                                 }


if( tempobot < tempo && Tela != "Tela7")
                                               {  
      tempobot = tempo + 222;
if(digitalRead (Pin17) == LOW) 
                                                 {
     bot17 = false;
                                                 }
if(bot17 == false && digitalRead (Pin17) == HIGH )
                                                 {
     if(Tela == "Tela1")                         {
        if (avanco == true)
                                                 {
  Ensaio = false;
  lcd.clear();
  lcd.print("Pare a Maquina!!: ");
  avanco = false;
  delay(3000);
 
                                                 }
       else 
                                                 {                                          
         Ensaio = true;
         Tela = "Tela7";
         delay(500);
         TempoStart = millis(); 
                                                 }
                                                 }
     else if( Tela == "Tela3")  
                                                 {
                                                 }
     else if( Tela == "Tela4")
                                                 {
     if (avanco == true)                      
                                                 {
          avanco = false;                        
                                                 }
     else
                                                 {
            avanco = true;
                                                 } 
                                                 }
     else if( Tela == "Tela5")                   
                                                 {
        tarando = true; 
                                           
                                                 } 
                                                   
      else if( Tela == "Tela6")                   
                                                 {
        tarando = true; 
        Tela = "SDSelect";                                 
                                                 }
      else if( Tela == "SDselect")                   
                                                 {
     if ( SDselect == "UpLoad")             
                                                 {


                                      
                                                 }
     if ( SDselect == "Delete")             
                                                 {
       // deleteFile(SD, nome);                                  
                                                 }
                                                 
                                                 }
                                                 
        bot17 = true;                                          
                                                 }
 

  if( digitalRead (Pin16) == LOW)                {
     bot16 = false;
                                                 }
if(bot16 == false && digitalRead (Pin16) == HIGH )
                                                 {
        if(Tela == "Tela1")                      {
          Tela = "Tela2";
                                                 }
        else if( Tela == "Tela2")                {
          Tela = "Tela3";
                                                 }
        else if( Tela == "Tela3")                {
          Tela = "Tela4";
                                                 }
         else if( Tela == "Tela4")               {
          Tela = "Tela5";
                                                 }
else if( Tela == "Tela5")                        {
          Tela = "Tela6";
                                                 }         
        else
                                                 {
          Tela = "Tela1";
                                                 }
     bot16 = true;
                                                 }

  
    if( digitalRead (Pin4) == LOW)               {
     bot4 = false;
                                                 }
  if(bot4 == false && digitalRead (Pin4) == HIGH )
                                                 {
         if(Tela == "Tela6")                     {
          Tela ="Tela5";
                                                 }                                           
        else if(Tela == "Tela5")                 {
          Tela ="Tela4";
                                                 }
        else if( Tela == "Tela4")                {
          Tela = "Tela3";
                                                 }
        else if( Tela == "Tela3")                {
          Tela = "Tela2";
                                                 }
        else if( Tela == "Tela2")                {
          Tela = "Tela1";
                                                 }
        else
                                                 {
          Tela = "Tela6";
                                                 }
     bot4 = true;
                                                 }
     if( digitalRead (Pin2) == LOW)              {
     bot2 = false;
                                                 }
  if(bot2 == false && digitalRead (Pin2) == HIGH )
                                                 {
        if(Tela == "Tela3")                      {
         digitalWrite(dir, HIGH);
                                                 }
        else if( Tela == "Tela2")                
                                                 {
         bot2 == true;
         delay(100);
         tempoPasso = tempoPasso /1.1;
         delay(100);
                                                 }
    else if( Tela == "Tela6")                
                                                 {
         bot2 == true;
        
         entry = SD.open("/");                                                  
        bot2 == true;                                           
        numero = numero -1;
        listDir(SD, "/", numero); 
         
                                                 }  
                                                   /////////////////////////////////////////////////////////////
   else if(Tela == "SDselect")                   {

         if( SDselect == "UpLoad")           {
          SDselect = "Delete";
                                                 }
         if( SDselect == "Delete")           {
          SDselect = "UpLoad";
                                                 }
   }
                                                 }
                                                   ///////////////////////////////////////////////////////////// 
 if( digitalRead (Pin15) == LOW)                 {
     bot15 = false;
                                                 }
 if(bot15 == false && digitalRead (Pin15) == HIGH )
                                                 {
 if(Tela == "Tela3")                      
                                                 {
         digitalWrite(dir, LOW);
                                                 }
        else if( Tela == "Tela2")                
                                                 {
         bot15 == true;
         delay(100);
         tempoPasso = tempoPasso *1.1 ;
         delay(100);
                                                 }
 else if( Tela == "Tela6")                
                                                 {
         bot2 == true;
      
         entry = SD.open("/");                                                  
        bot2 == true;                                           
        numero = numero +1;
        listDir(SD, "/", numero); 
         
                                                 }
                                                   /////////////////////////////////////////////////////////////
   else if(Tela == "SDselect")                   {

         if( SDselect == "UpLoad")           {
          SDselect = "Delete";
                                                 }
         if( SDselect == "Delete")           {
          SDselect = "UpLoad";
                                                 }
                                                   /////////////////////////////////////////////////////////////  
                                                 }
                                                 }
                                               }


  TIMERG0.wdt_feed=1;
  TIMERG0.wdt_wprotect=0;                                              
                                                 
                                                 } 
                                                 }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                                                  
     


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//essa função será responsável por ler o estado do botão
//e atualizar a variavel de controle.
void coreTaskOne( void * pvParameters )
                                                 {
while(true)       
                                                 {     
TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;

if (avanco == true)            
                                                 { 
tempoMicros = micros() ; 
if (tempoloop <= tempoMicros)      
                                                 {
Step();
tempoloop = tempoMicros + tempoPasso;
                                                 }
                                                 }                                       

tempo = millis() ;
 if( tempoLeitura < tempo)                       {
       tempoLeitura = tempo + 3;                                      
                                                 
     if (scale.is_ready())                       {  
      if (voltas < 5)                            {                     
     media = media + scale.read(); 
     voltas++;                                   }
      else                                       {  
     media =0;                                         
                                                 }
                                                 }                                           
       //voltas =0;                                               
                                                 }
     if (voltas >= 5)                            {                                                                             
     media = media/5;
     mediaf = calculaMassa (media); 
     mediaf = mediaf - tara; 
     media = 0;
     voltas = 0; 
                                                 }

TIMERG0.wdt_feed=1;
TIMERG0.wdt_wprotect=0;
                                                 }
                                                 } 




void Escrita()                                   {
  
  if(Tela == "Tela1")                            { 
       lcd.clear();
      
        lcd.backlight();
        lcd.setCursor(0,0);
        lcd.print("Peso: ");
        lcd.print(mediaf);
        lcd.print("g");
        lcd.setCursor(0,1);
        lcd.print("Tpasso: ");
        lcd.print(((1000000/tempoPasso*60)/5760)*5);
        lcd.print("mm/min");
                                                 }
  if(Tela == "Tela2")                            {
      lcd.clear();
      
        lcd.backlight();
        lcd.setCursor(0,0);
        lcd.print("Velocidade =");
        lcd.setCursor(0,1);
        lcd.print(((1000000/tempoPasso*60)/5760)*5);
        lcd.print("mm/min");
                                                 }
  if(Tela == "Tela3")                            {
       lcd.clear();
     
       lcd.backlight();
       lcd.setCursor(0,0);
       lcd.print("Posicao");
                                                 }
  if(Tela == "Tela4")                            {
     lcd.clear();
    
       lcd.print("avanco");
                                                 }
  if(Tela == "Tela5")                            {
       lcd.clear();
       
       lcd.print("Tarar");
                                                 }
  if(Tela == "Tela6")                            {
          lcd.clear();
       lcd.setCursor(0,0);
       lcd.print("UPLOAD");
       lcd.setCursor(0,1);
      lcd.print(nome);
      lcd.print(" ");
      lcd.print(tamanho);
      lcd.print("kB");
                                                 }
                                                 
  if(Tela == "Tela7" ) 
                                                 {
 TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;                                                  
        lcd.clear();
    tempo = millis() ;   
  if( tempoTelaEnsaio < tempo)                      
        TempoEnsaio = millis() - TempoStart;     {
        tempoTelaEnsaio = tempo + 3.3333;                                                               
        lcd.clear();                                      
        lcd.print("Peso: ");
        lcd.print(mediaf);
        lcd.print("g");
        lcd.setCursor(0,1);
        lcd.print("Ensaiando!");
        lcd.print(" T=");
        lcd.print(TempoEnsaio/1000);
        lcd.print(" S ");
        tela6 ++;  

        TIMERG0.wdt_feed=1;
        TIMERG0.wdt_wprotect=0;
                                                 

       
                                                 }

  if(Tela == "SDselect")                         {
     lcd.clear();
      lcd.setCursor(0,0);
       lcd.print(nome);
        lcd.setCursor(0,1);
        lcd.print(SDselect);
                                                 }
                                                 

  
  if (mediaf >= mediaAnterior && Ensaio == true  )          
                                                 {
       avanco = true;
       mediaAnterior = mediaf - 1000; 
       Serial.println("Peso: ");
       Serial.print(mediaf);
       Serial.print("g");
       Serial.println("Tempo ");
       Serial.print(mediaf); 
       Serial.print("mS ");
       Serial.print("carga de ruptura: ");
       Serial.print(mediaAnterior);                                        
                                                 }
  else                                           {
       avanco = false;
       lcd.clear();
       tela6 = 0;
       lcd.print("Ruptura: ");
       lcd.print(mediaAnterior + 1000);
       lcd.print(" g");
       lcd.print(" T=");
       lcd.print(TempoEnsaio/1000);
       lcd.print(" S ");
       delay(15000);
       Tela = "Tela1";
       mediaAnterior = mediaf;                                           
                                                 }
                                           
                                                 
                                                 }
                                                          /////////////////////////////////////////////////////////////////////////////////////////
  
 //    Serial.println
    Serial.print("HX711 reading: ");
    Serial.println(mediaf);
    Serial.print("Tempo por passo:");
    Serial.println(tempoPasso);
    Serial.print("Tara: ");
    Serial.println(tara);
    Serial.print("passos dados");
    Serial.println(contapasso);
   delayMicroseconds(5);
                                                     /////////////////////////////////////////////////////////////////////////////////////////     
  
//lcd.clear();       
   // lcd.print("step");
  // lcd.print(contapasso);
server.handleClient(); 


  

         TIMERG0.wdt_feed=1;
       TIMERG0.wdt_wprotect=0;                           /////////////////////////////////////////////////////////////////////////////////////////
  
                                               }


void Step()    
                                               { 
     if(Pulso == HIGH)         
                                               { 
         Pulso = LOW;
          digitalWrite(pul, Pulso);
          contapasso ++;       
                                               }
    else                                       {
      Pulso = HIGH;
      digitalWrite(pul, Pulso);  
                                               }       
                                               }


void tarar()                                   {


 delayMicroseconds(1);
lcd.clear();
delayMicroseconds(1);
lcd.print("tarando... ");
delay(200);

  for (int l = 0; l<100; )                     {
    TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
  if (scale.is_ready()) 
                                               {
                       
taxa = scale.read(); 
tara = tara + taxa;
l++;
delay (0.2);
TIMERG0.wdt_feed=1;
TIMERG0.wdt_wprotect=0;
                                                }
else                                            {   
delay (0.2);
TIMERG0.wdt_feed=1;
TIMERG0.wdt_wprotect=0;
                                                }
                                                }
tara = tara/100;
tara   = calculaMassa (tara);
lcd.clear(); 
Tela = "Tela1";
tarando = false;


                                                }


long calculaMassa (long medida)
                                                {   
  //long reading ;

         medida    = (( 1028.55066061535  +  ( 1.003541768810e-002 * medida)) +
                      (( 1.0119051171717400e-009) * ( medida * medida )) +
                      ((-3.0635532812700900e-013) * ( medida * medida * medida ))+
                      ((-7.5059032353987200e-018) * ( medida * medida * medida * medida ))+
                      (( 3.3994448603850400e-022) * ( medida * medida * medida * medida * medida ))+
                      (( 1.0448734446665500e-026) * ( medida * medida * medida * medida * medida * medida ))+
                      ((-7.7929851225353400e-032) * ( medida * medida * medida * medida * medida * medida * medida ))+
                      ((-4.5675870926345575e-036) * ( medida * medida * medida * medida * medida * medida * medida * medida ))+
                      ((-2.2667311011741900e-041) * ( medida * medida * medida * medida * medida * medida * medida * medida * medida ))+
                      (( 5.2161744866433000e-046) * ( medida * medida * medida * medida * medida * medida * medida * medida * medida * medida ))+
                      (( 6.5780865863343273e-051) * ( medida * medida * medida * medida * medida * medida * medida * medida * medida * medida * medida ))+
                      (( 2.1658712867870274e-056) * ( medida * medida * medida * medida * medida * medida * medida * medida * medida * medida * medida * medida )));
 
      return medida;
                                                    
                                                }



void listDir(fs::FS &fs, const char * dirname, uint8_t levels)
                                                      {
        Serial.printf("Listing directory: %s\n", dirname);
        File file;
        File root = fs.open(dirname);

   if(!root)                                          {
        Serial.println("Failed to open directory");
        return;
                                                      }

    
     for (int ciclos =0 ; ciclos<= levels ; ciclos++)
                                                      {
        TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;                                                
        file = root.openNextFile();

                if(!file){
        Serial.println("Failed to open directory");
        listDir(SD, "/", 0);
        numero = 0;
        return;
                                                      }
    
     if(file.isDirectory())
                                                      {
         Serial.print("  DIR : ");
         Serial.println(file.name());
         nome = file.name();
    
                                                      }
        
      else                                            {
         tamanho =file.size();
         nome = file.name();
         Serial.print("  FILE: ");
         Serial.print(file.name());
         Serial.print("  SIZE: ");
         Serial.println(file.size());
                                                      }
 
        TIMERG0.wdt_feed=1;
        TIMERG0.wdt_wprotect=0; 
                                                      }
    
                                                      }


void deleteFile(fs::FS &fs, const char * path)        {
    Serial.printf("Deleting file: %s\n", path);
    if(fs.remove(path))                               {
        Serial.println("File deleted");
                                                      } 
    else                                              {
        Serial.println("Delete failed");
                                                      }
                                                      }


void handleADC() {
 long a = mediaf;

 String data = "{\"Celula de carga\":\""+String(a)+"\", \"Passos \":\""+ String(temperature) +"\", \"Tempo\":\""+ String(tempo) +"\"}";
 
 server.send(200, "text/plane", data); //Send ADC value, temperature and humidity JSON to client ajax request
 
 //Ref 1: https://circuits4you.com/2019/01/11/nodemcu-esp8266-arduino-json-parsing-example/
 //Ref 2: https://circuits4you.com/2019/01/25/arduino-how-to-put-quotation-marks-in-a-string/
 //Give enough time to handle client to avoid problems
  delay(2);

}

void handleRoot() {
 String s = MAIN_page; //Read HTML contents
 server.send(200, "text/html", s); //Send web page
}


                                                      

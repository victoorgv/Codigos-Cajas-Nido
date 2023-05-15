#include <DHT.h>
#include <DHT_U.h>
#include "LoRaWan_APP.h"
#include "Arduino.h"
#include "LoRa_APP.h"

#define timetillsleep  120000   //define el tiempo que duerme 
#define timetillwakeup 42000   //define el tiempo que esta despierto
static TimerEvent_t sleep;
static TimerEvent_t wakeUp;
uint8_t lowpower=1;
//-------------------------
int PIRvcc=          GPIO0;
int PIR =            GPIO1;
int Temphumidintvcc= GPIO2;
int SENSOR =         GPIO3;
int Temphumidextvcc= GPIO4;
int SENSOR2 =        GPIO5;
DHT dht1 (SENSOR, DHT11);
DHT dht2 (SENSOR2, DHT11);
//--------------------------
#define RF_FREQUENCY                                868000000 // Hz para módulo de frecuencia Europea
#define TX_OUTPUT_POWER                             14        // potencia en dBm de trabajo

#define LORA_BANDWIDTH                              0         // [0: 125 kHz, > ancho de banda
                                                              //  1: 250 kHz,
                                                              //  2: 500 kHz,
                                                              //  3: Reserved]
#define LORA_SPREADING_FACTOR                       7         // [SF7..SF12]
#define LORA_CODINGRATE                             1         // [1: 4/5,
                                                              //  2: 4/6,
                                                              //  3: 4/7,
                                                              //  4: 4/8]
#define LORA_PREAMBLE_LENGTH                        8         // Longitud para Tx y Rx
#define LORA_SYMBOL_TIMEOUT                         0         // Simbolos
#define LORA_FIX_LENGTH_PAYLOAD_ON                  false
#define LORA_IQ_INVERSION_ON                        false
#define RX_TIMEOUT_VALUE                            1000      // Tiempo espera en recepción
#define BUFFER_SIZE                                 60        // Define el tamaño del buffer
char txpacket[BUFFER_SIZE];
char rxpacket[BUFFER_SIZE];
static RadioEvents_t RadioEvents;
bool lora_idle=true;
 




void onSleep()
{
  Serial.printf("Entra en modo bajo consumo, despierta en %d ms  más tarde \r\n",timetillwakeup);
  lowpower=1;
  //timetillwakeup son los ms en que despierta más tarde
  TimerSetValue( &wakeUp, timetillwakeup );
  TimerStart( &wakeUp );
}

void onWakeUp()
{
  Serial.printf(" Ha despertado el Cube Cell, y entrará en modo bajo consumo %d ms más tarde \r\n",timetillsleep);
  lowpower=0;
  //timetillsleep ms más tarde que está en modo bajo consumo
  TimerSetValue( &sleep, timetillsleep );
  TimerStart( &sleep );
}

void setup(){
    Serial.begin(115200);
    pinMode(Temphumidintvcc,OUTPUT);
    pinMode(Temphumidextvcc,OUTPUT);
    pinMode(PIRvcc,OUTPUT);
    Radio.Sleep( );
    TimerInit( &sleep, onSleep );
    TimerInit( &wakeUp, onWakeUp );
    onSleep();
    //---------------------------------------------
    RadioEvents.TxDone = OnTxDone;
    RadioEvents.TxTimeout = OnTxTimeout;
    Radio.Init( &RadioEvents );
    Radio.SetChannel( RF_FREQUENCY );
    Radio.SetTxConfig( MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                                   LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                                   LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                                   true, 0, 0, LORA_IQ_INVERSION_ON, 3000 ); 
  dht1.begin();
  dht2.begin();
  pinMode(PIR ,INPUT);
  delay(20000);
  
}

void loop(){


  if(lowpower)
  {

    delay(2000);
    Serial.println("PROCEDEMOS A ENCENDER");
    digitalWrite(Temphumidintvcc,HIGH);
    digitalWrite(Temphumidextvcc,HIGH);
    digitalWrite(PIRvcc,HIGH);
    delay(20000);
    int h1,h2,t1,t2,P = 0;
    int Initpacket = 1;
    P = digitalRead(PIR);
    t1 = dht1.readTemperature();
    h1 = dht1.readHumidity();
    t2 = dht2.readTemperature();
    h2 = dht2.readHumidity();
    delay(10000);

    if(lora_idle == true)
    {
    delay(10000);
    
    //sprintf("Temperatura: %d",TEMPERATURA,"Humedad: %d",HUMEDAD,"Presencia: %d",ESTADO);
      
    sprintf(txpacket,"%d,%d,%d,%d,%d,%d",Initpacket,t1,h1,t2,h2,P);  //Crea el Paquete
    Serial.printf("\r\nEnviando: \"%s\" , Longitud:  %d\r\n",txpacket, strlen(txpacket));
    //turnOnRGB(COLOR_SEND,5);
    Radio.Send( (uint8_t *)txpacket, strlen(txpacket) ); //Manda el paquete  
    lora_idle = false;
    }
    

    
    
    //delay(5000);

    lowPowerHandler();  //Función para entrar en modo sleep

    Serial.println("PROCEDEMOS A APAGAR");
    digitalWrite(Temphumidintvcc,LOW);
    digitalWrite(Temphumidextvcc,LOW);
    digitalWrite(PIRvcc,LOW);

    Serial.println(" ");
    
  }

}

void OnTxDone( void )
{
  //turnOffRGB();
  Serial.println("TX done......");
  lora_idle = true;
}

void OnTxTimeout( void )
{
  //turnOffRGB();
  Radio.Sleep( );
  Serial.println("TX Timeout......");
  lora_idle = true;
}


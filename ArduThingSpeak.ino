#include <DHT.h>
#include <DHT_U.h>
#include <WiFiNINA.h>
#include "secrets.h"
#include "ThingSpeak.h"  // always include thingspeak header file after other header files and custom macros

char ssid[] = SECRET_SSID;  // your network SSID (name)
char pass[] = SECRET_PASS;  // your network password
int keyIndex = 0;           // your network key Index number (needed only for WEP)
WiFiClient client;

unsigned long myChannelNumber1 = SECRET_CH_ID1;
const char* myWriteAPIKey1 = SECRET_WRITE_APIKEY1;

unsigned long myChannelNumber2 = SECRET_CH_ID2;
const char* myWriteAPIKey2 = SECRET_WRITE_APIKEY2;

unsigned long myChannelNumber3 = SECRET_CH_ID3;
const char* myWriteAPIKey3 = SECRET_WRITE_APIKEY3;
String myStatus = "";
char DATOS[18];





void setup() {
  Serial.begin(115200);
  Serial1.begin(115200);

  while (!Serial) {
    ;
  }


  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true)
      ;
  }

  String fv = WiFi.firmwareVersion();
  if (fv != "1.0.0") {
    Serial.println("Porfavor Actualiza el Firmware");
  }

  ThingSpeak.begin(client);  //Initialize ThingSpeak
}

void loop() {

  // Connect or reconnect to WiFi
  if (WiFi.status() != WL_CONNECTED) {
    //Serial.print("Intentando conectar con la Red: ");
    //Serial.println(SECRET_SSID);
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass);
      //Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected.");
  }

  if (Serial1.available() > 0) {

    int Paquete = Serial1.readBytes(DATOS, 100);
    String myString = "";  // String vacío

    for (int i = 0; i < sizeof(DATOS); i++) {  // Bucle para leer cada byte del array
      myString += (char)DATOS[i];              // Convertir cada byte en un carácter y añadirlo a la cadena
    }
    //Serial.println("------------------------------");
    //Serial.println(myString);  // Imprimir la cadena en el monitor serie
    //----------------------------------------------------------------------------------------
    String InitPacket = "";
    String temp1 = "";
    String hum1 = "";
    String temp2 = "";
    String hum2 = "";
    String pres = "";
    String cadena = "";

    int j = 1;
    for (int i = 0; i < myString.length(); i++) {
      if (myString[i] == ',') {
        if (j == 1) {
          InitPacket = cadena;
        }
        if (j == 2) {
          temp1 = cadena;
        }
	      if (j == 3) {
          hum1 = cadena;
        }
	      if (j == 4) {
          temp2 = cadena;
        }
	      if (j == 5) {
          hum2 = cadena;
        }
        cadena = "";
        j++;
      } else {
        cadena = cadena + myString[i];
      }
    }
    pres = cadena;

    //Serial.println(temp1);
    //Serial.println(hum1);
    //Serial.println(temp2);
    //Serial.println(hum2);
    //Serial.println(pres);
    //Serial.println("------------------------------");

    int temperatura1 = atoi(temp1.c_str());
    int humedad1 = atoi(hum1.c_str());
    int temperatura2 = atoi(temp2.c_str());
    int humedad2 = atoi(hum2.c_str());
    int presencia = atoi(pres.c_str());

    if(InitPacket == 1){
      ThingSpeak.setField(1, temperatura1);
      ThingSpeak.setField(2, humedad1);
      ThingSpeak.setField(3, temperatura2);
      ThingSpeak.setField(4, humedad2);
      ThingSpeak.setField(5, presencia);

      ThingSpeak.setStatus(myStatus);

      int x = ThingSpeak.writeFields(myChannelNumber1, myWriteAPIKey1);
      if (x == 200) {
      Serial.println("Canal Caja 1 Actualizado.");
      }else {
      //Serial.pintln("Problema en la Actualizacion del Canal. HTTP error code " + String(x));SOLUCIONAR SI HAY TIEMPO (NO MUY IMPORTANTE)
      //resetSoftware();
      }
      delay(20000);  // Wait 20 seconds to update the channel again
    }

    if(InitPacket == 2){
      ThingSpeak.setField(1, temperatura1);
      ThingSpeak.setField(2, humedad1);
      ThingSpeak.setField(3, temperatura2);
      ThingSpeak.setField(4, humedad2);
      ThingSpeak.setField(5, presencia);

      ThingSpeak.setStatus(myStatus);

      int x = ThingSpeak.writeFields(myChannelNumber2, myWriteAPIKey2);
      if (x == 200) {
      Serial.println("Canal Caja 2 Actualizado.");
      }else {
      //Serial.pintln("Problema en la Actualizacion del Canal. HTTP error code " + String(x));SOLUCIONAR SI HAY TIEMPO (NO MUY IMPORTANTE)
      //resetSoftware();
      }
      delay(20000);  // Wait 20 seconds to update the channel again
    }

    if(InitPacket == 3){

      Serial.println(temperatura1);
      Serial.println(temperatura2);
      Serial.println(humedad1);
      Serial.println(humedad2);
      Serial.println(presencia);
      
      ThingSpeak.setField(1, temperatura1);
      ThingSpeak.setField(2, humedad1);
      ThingSpeak.setField(3, temperatura2);
      ThingSpeak.setField(4, humedad2);
      ThingSpeak.setField(5, presencia);

      ThingSpeak.setStatus(myStatus);

      int x = ThingSpeak.writeFields(myChannelNumber3, myWriteAPIKey3);
      if (x == 200) {
      Serial.println("Canal Caja 3 Actualizado.");
      }else {
      //Serial.pintln("Problema en la Actualizacion del Canal. HTTP error code " + String(x)); SOLUCIONAR SI HAY TIEMPO (NO MUY IMPORTANTE)
      //resetSoftware();
      }
      delay(20000);  // Wait 20 seconds to update the channel again
    }
    
  }
}

//-------------------------
// DHT11
//-------------------
#include <WiFi.h>
#include <HTTPClient.h>
#include "DHT.h"

//DHT
#define DHTPIN 4
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

//Conexão de Wifi
const char* ssid = "nome da rede";
const char* password = "senha";

//ip Server
const char* serverIP = "192.168.1.0"; /*Altera o IP conforme o 1 circuito| Substitui quando for testar e o novo ip aparecer*/

//controle de canal
unsigned long lastSend = 0;
const long interval = 5000;

bool wifiConnected = false;

//---------------------------------------------------
//WIFI
//---------------------------------------------------
bool connectWifi(){
  Serial.println("\n[WIFI] Resetando a interface....");
  WiFi.disconnected(true);
  delay(1000);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("\n[WIFI]conectando....");
  Serial.println("\n[WIFI]ssid:  ");
  Serial.println(ssid);

  int tentativas = 0;
  while(WiFi.status() != WL_CONNECTED && tentativas < 20){
    delay(500);
    Serial.println(".");
    tentativas++;
  }

  if(WiFi.status() == WL_CONNECTED){
      Serial.println("\n[WiFi] Conectado!!!");
      Serial.println("[WiFi] IP:  ");
      Serial.println(WiFi.localIP()); // retorna o ip, aqui acessaremos o servidor no navegador. 

      return true;
    } else {
      Serial.println("\n[WiFi] Falha na conexão");
      return false;
    }
}
//---------------------------------------------------
// Envio HTTP
//---------------------------------------------------
void sendData(float tempo, float hum){

  HTTPClient http;

  String url = "http://" + String(serverIP) + "/update?temp=" + String(temp,1) + "&hum" + String(hum,1);

  Serial.println("[HTTP] Enviando:  ");
  Serial.println(url);
  http.begin(url);
  http.setTimeout(3000);

  int httpCode = http.GET();
  if (httpCode > 0){
    Serial.println("[HTTP]Resposta: ");
    Serial.println(payload);
  } else {
    Serial.println("[HTTP]ERROR ");
    Serial.println(httpCode);;
  }
  http.end();




}

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}

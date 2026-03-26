//________________________________________

// ESP32 WEB SERVER - LED - RETURN DE DHT11

//________________________________________

//inclusao de libs
#include <WiFi.h> //Ativacao modulo WIFI
#include <WebServer.h> // Ativacao servvidor web

//variaveis
const char* ssid = "nome da rede";
const chat* password = "senha";

//login http
const char* http_user = "admin";
const char* http_password = "1234";

// Iniciacao servidor
Webserver server(80);

//Led
const int LED_PIN = 2; //conectando gpio2
bool ledState = false; // inicializa desligado

// Return DHT11
float temperatura = 0;
float umidade = 0;

//Conexao
bool wifiConnected = false;

//____________________________________________
// Autenticathor
//____________________________________________

bool isAuthenticated(){
  if (!server.authenticate(http_user, http_password)){
    server.requestAuthenticator();
    return false;
  }
  return false
}

//____________________________________________
// FRONTEND
//____________________________________________

String buildPage(){
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta charset='utf-8'>";
  html += "<meta name = 'viewport' content='width=device-width'>";
  html += "<title>ESP32</title>";
  html += "<style>";
  html += "body{font-family:sans-serif; text-align:center;padding:40px;background:#f4f4f4}";
  html += ".on{background:#009d00; color:#f4f4f4}";
  html += ".off{background:#F43208; color:#f4f4f4}";
  html += "</head> <body>";


  html += "<h1> Comunicação LAN</h1>";
  html += "<p> Status <Strong>";
  html += (ledState) ? "LIGADO" : "DESLIGADO";
  html += "</strong></p>";

  html += "<hr>";

  html += "<a href='/on'><button class ='on'>LIGAR</button></a>";
  html += "<a href='/off'><button class ='off'>DESLIGAR</button></a>";

  html += "<h2>SENSOR DHT11</h2>";
  html += "<p>Temperatura: ";
  html += String(temperatura);
  html += "%</p>";

  html += "</body></html>";

 return html;
}

  //-----------------------------------
  //Wifi
  //--------------------------------
  bool connectWifi(){
    Serial.println("\nWifi connect... Resetando a interface");

    Wifi.disconnect(true);
    delay(1000);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    Serial.println("[WiFi] conectando...");
    Serial.println("[WiFi] SSID:    ");
    Serial.println(ssid);

    int tentativas = 0;

    while (WiFi.status() != WL_CONNECT && tentativas < 20) {
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

  //----------------------------------------------------------------
  //Routes
  //----------------------------------------------------------------
void setupRoutes(){
  //se autenticado
  server.on("/", [](){
    if(!isAuthenticated()) return;
    server.send(200, "text/html", buildPage());
  });

  server.on("/on", [](){
    if(!isAuthenticated()) return;
    ledState = true;
    digitalWrite(LED_PIN, HIGH);
    server.sendHeader("Location", "/");
    server.send(303);
  });

  server.on("/off", [](){
    if(!isAuthenticated()) return;
    ledState = false;
    digitalWrite(LED_PIN, LOW);
    server.sendHeader("Location", "/");
    server.send(303);
  });

  //rebecer dados do dht11
  server.on("/update", [](){
    //SEM AUTH
    if(server.hasArg("temp")){
      temperatura = server.arg("temp").toFloat();
    }

    if(server.hasArg("hum")){
      umidade = server.arg("hum").toFloat();
    }

    Serial.println("[DHT] Dados recebidos");
    Serial.println("Temperatura: ");
    Serial.println(temperatura);
    Serial.println("Umidade: ");
    Serial.println(umidade);
    server.send(200, "text/plain", OK);
  })
}

 //----------------------------------------------------------------
  //SETUP
  //----------------------------------------------------------------
void setup(){
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n[BOOT]Inicializando o ESP32...")
  pinMode(LED_PIN,OUTPUT);
  digitalWrite(LED_PIN, LOW);

  wifiConnected = connectWifi();
  setupRoutes();

  if(wifiConnected){
    server.begin();
    Serial.println("[HTTP] Servidor Iniciado!");
  } else {
    Serial.println("[HTTP]Servidor NÃO iniciado (sem WIFI)");
  }
}

 //----------------------------------------------------------------
  //Loop
  //----------------------------------------------------------------
void loop(){
  if(WiFi.status()!= WL_CONNECTED){
    if(wifiConnected){
      Serial.println("[WIFI] Conexão Perdida");
      wifiConnected = false;
    }
    delay(2000);
    wifiConnected = connectWiFi();
    if (wifiConnected){
      server.begin();
      Serial.println("[HTTP] Servidor reiniciado!")
    }
    return;
  }
  server.handleClient();
}
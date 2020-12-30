#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>

const char *ssid = "Livebox-EF3C";
const char *password = "5FC2DC1221F679EC21C1592ECE";

const int led = 2;
const int capteurLuminosite = 34;

int valeurDelayLed = 1000;
bool etatLed = 0;
bool etatLedVoulu = 0;
int previousMillis = 0;

AsyncWebServer server(80);

void setup()
{
  //----------------------------------------------------Serial
  Serial.begin(9600);
  Serial.println("\n");

  //----------------------------------------------------GPIO
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  pinMode(capteurLuminosite, INPUT);

/*
  //----------------------------------------------------SPIFFS
  if (!SPIFFS.begin())
  {
    Serial.println("Erreur SPIFFS...");
    return;
  }

  File root = SPIFFS.open("/");
  File file = root.openNextFile();

  while (file)
  {
    Serial.print("File: ");
    Serial.println(file.name());
    file.close();
    file = root.openNextFile();
  }
//---------------------------------------------------SIZE SPIFF
      // Get all information of SPIFFS
 
    unsigned int totalBytes = SPIFFS.totalBytes();
    unsigned int usedBytes = SPIFFS.usedBytes();
 
    Serial.println("===== File system info =====");
 
    Serial.print("Total space:      ");
    Serial.print(totalBytes);
    Serial.println("byte");
 
    Serial.print("Total space used: ");
    Serial.print(usedBytes);
    Serial.println("byte");
*/
  //----------------------------------------------------WIFI
  WiFi.begin(ssid, password);
  Serial.print("Tentative de connexion...");

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(100);
  }

  Serial.println("\n");
  Serial.println("Connexion etablie!");
  Serial.print("Adresse IP: ");
  Serial.println(WiFi.localIP());

  //----------------------------------------------------SERVER
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    //request->send(SPIFFS, "/index.html", "text/html");
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "OK");
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
  });
  /*
  server.on("/w3.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/w3.css", "text/css");
  });

  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/script.js", "text/javascript");
  });

  server.on("/jquery-3.4.1.min.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/jquery-3.4.1.min.js", "text/javascript");
  });

  server.on("/lireLuminosite", HTTP_GET, [](AsyncWebServerRequest *request) {
    int val = analogRead(capteurLuminosite);
    String luminosite = String(val);
    request->send(200, "text/plain", luminosite);
  });*/

  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request) {
    etatLedVoulu = 1;
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "ON");
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
    //request->send(204);
  });

  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request) {
    etatLedVoulu = 0;
    digitalWrite(led, LOW);
    etatLed = 0;
    AsyncWebServerResponse *response = request->beginResponse(200, "text/plain", "OFF");
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
    //request->send(204);
  });
/*
  server.on("/delayLed", HTTP_POST, [](AsyncWebServerRequest *request) {
    if(request->hasParam("valeurDelayLed", true))
    {
      String message;
      message = request->getParam("valeurDelayLed", true)->value();
      valeurDelayLed = message.toInt();
    }
    request->send(204);
  });
*/
  server.begin();
  Serial.println("Serveur actif!");
}

void loop()
{
  if(etatLedVoulu)
  {
    unsigned long currentMillis = millis();
    if(currentMillis - previousMillis >= valeurDelayLed)
    {
      previousMillis = currentMillis;

      etatLed = !etatLed;
      digitalWrite(led, etatLed);
    }
  }
}
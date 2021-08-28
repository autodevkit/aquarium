/*
pio device monitor -b 115200
//mes mots de passe
BODzlxIsJfqrvhNrFwqsQqlVaEefe2o7EMVPxF8W
const char *ssid = "Livebox-EF3C";
const char *password = "5FC2DC1221F679EC21C1592ECE";
*/
//Required HTTPClientESP32Ex library to be installed  https://github.com/mobizt/HTTPClientESP32Ex

#include <WiFi.h>
#include <FirebaseESP32.h>
#include <NTPClient.h>


#define FIREBASE_HOST "https://aquadevkit-default-rtdb.europe-west1.firebasedatabase.app/" //Change to your Firebase RTDB project ID e.g. Your_Project_ID.firebaseio.com
#define FIREBASE_AUTH "NNwVE9jms6Q5OXGIBswAVX1tDNxlhokARofdFyA4" //Change to your Firebase RTDB secret password

#define WIFI_SSID "Bbox-C02AF1AE"
#define WIFI_PASSWORD "7D2AAFD9CD9A211735161A69C7E2D1"
//#define WIFI_SSID "Livebox-EF3C"
//#define WIFI_PASSWORD "Aze6655448822"

#include <RBDdimmer.h>

//Define Firebase Data objects
FirebaseData firebaseDataLoop;
// Allocate a 1024-byte buffer for the JSON document.

int millisTimeInit0 = 0;
int millisTimeInit1 = 0;


const int led = 2;

int temps;
int horloge;
String heure;
String minutes;
int h = 99999;
int m = 99999; 
int lumiereActiveHorlogetemps;
int minutePause;


String selectLum1;
String selectLum2;
String selectLum3;
String selectLum4;
String etatLum1;
String etatLum2;
String etatLum3;
String etatLum4;
String intensiteLum1;
String intensiteLum2;
String intensiteLum3;
String intensiteLum4;
String etatHoraire;
String horlogeLengt;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 7200, 60000);


const int zeroCrossPin = 34;
const int acdPin1 = 13;
const int acdPin2 = 12;
const int acdPin3 = 14;
const int acdPin4 = 27;

dimmerLamp acd1(acdPin1, zeroCrossPin);
dimmerLamp acd2(acdPin2, zeroCrossPin);
dimmerLamp acd3(acdPin3, zeroCrossPin);
dimmerLamp acd4(acdPin4, zeroCrossPin);

void setup()
{
  Serial.print("config Wi-Fi");
  Serial.begin(115200);

  pinMode(led, OUTPUT);

  Serial.println();
  Serial.println();

  digitalWrite(led, HIGH);

  delay(10);

    // We start by connecting to a WiFi network

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(WIFI_SSID);
    Serial.println(WIFI_PASSWORD);


  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecter to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connecter sur IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  timeClient.begin();

  acd1.begin(NORMAL_MODE, ON);
  acd2.begin(NORMAL_MODE, ON);
  acd3.begin(NORMAL_MODE, ON);
  acd4.begin(NORMAL_MODE, ON);

  acd1.setPower(0);
  acd2.setPower(0);
  acd3.setPower(0);
  acd4.setPower(0);

  temps = 0;
  
  //Firebase.setInt(firebaseDataLoop, "/carte2/temps", temps);      

}

String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void LedSelec(int nLed, int etat, String intensite)
{
  //Serial.println(nLed);
   //Serial.println(etat);
  //Serial.println(intensite);
  
  if(intensite == "-180")intensite = "25";
  if(intensite == "-132")intensite = "30";
  if(intensite == "-91")intensite = "40";
  if(intensite == "-50")intensite = "45";
  if(intensite == "0")intensite = "50";
  if(intensite == "49")intensite = "55";
  if(intensite == "90")intensite = "65";
  if(intensite == "132")intensite = "80";
  

  //si lum on
  if(nLed == 1 && etat == 1){digitalWrite(led, HIGH);acd1.setPower(intensite.toInt()); }
  if(nLed == 2 && etat == 1){digitalWrite(led, HIGH);acd2.setPower(intensite.toInt());}
  if(nLed == 3 && etat == 1){digitalWrite(led, HIGH);acd3.setPower(intensite.toInt());}
  if(nLed == 4 && etat == 1){digitalWrite(led, HIGH);acd4.setPower(intensite.toInt());}

  //si lum off
  if(nLed == 1 && etat == 0){digitalWrite(led, LOW);acd1.setPower(0);}
  if(nLed == 2 && etat == 0){digitalWrite(led, LOW);acd2.setPower(0);}
  if(nLed == 3 && etat == 0){digitalWrite(led, LOW);acd3.setPower(0);}
  if(nLed == 4 && etat == 0){digitalWrite(led, LOW);acd4.setPower(0);}


}

void loop()
{  


    //lumieres
      if (Firebase.getString(firebaseDataLoop, "/carte2/etatHoraire")) 
      {
           etatHoraire = firebaseDataLoop.stringData();
            //Serial.println("test :" + etatHoraire);
         
      }

     
      if(etatHoraire.toInt() == 0)
      {
        if(millis() - millisTimeInit0 >= 1000)
        {
        //recupere les infos lumiere + heure 8param 
        //numeroled*4/4etat/5intensite/6heureEtat/7nbrTotaltableauheure
        if (Firebase.getString(firebaseDataLoop, "/carte2/lumiere")) 
        {
          String slval = firebaseDataLoop.stringData();
          //etatHoraire = getValue(slval, '/', 0);//etat horaire active ou pas
          etatLum1 = getValue(slval, '/', 1);//etat lumiere1
          etatLum2 = getValue(slval, '/', 2);//etat lumiere2
          etatLum3 = getValue(slval, '/', 3);//etat lumiere3
          etatLum4 = getValue(slval, '/', 4);//etat lumiere4
          intensiteLum1 = getValue(slval, '/', 5);//intensite lumiere
          intensiteLum2 = getValue(slval, '/', 6);//intensite lumiere
          intensiteLum3 = getValue(slval, '/', 7);//intensite lumiere
          intensiteLum4 = getValue(slval, '/', 8);//intensite lumiere
        
          
      
          
        //mise a jour etat de la led select
        LedSelec(1, etatLum1.toInt(), intensiteLum1); 
        LedSelec(2, etatLum2.toInt(), intensiteLum2); 
        LedSelec(3, etatLum3.toInt(), intensiteLum3); 
        LedSelec(4, etatLum4.toInt(), intensiteLum4); 

            
        }
        millisTimeInit0 = millis();  
        //Serial.println("on desactive horaire");
      }   
    }

      
      //horaire lumieres
      if(etatHoraire.toInt() == 1)
      {
        if(millis() - millisTimeInit1 >= 10000)
        {
          if (Firebase.getString(firebaseDataLoop, "/carte2/horlogeLengt")) 
          {
              horlogeLengt = firebaseDataLoop.stringData();
          }

          for(int i = 0; i <= horlogeLengt.toInt(); i++)
          {
            if (Firebase.getString(firebaseDataLoop, "/carte2/lumiereHorloge/"+String(i))) 
            {
              String slval = firebaseDataLoop.stringData();
              etatLum1 = getValue(slval, '/', 0);//etat lumiere1
              etatLum2 = getValue(slval, '/', 1);//etat lumiere2
              etatLum3 = getValue(slval, '/', 2);//etat lumiere3
              etatLum4 = getValue(slval, '/', 3);//etat lumiere4
              intensiteLum1 = getValue(slval, '/', 4);//intensite lumiere
              intensiteLum2 = getValue(slval, '/', 5);//intensite lumiere
              intensiteLum3 = getValue(slval, '/', 6);//intensite lumiere
              intensiteLum4 = getValue(slval, '/', 7);//intensite lumiere
              heure = getValue(slval, '/', 8);//heures
              minutes = getValue(slval, '/', 9);//minutes

              //Serial.println(heure);
              //Serial.println(minutes);
              int minutePause = minutes.toInt()+1;
              if( h == heure.toInt() && m == minutes.toInt() && lumiereActiveHorlogetemps == 0)
              {
                //Serial.println("on active une lumiere");
                lumiereActiveHorlogetemps = 1;
                minutePause = minutes.toInt()+1;

                //mise a jour etat de la led select
                LedSelec(1, etatLum1.toInt(), intensiteLum1); 
                LedSelec(2, etatLum2.toInt(), intensiteLum2); 
                LedSelec(3, etatLum3.toInt(), intensiteLum3); 
                LedSelec(4, etatLum4.toInt(), intensiteLum4); 

              }
              if( h == heure.toInt() && m == minutePause && lumiereActiveHorlogetemps == 1)
              {
                //Serial.println("on reactive la lumiere");
                lumiereActiveHorlogetemps = 0;
              }
              
            }
          }
          millisTimeInit1 = millis();  
          //Serial.println("on active horaire");
        }
        
      }


    // Met à jour temps
    timeClient.update();
    //Serial.println(timeClient.getFormattedTime());
    
   
    h = timeClient.getHours();
   m = timeClient.getMinutes();   

    
}


/*
#include "WiFi.h"

const char* ssid = "Livebox-EF3C";
const char* password =  "5FC2DC1221F679EC21C1592ECE";
 
void setup() {
 
  Serial.begin(115200);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("WL not connected, trying again...");
    WiFi.begin(ssid, password);
    delay(10000);
  }
 
  Serial.println("Connected to the WiFi network");
 
}
 
void loop() {}
*/

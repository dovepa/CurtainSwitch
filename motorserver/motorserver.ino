#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ezButton.h>

char * ssid_ap = "curtain_switch";
char * password_ap = "curtain_switch";

// defines pins numbers
const int stepPin = 4; //D4
const int dirPin = 13; //D7
const int enPin = 12; //D6

ezButton u(D5); // D5
ezButton d(0); //D8

IPAddress ip(192,168,11,4);
IPAddress gateway(192,168,11,1);
IPAddress subnet(255,255,255,0);

ESP8266WebServer server;

int dirValue = HIGH;
int enableMotor = HIGH;
int value = 0;
int lastValue = 0;

void changeEnb(int state) {
  if (state != enableMotor) {
    enableMotor = state;
     pinMode(enPin,enableMotor);
  }
}

void changeDir(int state) {
  if (state != dirValue) {
    dirValue = state;
     pinMode(dirPin,dirValue);
  }
}

void setup() {
  u.setDebounceTime(10);
  d.setDebounceTime(10);

  Serial.begin(115200);
    
  pinMode(stepPin,OUTPUT); 
  pinMode(dirPin,OUTPUT);
  pinMode(enPin,OUTPUT);

  // Free motor at init
  digitalWrite(enPin, enableMotor);
  digitalWrite(dirPin, dirValue);

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(ip,gateway,subnet);
  WiFi.softAP(ssid_ap,password_ap);

  Serial.println();
  Serial.print("IP Address: "); Serial.println(WiFi.localIP());
  server.on("/",handleIndex); 
  server.on("/update",handleUpdate);
  server.begin();
}



void handleIndex() {
  server.send(200,"text/plain",String(value)); // we'll need to refresh the page for getting the latest value
}

void newStep() { 
   digitalWrite(stepPin,HIGH); 
   delayMicroseconds(1000); 
   digitalWrite(stepPin,LOW); 
   delayMicroseconds(1000); 
}

void moveMotor(){
    // Il y a deux cas  : open et close
    // u = bouton 1 > capteur de open mac
    // d = bouton deux > capteur de close max
    // si value = 1 ou 2 > open  > dirvalue HIGH
    // si value = 3 ou 4 > close > dirvalue LOW
    // si value = 0 ou u.isactive ou d.isactive disable motor
    // si u.isactive catch 1 et 3
    // si d.isactive catch 2 et 3

    if ((value == 1 || value == 2) && u.getState() == 1 ){
      changeDir(HIGH);
      changeEnb(LOW);
      newStep();
    }else if ((value == 3 || value == 4) && d.getState() == 1 ){
      changeDir(LOW);
      changeEnb(LOW);
      newStep();
    } else if (value == 0
     || ((value == 3 || value == 4) && d.getState() == 0) 
     || ((value == 1 || value == 2) && u.getState() == 0)
     ){
      value = 0;
      changeEnb(HIGH);
    }
    
}

void handleUpdate() {
  // The value will be passed as a URL argument
  lastValue = value;
  value = server.arg("value").toFloat();
  Serial.println(value);
  server.send(200,"text/plain","Updated");
}

void loop() {
  u.loop();
  d.loop();

   if (u.getState() == 0) {
    Serial.println(F("The button: U"));
   }
   
   if (d.getState() == 0) {
    Serial.println(F("The button: D"));
   }
   
   server.handleClient(); 
   moveMotor();
}

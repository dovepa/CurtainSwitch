
#include <ESP8266WiFi.h>
#include <ezButton.h>

ezButton u(D3); // D3
ezButton r(12); // D6
ezButton d(13); // D7
ezButton l(14); // D5

const int ledR = D2;
const int ledG = D4;

// Initialize network parameters
const char* ssid = "curtain_switch";
const char* password = "curtain_switch";
const char* host = "192.168.11.4"; // as specified in server.ino

// Set up the client objet
WiFiClient client;

// Configure deep sleep in between measurements
const int sleepTimeSeconds = 2;

void flash(int led) {
  int color = ledR;
  if (led > 0){
    color = ledG;
  }  
 digitalWrite(color,HIGH);
 delay(500);
  digitalWrite(color,LOW);
}


void setup() {
  pinMode(ledR,OUTPUT);
  pinMode(ledG,OUTPUT);
  u.setDebounceTime(10);
  r.setDebounceTime(10);
  l.setDebounceTime(10);
  d.setDebounceTime(10);
  Serial.begin(115200);

  // Connect to the server
  WiFi.begin(ssid, password);
  Serial.begin(115200);
  while (WiFi.status() != WL_CONNECTED) {
   Serial.print(".");
   flash(0);
   delay(500);
}
  Serial.println();
  Serial.print("IP Address (AP): "); Serial.println(WiFi.localIP());
}

void sendData(int data) {
 if (client.connect(host, 80)) {
   String url = "/update?value=";
   url += String(data);
   client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host +  "\r\n" +
            "Connection: keep-alive\r\n\r\n"); // minimum set of required URL headers
    flash(1);
  } else {flash(0);}
}

int last = 0;
int current = 0;
void loop() {
  u.loop();
  r.loop();
  l.loop();
  d.loop();

  // if push state == 0 else state == 1

  if (u.getState() == 0 && last == 0) {
    Serial.println(F("The button: UP"));
    sendData(1);
    last = 1;
  }
  else if (r.getState() == 0 && last == 0) {
    Serial.println(F("The button: RIGHT"));
    sendData(2);
    last = 2;
  }
  else if (l.getState() == 0 && last == 0) {
    Serial.println(F("The button: LEFT"));
    sendData(4);
    last = 4;
  }
  else if (d.getState() == 0 && last == 0) {
    Serial.println(F("The button: DOWN"));
    sendData(3);
    last = 3;
  } else if (last != 0 && d.getState() == 1 && l.getState() == 1 && r.getState() == 1 && u.getState() == 1) {
    Serial.println(F("The button is NULL"));
    if (last != 1 && last != 3){
    sendData(0);
    }
    last = 0;
  }

}

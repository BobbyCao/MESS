#include "SoftwareSerial.h"
#include <SparkFun_RHT03.h>
#include <Servo.h>

String ssid = "Ashlyn Furniture 2";
String pass = "9164784420";
String api = "V5301UYL0NY2UQMD";
String host = "api.thingspeak.com";
String port = "80";

SoftwareSerial ESPmodule(10,11);
RHT03 rht;
Servo myServo;
const int RHT03_DATA_PIN = 4;
int counter1;
int counter2;
float _h, _t;
boolean found = false;

void setup() {
  Serial.begin(9600);
  ESPmodule.begin(115200);
  rht.begin(RHT03_DATA_PIN);
  myServo.attach(7);
  myServo.write(0);
  ATcmd("AT",5,"OK", false);
  ATcmd("AT+CWMODE=1",5,"OK", false);
  ATcmd("AT+CWJAP=\""+ ssid +"\",\""+ pass +"\"",20,"OK", false);
}

void loop() {
  int updateRHT = rht.update();
  
  if (updateRHT == 1) {
    _h = rht.humidity();
    _t = rht.tempF();
    Serial.println("Humidity: " + String(_h) + " %\nTemp: " + String(_t) + " deg F");

    if (_h <= 53.9) {
      myServo.write(170);
    }
    
    String getData = "GET /update?api_key=" + api + "&field1=" + String(_h) + "&field2=" + String(_t);
    ATcmd("AT+CIPMUX=1",5,"OK", false);
    ATcmd("AT+CIPSTART=0,\"TCP\",\""+ host +"\","+ port,15,"OK", false);
    ATcmd("AT+CIPSEND=0," +String(getData.length()+4),4,">", true);
    ESPmodule.println(getData);
    delay(1500);
    counter1++;
    ATcmd("AT+CIPCLOSE=0",5,"OK", false);
    
    delay(1000);
  } else { delay (RHT_READ_INTERVAL_MS); }
}

void ATcmd(String command, int maxTime, char readReplay[], boolean B) {
  
  while(counter2 < (maxTime*1)) {
    ESPmodule.println(command);
    if(ESPmodule.find(readReplay)) {
      found = true;
      break;
    }
    counter2++;
  }
  
  if(found == true) {
    if (B) {
      Serial.println("Request Succeeded");
    }
    counter1++;
    counter2 = 0;
  } else {
    if (B) {
      Serial.println("Request Failed");
    }
    counter1 = 0;
    counter2 = 0;
  }
  
  found = false;
}

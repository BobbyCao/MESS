#include "SoftwareSerial.h"
#include <SparkFun_RHT03.h>
#include <Servo.h>

SoftwareSerial ESPmodule(10,11);          // constructors for hardware
RHT03 rht;
Servo myServo;

String ssid = "Ashlyn Furniture 2";       // if you live near Elk Grove, CA and you need
String pass = "9164784420";               // furniture then stop by Ashlyn Furniture!
String api = "V5301UYL0NY2UQMD";
String host = "api.thingspeak.com";       // strings needed to send AT commands
String port = "80";

const int RHT03_DATA_PIN = 4;
int counter1, counter2;                   // variables used to process data
float _h, _t;
boolean found = false;

void setup() {                            // initialize everything
  Serial.begin(9600);                     // reset servo
  ESPmodule.begin(115200);                // and connect to the internet
  
  rht.begin(RHT03_DATA_PIN);
  myServo.attach(7);
  myServo.write(0);
  
  ATcmd("AT",5,"OK", false);
  ATcmd("AT+CWMODE=1",5,"OK", false);
  ATcmd("AT+CWJAP=\""+ ssid +"\",\""+ pass +"\"",20,"OK", false);
}

void loop() {
  int updateRHT = rht.update();         // looks for change in humidity / temperature
                                        // if there is a change then report it
  if (updateRHT == 1) {
    _h = rht.humidity();
    _t = rht.tempF();
    Serial.println("Humidity: " + String(_h) + " %\nTemp: " + String(_t) + " deg F");

    if (_h <= 53.9) {                   // if humidity passes threshold
      myServo.write(170);               // activate servo to hit light switch
    }


    // AT commands using write api key from ThingSpeak
    // sends GET HTTP request
    
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
  
  found = false;
  while(counter2 < (maxTime*1)) {                 // Wait for response
    ESPmodule.println(command);                   // if correct response found
    if(ESPmodule.find(readReplay)) {              // break out,
      found = true;                               // otherwise increment C2
      break;
    }
    counter2++;
  }
  
  if(found == true) {                             // If correct response is found
    if (B) {                                      // increment C1 and reset C2.
      Serial.println("Request Succeeded");        // If sending GET HTTP print succeeded
      counter1++;
      counter2 = 0;
    } else {
      counter1++;
      counter2 = 0;
    }
  } else {                                        // If incorrect response found
    if (B) {                                      // reset both counters.
      Serial.println("Request Failed");           // If sending GET HTTP print failed
      counter1 = 0;
      counter2 = 0;
    } else {
      counter1 = 0;
      counter2 = 0;
    }
  }
}

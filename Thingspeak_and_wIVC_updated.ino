#include <ESP8266WiFi.h>
#include "secrets.h"
#include "ThingSpeak.h" 


#include <Servo.h>
#include <DHT.h>
#include <DHT_U.h>
#define DHTPIN 14          
DHT dht(DHTPIN,  DHT11);

const int servoPin = 5;
const int enablePin = 12;
const int motorPin1 = 13;
const int motorPin2 = 15;
int motorSpeed = 250; // Adjust motor speed (0-255)
Servo myservo;

// Enter your Wifi Credentials
// replace x with your details
char ssid[] = "xxxxxxxxxxxxx";   
char pass[] = "xxxxxxxxx";   
int keyIndex = 0;            
WiFiClient  client;

// Enter Thingspeak Channels Credentials
// Create separate channels for each. The gas sensor and temp&humidity sensor can have same channel.
// Note: " The Thingspeak platform accepts data every 15 seconds, which means there should be a minimum delay of 15 seconds between uploads. 
//         This is not much important for sensor values, but when testing, make sure the orders you upload meet the requirements."

// Order channel
unsigned long channelid = xxxxxxxxx;
const char* readapikey = "xxxxxxxxx" ;
const char* myWriteAPIKey = "xxxxxxxxxxx";
unsigned int fieldnumber = x;

// Gas Sensor channel details
unsigned long gaschannelid = xxxxxxx;
const char* g_readapikey = "xxxxxxxxxxx";
const char* g_writeapikey = "xxxxxxxxxxxxx";

// T&H Channel Details
unsigned long tandhchannelid = xxxxxxxxxxx;
const char* tandh_readapikey = "xxxxxxxxxxxxxxxx";
const char* tandh_writeapikey = "xxxxxxxxxxxxxx";

void setup() {
  
  Serial.begin(115200);  // Initialize serial monitor

  // Setting Pins
  pinMode(servoPin, OUTPUT);
  pinMode(enablePin, OUTPUT);
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  
  // Initalizing Wifi and Thingspeak

  WiFi.mode(WIFI_STA); 
  ThingSpeak.begin(client);  // Initialize ThingSpeak
   // Connecting or reconnecting to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass); 
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected");
  }
  
  // Closing gate
  myservo.attach(servoPin);
  myservo.write(0);

}

void loop() {
 
  // Checking Storage's Temperature, Humidity and Spoilage
  int gas =analogRead(A0);
  Serial.print("Gas :");
  Serial.println(gas);
  ThingSpeak.writeField(gaschannelid, 1, gas, g_writeapikey);;

  int h = dht.readHumidity();
  int t = dht.readTemperature();
  int hic = dht.computeHeatIndex(t, h, false);
  Serial.print("Temperature : ");
  Serial.println(hic);
  ThingSpeak.writeField(tandhchannelid, 1, hic, tandh_writeapikey);;
  Serial.print("Humidity : ");
  Serial.println(h);
  ThingSpeak.writeField(tandhchannelid, 2, h, tandh_writeapikey);;
 
  // Reading order
  int DishInF = ThingSpeak.readFloatField(channelid, fieldnumber, readapikey);  
  Serial.print("Order info: ");
  Serial.println(DishInF);


  if (DishInF == 1)
  {
    startSERVOmotor();
    startDCmotor(5000);
  } 
  else if (DishInF == 2) 
  {
    startSERVOmotor();
    startDCmotor(10000);
  } 
  else if (DishInF == 3) 
  {
    
    startSERVOmotor();
    startDCmotor(15000);
  } 
  else {
    // Stop servo and motor
    myservo.write(0);
    digitalWrite(enablePin, LOW);
    Serial.println("wVIC State: Not chopping");
  }
  
  delay(500); 
  
}

void startSERVOmotor()
{
  // Opening Storage Gate 
  for (int pos = 0; pos <= 180; pos += 1) {
      myservo.write(pos);
      delay(15);
    }
  // Closing Storage Gate
    for (int pos = 180; pos >= 0; pos -= 1) {
      myservo.write(pos);
      delay(15);
    }
}

int startDCmotor(int d)
{
 
  digitalWrite(enablePin, HIGH);  // Enable motor driver
  digitalWrite(motorPin1, HIGH);  // Forward
  digitalWrite(motorPin2, LOW);
  Serial.println("wVIC: Chopping");
  delay(d);                 

  digitalWrite(enablePin, LOW);  // Stop motor
  return 0;
}

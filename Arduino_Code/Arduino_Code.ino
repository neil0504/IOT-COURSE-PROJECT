#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>


const char* ssid = "GOT";
const char* password = "Aranav@19092011";

//const char* ssid = "Neil";
//const char* password = "123456789";

//Your Domain name with URL path or IP address with path
const char* serverName = "http://iot-cp-new.herokuapp.com/sendData";

MAX30105 particleSensor;
float beatsPerMinute;
int beatAvg;
int val;

int tempOutput = A0;
float temp;

//const byte RATE_SIZE = 10; //Increase this for more averaging. 4 is good.
//byte rates[RATE_SIZE]; //Array of heart rates
//byte rateSpot = 0;
//long lastBeat = 0; //Time at which the last beat occurred

const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; //Time at which the last beat occurred


// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 5000;



void initiateHeartBeatSensor(){
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30102 was not found. Please check wiring/power. ");
    while (1);
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");

  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED
}

float getReading(){
  Serial.println("Inside getReading Function");
  val = analogRead(tempOutput);
//  (analogValue/1024.0) * 3300
  float mv = ( val/1024.0)*3300;
  float cel = mv/10;
  float farh = (cel*9)/5 + 32;
  Serial.println("Returning Temp = "+String(cel));
  return cel;
}
float takeTempReading(){
  Serial.println("Inside takeTempReading Function");
  int c = 0;
  float temp1;
  // skip first records
  Serial.println("Skipping first 15 reading");
  while(c<15){
    temp1 = getReading();
    c++;
  }
  Serial.println("getting next 10 reading");
  // take average of next 10 recoeds
  c = 0;
  temp1=0;
  float tempavg = 0;
  int unsuccessfullReading = 0;
  int error = 0;
  
  while(c<10){
    temp1 = getReading();
    if(temp1 > 15 && temp1 < 50){
      Serial.println("Successfull Reading no.: " + String(c));
      tempavg += temp1;
      c++;
    }else{
      Serial.println("Temp Reading Unsuccessfull");
      unsuccessfullReading++;
      if(unsuccessfullReading > 20){
        error = 100;
        break;
      }
    }
  }
  if(error == 100){
    Serial.println("Returnin 0 ERROR");
    return 0;
  }else{
    Serial.println("Returnin Temp = " + String(tempavg/10));
    return tempavg/10;
  }
}

int takeReadingHeartRate(){
  rates[RATE_SIZE] = 0; //Array of heart rates
  rateSpot = 0;
  lastBeat = 0; //Time at which the last beat occurred
  
  beatsPerMinute = 0;
  float beatsPerMinuteTotal = 0;
  beatAvg = 0;
  int successfullReading = 0;
  int unsuccessfullReading = 0;
  
  while(1){
    long irValue = particleSensor.getIR();
    if (irValue < 50000){
      Serial.println(" No finger? Place Finger to Begin");
      delay(500);
      unsuccessfullReading++;
      if(unsuccessfullReading > 50){
        Serial.println("Failed to record heartRate");
        beatsPerMinuteTotal = 0;
        beatAvg = 0;
        break;
      }
      
    }else if(checkForBeat(irValue) == true){
      
        //We sensed a beat!
        long delta = millis() - lastBeat;
        lastBeat = millis();
    
        beatsPerMinute = 60 / (delta / 1000.0);
    
        if (beatsPerMinute < 255 && beatsPerMinute > 20)
        {
          rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
          rateSpot %= RATE_SIZE; //Wrap variable
          successfullReading++;
//          beatsPerMinuteTotal += beatsPerMinute;
          
          //Take average of readings
          beatAvg = 0;
          for (byte x = 0 ; x < RATE_SIZE ; x++)
            beatAvg += rates[x];
          beatAvg /= RATE_SIZE;
          Serial.println("Successfull reading no.: " + String(successfullReading));
          delay(1000);
        
      }
      if(successfullReading >= 4){
        Serial.println("4 successfull reading recorded");
//        Serial.print(", Avg BPM=");
//        Serial.println(beatAvg);
        break;
      }
    }
  }
  
//  return beatsPerMinuteTotal/10;
  return beatAvg;
}

void sendPostRequest(float temp, int heartRate, float oxygen_level){
  if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;
      
      // Your Domain name with URL path or IP address with path
      http.begin(client, serverName);

      // Specify content-type header
//      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
//      // Data to send with HTTP POST
//      String httpRequestData = "api_key=tPmAT5Ab3j7F9&sensor=BME280&value1=24.25&value2=49.54&value3=1005.14";           
      // Send HTTP POST request
//      int httpResponseCode = http.POST(httpRequestData);
      
      // If you need an HTTP request with a content type: application/json, use the following:
      http.addHeader("Content-Type", "application/json");
      int httpResponseCode = http.POST("{\"patient_name\":\"Neil's Device\",\"entry_time\":\"NOT ADDED\",\"temperature\":\"" + String(temp) + "\",\"heart_rate\":\""+String(heartRate)+"\",\"oxygen_level\":\""+String(oxygen_level)+"\"}");

      // If you need an HTTP request with a content type: text/plain
      //http.addHeader("Content-Type", "text/plain");
      //int httpResponseCode = http.POST("Hello, World!");
     
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
        
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
}

void setup() {
  Serial.begin(9600);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
  initiateHeartBeatSensor();
}

void loop() {
  //Send an HTTP POST request every 10 minutes
  if(Serial.available()){

    if(Serial.read() == '1'){
      int avgRate = takeReadingHeartRate();
      float temp = takeTempReading();
      sendPostRequest(temp, avgRate, 99.99);
    }
  }
  delay(10);
}

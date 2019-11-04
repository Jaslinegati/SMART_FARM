//MWITA GATI JASLINE//
//P15/33850/2014//
//REMOTE CROP MONITORING SYSTEM//



#include "DHT.h"
#include <Adafruit_Sensor.h>
#include <SoftwareSerial.h>
#include <stdlib.h>

#define DHTPIN 7    
#define DHTTYPE DHT11   

DHT dht(DHTPIN, DHTTYPE);



SoftwareSerial wifiSerial(3,2);      // RX, TX for ESP8266

bool DEBUG = true;   //show more logs
int responseTime = 5; //communication timeout

String HOST = "api.thingspeak.com";
String API_KEY = "M8A62H9NGOGEPGPI"; 
String AP = "fatma";
String PASSWORD = "burntforest";



void setup()
{
  pinMode(13,OUTPUT);  //set build in led as output
  // Open serial communications and wait for port to open esp8266:
  Serial.begin(9600);
  wifiSerial.begin(9600);
  dht.begin();

  
  Serial.println("Setup process beginning...");
  delay(3000);
  sendToWifi("AT+CWMODE=1",responseTime,DEBUG); // configure as access point
  delay(3000);
  sendToWifi("AT+CIPMUX=1",responseTime,DEBUG); // configure for multiple connections
  delay(3000);
  sendToWifi("AT+CWJAP=\""+ AP +"\",\""+ PASSWORD +"\"", responseTime, DEBUG);
  delay(3000);
  sendToWifi("AT+CIFSR",responseTime,DEBUG); // get ip address
  sendToUno("Wifi connection is running!",responseTime,DEBUG);
  

}


void loop()
{
    // read the input on analog pin 0: 
    
  int soilMoisture = analogRead(A0);

    // print out the value you read:
  Serial.println( soilMoisture);
  delay(2000); // delay in between reads for stability

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float humidity = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float temperature = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temperature) || isnan(soilMoisture)) {
    Serial.println(F("Failed to read from DHT sensor!"));
   
  }

  else {
    sendDataToThingspeak(humidity, "field2");
      delay(15000);

    sendDataToThingspeak(temperature, "field4");
      delay(15000);

     sendDataToThingspeak(soilMoisture, "field3");
    }
 
  
  delay(30000);

  
}


/*
* Name: sendDataToThingspeak
* Description: Function used to send string to tcp client using cipsend
* Params: 
* Returns: void
*/
void sendDataToThingspeak(int data, String fieldNumber){
  String url =  "/update?api_key=" + API_KEY + "&" + fieldNumber + "=" + data;
  sendToWifi("AT+CIPSTART=0,\"TCP\",\"" + HOST + "\",80", responseTime, DEBUG); 
  delay(3000);
  sendToWifi("AT+CIPSEND=0,50",responseTime,DEBUG);
  delay(2000);
  sendToWifi("GET " + url + "\r\n",responseTime,DEBUG);
  delay(3000);
  sendToWifi("AT+CIPCLOSE",responseTime,DEBUG);
}



/*
* Name: sendDataToTwilio
* Description: Function used to send string to tcp client using cipsend
* Params: 
* Returns: void
*/



/*
* Name: find
* Description: Function used to match two string
* Params: 
* Returns: true if match else false
*/
boolean find(String string, String value){
  return string.indexOf(value)>=0;
}


/*
* Name: readSerialMessage
* Description: Function used to read data from Arduino Serial.
* Params: 
* Returns: The response from the Arduino (if there is a reponse)
*/
String  readSerialMessage(){
  char value[100]; 
  int index_count =0;
  while(Serial.available()>0){
    value[index_count]=Serial.read();
    index_count++;
    value[index_count] = '\0'; // Null terminate the string
  }
  String str(value);
  str.trim();
  return str;
}



/*
* Name: readWifiSerialMessage
* Description: Function used to read data from ESP8266 Serial.
* Params: 
* Returns: The response from the esp8266 (if there is a reponse)
*/
String  readWifiSerialMessage(){
  char value[100]; 
  int index_count =0;
  while(wifiSerial.available()>0){
    value[index_count]=wifiSerial.read();
    index_count++;
    value[index_count] = '\0'; // Null terminate the string
  }
  String str(value);
  str.trim();
  return str;
}



/*
* Name: sendToWifi
* Description: Function used to send data to ESP8266.
* Params: command - the data/command to send; timeout - the time to wait for a response; debug - print to Serial window?(true = yes, false = no)
* Returns: The response from the esp8266 (if there is a reponse)
*/
String sendToWifi(String command, const int timeout, boolean debug){
  String response = "";
  wifiSerial.println(command); // send the read character to the esp8266
  long int time = millis();
  while( (time+timeout) > millis())
  { 
    while(wifiSerial.available())
    {
    // The esp has data so display its output to the serial window 
    char c = wifiSerial.read(); // read the next character.
    response+=c;
    }  
   }
  if(debug)
  {
    Serial.println(response);
  }
  return response;
}

/*
* Name: sendToUno
* Description: Function used to send data to Arduino.
* Params: command - the data/command to send; timeout - the time to wait for a response; debug - print to Serial window?(true = yes, false = no)
* Returns: The response from the esp8266 (if there is a reponse)
*/
String sendToUno(String command, const int timeout, boolean debug){
  String response = "";
  Serial.println(command); // send the read character to the esp8266
  long int time = millis();
  while( (time+timeout) > millis())
  {
    while(Serial.available())
    {
      // The esp has data so display its output to the serial window 
      char c = Serial.read(); // read the next character.
      response+=c;
    }  
  }
  if(debug)
  {
    Serial.println(response);
  }
  return response;
}



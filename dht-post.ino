#include <DHT.h>
#include <SPI.h>
#include <Ethernet.h>
#include <Wire.h>

 
// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)
char server[] = "api.scalenics.io";    // name address for Google (using DNS)

// dtostrf buffer
char s[16];
char t[16];

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192,168,0,9);

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;



/*
  AM2302 Temperature / Humidity Sensor (DHT22)
  
  Current Code
  Created by Derek Erb 30/01/2013
  Modified 30/01/2013
  
  Requirement : Arduino + AM2302 sensor connected to pin 2
  
  DHT Library from:
  https://github.com/adafruit/DHT-sensor-library

  v0.03 Delays and formatting
  v0.02 Delay 3 seconds between readings
*/

// Version number
const float fVerNum = 0.03;

// Data pin connected to AM2302
#define DHTPIN 2

#define DHTTYPE DHT22       // DHT 22  (AM2302)

DHT dht(DHTPIN, DHTTYPE);   // LED pins


// scalenics 
#define DEVICE_TOKEN "FA2EFC30EC66E07DFD5B7B4124A99E49"
#define DEVICE_ID "arduino01"



void setup() {
 // Open serial communications and wait for port to open:
  Serial.begin(9600);
   while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  dht.begin();

  }

  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    Ethernet.begin(mac, ip);
  }
  // print your local IP address:
  Serial.print("My IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print("."); 
  }
  Serial.println();
}


void loop()
{
  // Read DHT
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Read DHT fail
  if (isnan(t) || isnan(h)) {
    Serial.println(F("Failed to read from DHT"));
  }
  else {
    Serial.print(F("Humidity: ")); 
    Serial.print(h);
    Serial.print(F(" %\t"));
    Serial.print(F("Temperature: ")); 
    Serial.print(t);
    Serial.println(F(" C"));
  }
  
  // give the Ethernet shield a second to initialize:
  // delay(1000);
  Serial.println("connecting...");

  // DEVICE_TOKEN
  String Auth_header;
  Auth_header = "X-DEVICETOKEN:";
  Auth_header+= DEVICE_TOKEN;
  
  String Deviceid_header;
  Deviceid_header = "X-DEVICEID:";
  Deviceid_header+= DEVICE_ID;

  // serialize env_data to HTTP_POST strings
  String PostData;
  PostData = "";
  PostData+= "v=";
  PostData+= dtostrf(t,4,1,s);
  PostData+= "&v2=";
  PostData+= dtostrf(h,4,1,s);
  // for debug
  Serial.println("DEVICE_TOKEN=");
  Serial.println(Auth_header);
  Serial.println("DEVICE_ID=");
  Serial.println(Deviceid_header);
  Serial.println("POST strings=");
  Serial.println(PostData);

  
  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    Serial.println("connected");
    // Make a HTTP request:
    client.println("POST /post/ HTTP/1.1");
    client.println("Host: api.scalenics.io");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println(Auth_header);
    client.println(Deviceid_header);
    client.println("User-Agent: Arduino/1.0");
    client.println("Connection: close");
    client.print("Content-Length: ");
    client.println(PostData.length());
    client.println();
    client.println(PostData);
    Serial.println(PostData);
  } 
  else {
    // Connection fail
    Serial.println("connection failed");
  }

  // Read response
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

  // Disconnect
    Serial.println();
    Serial.println("disconnecting.");
    delay(1000);
    client.stop();
    
    Serial.println("Waiting for next loop...");
    delay(600000); //just here to slow down the output. You can remove this
    //delay(30000);
  
  
}


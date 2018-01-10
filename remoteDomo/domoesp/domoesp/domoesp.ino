/*
 Basic ESP8266 MQTT example

 This sketch demonstrates the capabilities of the pubsub library in combination
 with the ESP8266 board/library.

 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic" every two seconds
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the first character of the topic "inTopic" is an 1, switch ON the ESP Led,
    else switch it off

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.

 To install the ESP8266 board, (using Arduino 1.6.4+):
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Jupiter";
const char* password = "3827310955370393";
const char* mqtt_server = "192.168.100.54";

#define clientId "ESPWoon2"
char unitId = '6'; //Unit id
int idx1 = 999; //IDX number for domoticz
float data1 = 0.0; //Datapoint
float data2 = 0;

int relay1 = 16; //Hardwire output
int relay2 = 14; //Hardwire output

int val = 0; 

unsigned long last;
unsigned long interval = 300000; //Interval to send sensor data

unsigned long lastmotion = 0;
unsigned long intervalmotion = 900000; //Interval to send sensor data

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  digitalWrite(relay1,LOW);
  digitalWrite(relay2,LOW);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback); 
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  
  client.loop();

   if ((millis() - last) >= interval){ //To start sending sensordata
    data1 = 0.0;
    data2 = 0.0;
    //sensorDataout(idx1, data1, data2);
    last = millis();   
  }

  //Read connected sensors

    
}//end main loop

void sensorDataout(int idx, float data, int data2){
      Serial.println("Sending data....");
      String string = " { \"idx\" : ";
      string.concat(idx);
      string.concat(", \"nvalue\" : ");
      string.concat(data2);
      string.concat(", \"svalue\" : \"");
      string.concat(data);
      string.concat("\" \}");
      Serial.println(string);
      char output[80];
      string.toCharArray(output, 80);
      //char data2[] ={"\{\"command\": \"switchlight\", \"idx\": 14, \"switchcmd\": \"Off\", \"level\": 100\}"};
      client.publish("domoticz/in",output);
      // ... and resubscribe
      client.subscribe("domoticz/arduino"); 
}

void relayOut(char relay, char state){  
  Serial.println("relais");
  switch (relay) {
    case '0':  
      switch (state){
        case '0':
        //digitalWrite(rood, LOW);
        break;
        case '9':
        //digitalWrite(rood, HIGH);
        break;
      }
    break;   
    case '1':  
      switch (state){
        case '0':
        //digitalWrite(geel, LOW);
        break;
        case '9':
        //digitalWrite(geel, HIGH);
        break;
      }
    break;
    case '2':  
      switch (state){
        case '0':
        //digitalWrite(BLAUW, LOW);
        break;
        case '9':
        //digitalWrite(BLAUW, HIGH);
        break;
      }
    break;
    case '3':  
      switch (state){
        case '0':
        Serial.println("relais 1 uit");
        digitalWrite(relay1, LOW);
        break;
        case '9':
        Serial.println("relais 1 aan");
        digitalWrite(relay1, HIGH);
        break;
      }
    break;
    case '4':  
      switch (state){
        case '0':
        Serial.println("relais 2 uit");
        digitalWrite(relay2, LOW);
        break;
        case '9':
        Serial.println("relais 2 aan");
        digitalWrite(relay2, HIGH);
        break;
      }
    break;      
  }
}

void lightOut(int idx, char cmd[]){  
      Serial.println("lightOut");
      //char data2[] ={"\{\"command\": \"switchlight\", \"idx\": 26, \"switchcmd\": \"Off\", \"level\": 100\}"};
      String string = "\{\"command\": \"switchlight\", \"idx\": ";
      string.concat(idx);
      string.concat(", \"switchcmd\": \"");
      string.concat(cmd);
      string.concat("\", \"level\": 100\} ");
      char data2[80];
      string.toCharArray(data2, 80);
      client.publish("domoticz/in",data2);
      // ... and resubscribe
      client.subscribe("domoticz/arduino"); 
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);      
  }
  Serial.println();
  if ((char)payload[0] == unitId){
   
    switch ((char)payload[1]){
      case 'r':
       relayOut( (char)payload[2],(char)payload[3] );
       break;
    }//end switch case
    }//enf first iff
  
  else {
      Serial.println("reject unitId");
      }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(clientId)) {
      Serial.println("connected");
      client.subscribe("domoticz/arduino");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}



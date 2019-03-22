//ESP Garage

#include <ESP8266WiFi.h>
#include <PubSubClient.h>


const char* ssid = "Jupiter";
const char* password = "3827310955370393";
const char* mqtt_server = "192.168.100.54";

#define clientId "ESPSONOF"
char unitId = 'A'; //Unit id
int idx1 = 258; //IDX number for domoticz
float tempIn = 0.0; //Datapoint

//#define debug

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long startMillis;  //some global variables available anywhere in the program
unsigned long currentMillis;
const unsigned long period = 1000;  //the value is a number of milliseconds

void setup() {
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

    
}//end main loop





void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
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

  Serial.println();
  WiFi.printDiag(Serial);
  Serial.println();
  
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
      case 'w':
      char tempA[6];
       for (int i = 0; i < 5 ; i++) { // empty array
          tempA[i]= 0;
          }
          
       for (int i = 2; i < 7 ; i++) { // fill array with data
          tempA[i-2]= ((char)payload[i]);
          //Serial.print(tempA[i-2]);
          }
      
      tempIn = atof(tempA); //convert data to float
      //Serial.println();
      //Serial.println(tempIn);
       break;
      case 'a':
       //alarmState( (char)payload[2],(char)payload[3] );
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



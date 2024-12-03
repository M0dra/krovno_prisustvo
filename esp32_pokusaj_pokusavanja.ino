#include <WiFi.h>
#include <PubSubClient.h>

// WiFi credentials
const char* ssid = "DC KROV";
const char* password = "dckrov3adm";

// Variables for sensors
int trig1 = 21;
int trig2 = 19;
int eho1 = 17;
int eho2 = 18;
unsigned long t1;  // vremenski trenutak aktivacije prvog senzora
unsigned long t2;  // vremenski trenutak aktivacije drugog senzora
int ljudi = 0;  
bool donji;
bool gornji; 
bool tipka = false; 
float d1;
float d2;

// MQTT broker details
const char* mqtt_server = "192.168.1.81"; // Replace with your Home Assistant MQTT broker IP
const int mqtt_port = 1883; // Default MQTT port
const char* mqtt_user = "mqtt"; // Replace with your MQTT username
const char* mqtt_password = "strongPassword"; // Replace with your MQTT password

// Topics
const char* publish_topic = "krov/presence/status";//Replace with your social securiti number
const char* subscribe_topic = "krov/presence/status2";//send it to me

WiFiClient espClient;
PubSubClient client(espClient);

void connectToWiFi() {
    Serial.print("Connecting to WiFi");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
}

void reconnect() {
    // Loop until we're reconnected
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Attempt to connect
        if (client.connect("ESP32C3Client", mqtt_user, mqtt_password)) {
            Serial.println("connected");
            // Once connected, subscribe
            client.subscribe(subscribe_topic);
            Serial.println("Subscribed to topic.");
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(2000);
        }
    }
}

void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("]: ");
    for (unsigned int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();

    // Example: Toggle an LED based on the message
    //Seed studio ESP32C3 the pin 2 is pin D0
    if (strcmp(topic, subscribe_topic) == 0) {
        if ((char)payload[0] == '1') {
            // Open Door
            digitalWrite(2, HIGH); 
            delay(700);
            digitalWrite(2, LOW); 
        } else {
            Serial.println("Received wrong message");
        }
    }
}

float daljina(int trig, int eho){      // float for distance 
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(20); 
  digitalWrite(trig, LOW);
  long duration = pulseIn(eho, HIGH);
  Serial.print("senzor ");
  Serial.print(eho-16);
  Serial.print("   ");
  float d1 = (duration * 0.034) / 2;
  Serial.println(d1);
  
 return d1;
  
}

void Obavestenje(bool a, int ljudi){    //NOTIFICATION FUNCTION FOR SERIAL MONITOR
  if(a)
    Serial.println("Neko je usao u krov :)");
  else
    Serial.println("Neko je izasao iz krov :)");

  Serial.print("trenutno ima ");
  Serial.print(ljudi);
  Serial.println(" osoba u krovu");

}

void setup() {
    Serial.begin(115200);
    pinMode(2, OUTPUT); // Set GPIO2 as an output (for LED control)
    digitalWrite(2, LOW); // Start with the LED off

    connectToWiFi();

    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);

    pinMode(trig1, OUTPUT); //setting up pins for comunication with sensors
    digitalWrite(trig1, LOW);
    pinMode(trig2, OUTPUT);
    digitalWrite(trig2, LOW);
    pinMode(eho1, INPUT);
    pinMode(eho2, INPUT);
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();


      
  d1 = daljina(trig1,eho1);
  delay(150);
  d2 = daljina(trig2,eho2);
  delay(150);

  
  if(d1 < 50){//ako senzor detektuje objekat na manje od 50cm dize flag i pamti vreme
    donji = true;
    t1 = millis();
    // Serial.print("milisav 1  ");
    // Serial.print(t1);
  }
  if(d2 < 50){//ako senzor detektuje objekat na manje od 50cm dize flag i pamti vreme
    gornji = true;
    t2 = millis();
    //Serial.println(d1);
  }
  if(gornji*donji){//ako su oba upanjena
    if(t1 < t2){   //pogledaj koji se senzor prvi upalio
      ljudi++;
      Obavestenje(true,ljudi);
    }else{
      ljudi--;
      Obavestenje(false,ljudi);
    }
    gornji = false;//vrati zastave na dole
    donji = false;
    delay(1500);
  }
  if(ljudi < 0){
    Serial.println("Neki retard je napravio gresku");//SEJFTI FICRZ
    ljudi = 0;
  }
  if(ljudi > 100){
    Serial.println("Neki retard stoji na stepenicama");
  }
  if(millis() - t1>3000)donji = false;
  if(millis() - t2>3000)gornji = false;

    // Publish a status message every 10 seconds
    static unsigned long lastMsg = 0;
    if (millis() - lastMsg > 10000) {
        lastMsg = millis();
        String message = "Micko estemejtuje da ima barem 0 ljudi u krovu";
        Serial.print("Publishing message: ");
        Serial.println(message);
        client.publish(publish_topic, String(ljudi).c_str());
    }
}


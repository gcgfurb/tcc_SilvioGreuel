//#MQTT_MAX_PACKET_SIZE 255

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define TOGGLE 3
#define DLED D7
#define DRELAY D6

//WIFI
const char* WIFI_SSID = "iot";
const char* WIFI_PASS = "12345678";

//MQTT
const char* MQTT_IP = "athletic-prawn.rmq.cloudamqp.com";
//const char* MQTT_IP = "broker.greuel.com.br";
const int   MQTT_PORT = 1883;
const char* MQTT_USER = "iot";
const char* MQTT_PASS = "iot";
const char* MQTT_DEVICE = "11111111-1111-1111-1111-111111111111";

void setup_wifi();
void reconnect_mqtt();
void setup_mqtt();
void callback_mqtt(char* topic, byte* payload, unsigned int length);
int getJustSwitch(String data);
int getPin(String data);
int getSwitch(String data);

WiFiClient client;
PubSubClient mqtt(MQTT_IP, MQTT_PORT, callback_mqtt, client);

void setup() {
	Serial.begin(115200);
  while(!Serial) { }

	//for(int pin = 0; pin <= 13; pin++) {
		pinMode(DLED, OUTPUT);
    pinMode(DRELAY, OUTPUT);
	//}

	setup_wifi();
	setup_mqtt();
}

void setup_wifi() {
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  
  while (WiFi.status() != WL_CONNECTED) {
		digitalWrite(DLED, LOW);
    delay(500);
    Serial.print(".");
		Serial.println(WiFi.localIP());
    Serial.println(WiFi.status());
    digitalWrite(DLED, HIGH);
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  Serial.println("Server started");

	Serial.print("ip:");
  Serial.println(WiFi.localIP());
}

void reconnect_mqtt() {
  while (!mqtt.connected()) {
    Serial.println("Connecting to MQTT...");
 
    if (mqtt.connect(MQTT_DEVICE, MQTT_USER, MQTT_PASS)) {
      Serial.println("mqtt connected");  
    } else {
      Serial.print("failed with state ");
      Serial.print(mqtt.state());
      delay(2000);
    }
  }
} 

void setup_mqtt() {
  reconnect_mqtt();
  if(mqtt.subscribe("telemetry/sensor")) {
    Serial.println("subscribed");
  } 
}

void callback_mqtt(char* topic, byte* payload, unsigned int length) {
	String data = String((char*)payload);
  Serial.println(data.c_str());
	int justSwitch = getJustSwitch(data);
	int pin = getPin(data);
	int sw = getSwitch(data);

	if (justSwitch > 0) {
		digitalWriteWithToggle(DRELAY, sw);
		digitalWriteWithToggle(DLED, sw);
	} else {
		digitalWriteWithToggle(pin, sw);
	}
}

void digitalWriteWithToggle(int pin, int sw) {
	if (pin < 0 || sw < 0) return;

  if (sw == TOGGLE) {
    digitalWrite(pin, !digitalRead(pin));
  } else {
    digitalWrite(pin, sw);
  }
}

void loop() {
  reconnect_mqtt();
  mqtt.loop();
}

int getJustSwitch(String data) {
	if(data.indexOf("switch") > 1) return 1;
	return -1;
}

int getPin(String data) {
	if(data.indexOf("d0") > 1) return D0;
	if(data.indexOf("d1") > 1) return D1;
	if(data.indexOf("d2") > 1) return D2;
	if(data.indexOf("d3") > 1) return D3;
	if(data.indexOf("d4") > 1) return D4;
	if(data.indexOf("d5") > 1) return D5;
	if(data.indexOf("d6") > 1) return D6;
	if(data.indexOf("d7") > 1) return D7;
	if(data.indexOf("d8") > 1) return D8;
	if(data.indexOf("d9") > 1) return D9;
	if(data.indexOf("d10") > 1) return D10;

	return -1;
}

int getSwitch(String data) {
	if(data.indexOf("high") > 1) return HIGH;
	if(data.indexOf("low") > 1) return LOW;
  if(data.indexOf("toggle") > 1) return TOGGLE;
	return -1; 
}
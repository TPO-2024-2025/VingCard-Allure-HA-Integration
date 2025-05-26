#include <PubSubClient.h>
#include <ESP8266WiFi.h>

// +==================================================+
// |                                                  |
// |               CONFIG START                       |
// |                                                  |
// +==================================================+

/*
  SSID of the WiFi network that the device should connect to. This is required in order to send data to the MQTT broker
  Example: IHGnetwork
*/
const char* wifiSsid = "IHGnetwork";

/*
  Password of the WiFi network
  Example: password123
*/
const char* wifiPassword = "password123";

/*
  IP address of MQTT broker
  Example: 192.168.43.212
*/
const char* ipAddress = "192.168.43.212";

/*
  Unique identifier for the publisher, recommended format: "soba102{room number}"
  Example: soba102
*/
const char* publisherId = "soba102";

/*
  Topic name, this should only include the room number
  Example: 102
*/
const char* publisherTopic = "101";

/*
  Username for publisher, follows format: "soba{room number}"
  Example: "soba102"
*/
const char* publisherUsername = "soba102";

/*
  Password for publisher, follows format: "{room number in reverse}abos" (reversed username)
  Example: "201abos"
*/
const char* publisherPassword = "201abos";

// +==================================================+
// |                                                  |
// |               CONFIG END                         |
// |                                                  |
// +==================================================+

WiFiClient wifiClient;
PubSubClient client(ipAddress, 1883, NULL, wifiClient);

#define MAX_DATA 2000

const long idl = 0b01001111001100100000111101110100;
const long mur = 0b01001011010100110000101100010101;
const long dnd = 0b01000111111100000000011110110110;

int data[MAX_DATA] = {0};
int dataCounter = 0;

int state = -1;

void wifiConnect() {
  if (WiFi.status() == WL_CONNECTED) return;

  Serial.printf("\nConnecting to WiFi");
  WiFi.begin(wifiSsid, wifiPassword);
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.printf("\nConnected to WiFi!\n");
}

void mqttConnect() {
  if (client.connected()) return;

  Serial.printf("\nConnecting to MQTT broker");
  client.connect(publisherId, publisherUsername, publisherPassword);
  while (!client.connected()) {
    Serial.print(".");
    delay(100);
  }
  Serial.printf("\nConnected to MQTT broker as number %s!\n", publisherTopic);
}

void sendData() {

  wifiConnect();
  mqttConnect();

  if (state == -1) {
    client.publish(publisherTopic, "NDF");
  } else if (state == 0) {
    client.publish(publisherTopic, "IDL");
  } else if (state == 1) {
    client.publish(publisherTopic, "MUR");
  } else if (state == 2) {
    client.publish(publisherTopic, "DND");
  }
}

int countBits(int n) {
    int count = 0;
    while (n) {
        n &= (n - 1);
        count++;
    }
    return count;
}

void changeState() {
  long bitState = 0;
  for (int i = 0; i < dataCounter; i++) {
    
    if (data[i] < 500) bitState = bitState << 1;
    if (data[i] < 130) bitState += 1;

  }
  int stateSimilarities[] = {countBits(bitState ^ idl), countBits(bitState ^ mur), countBits(bitState ^ dnd)};
  
  int mostSimilarIndex = 0;
  for (int i = 1; i < 3; i++) {
    if (stateSimilarities[i] < stateSimilarities[mostSimilarIndex]) mostSimilarIndex = i;
  }

  if (state == mostSimilarIndex) return;

  state = mostSimilarIndex;

  Serial.printf("%s\n", state == 0 ? "IDL" : state == 1 ? "MUR" : "DND");

  sendData();
}

void ICACHE_RAM_ATTR interruptHandler() {
  noInterrupts();
  dataCounter = 0;

  while (digitalRead(D1) == 0) {}

  unsigned long started = micros() + 500000L;
  while (dataCounter < 33) {
    
    unsigned long timing = pulseIn(D1, LOW, 5000L);
    if (timing != 0) {
      data[dataCounter] = timing;
      dataCounter++;
    }

    if (started < micros()) {
      dataCounter = 0;
      interrupts();
      return;
    }
  }

  changeState();

  interrupts();
}

void setup() {
  // Begin Serial
  Serial.begin(9600);
  
  pinMode(D1, INPUT);
  attachInterrupt(D1, interruptHandler, FALLING);

  memset(data, sizeof(data), 0);

  // Wifi 
  WiFi.mode(WIFI_STA);
  wifiConnect();
  
  Serial.print("ESP8266 IP: ");
  Serial.println(WiFi.localIP());

  mqttConnect();

}

void loop() {

}

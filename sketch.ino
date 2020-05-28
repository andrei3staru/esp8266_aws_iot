/* ESP8266 AWS IoT example
*/

#include "FS.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// TODO - Update these with values suitable for your network.
const char *ssid = "SSID";
const char *password = "PASSWORD";
#define PUB_GPIO2_STATUS "esp8266/gpio2/status"
#define SUB_GPIO2_ACTION "esp8266/gpio2/action"
#define GPIO2_LED 2
String led_status = "ON";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

// TODO - Add AWS endpoint
const char *AWS_endpoint = "ENDPOINT"; //MQTT broker ip

void PubSubCallback(char *topic, byte *payload, unsigned int length)
{

  String strTopicGpio2Action = SUB_GPIO2_ACTION;
  String strPayload = "";
  String strON = "ON";
  String strOFF = "OFF";

  Serial.print("Topic:");
  Serial.println(topic);
  Serial.print("Message:");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
    strPayload += (char)payload[i];
  }
  Serial.println();
  Serial.println("-----------------------");

  if (strTopicGpio2Action == topic)
  {
    if (strON == strPayload)
    {
      digitalWrite(GPIO2_LED, LOW);
      //  client.publish(PUB_GPIO2_STATUS, "ON");
      led_status = "ON";
    }
    else if (strOFF == strPayload)
    {
      digitalWrite(GPIO2_LED, HIGH);
      //  client.publish(PUB_GPIO2_STATUS, "OFF");
      led_status = "OFF";
    }
  }
}

WiFiClientSecure espClient;
PubSubClient client(AWS_endpoint, 8883, PubSubCallback, espClient); //set  MQTT port number to 8883 as per //standard

long lastMsg = 0;
char msg[50];
int value = 0;

// Configure Wifi
void setup_wifi()
{

  delay(10);
  // We start by connecting to a WiFi network
  espClient.setBufferSizes(512, 512);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  timeClient.begin();
  while (!timeClient.update())
  {
    timeClient.forceUpdate();
  }

  espClient.setX509Time(timeClient.getEpochTime());
}

// Connect to Amazon MQTT and subscribe
void reconnect()
{
  Serial.println("In reconnect");
  client.setCallback(PubSubCallback);
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("Test1"))
    {
      Serial.println("connected");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");

      char buf[256];
      espClient.getLastSSLError(buf, 256);
      Serial.print("WiFiClientSecure SSL error: ");
      Serial.println(buf);

      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
  Serial.println("Reconnect - Publish and Subscribe");
  // Once connected, publish an announcement...
  client.publish(PUB_GPIO2_STATUS, "OFF");
  // ... and resubscribe
  client.subscribe(SUB_GPIO2_ACTION);
}

// Load Auth Certificates and call reconnect()
void setup()
{

  Serial.begin(115200);
  Serial.setDebugOutput(true);
  // GPIO2 is set OUTPUT
  pinMode(GPIO2_LED, OUTPUT);
  setup_wifi();
  delay(1000);
  if (!SPIFFS.begin())
  {
    Serial.println("Failed to mount file system");
    return;
  }

  Serial.print("Heap: ");
  Serial.println(ESP.getFreeHeap());

  Dir dir = SPIFFS.openDir("/");
  while (dir.next())
  {
    Serial.print(dir.fileName());
    File f = dir.openFile("r");
    Serial.println(f.size());
  }

  // TODO - Load certificate file
  File cert = SPIFFS.open("/foo-certificate.pem.crt", "r");
  if (!cert)
  {
    Serial.println("Failed to open cert file");
  }
  else
    Serial.println("Success to open cert file");

  delay(1000);

  if (espClient.loadCertificate(cert))
    Serial.println("cert loaded");
  else
    Serial.println("cert not loaded");

  // TODO - Load private key file
  File private_key = SPIFFS.open("/foo-private.pem.key", "r");
  if (!private_key)
  {
    Serial.println("Failed to open private cert file");
  }
  else
    Serial.println("Success to open private cert file");

  delay(1000);

  if (espClient.loadPrivateKey(private_key))
    Serial.println("private key loaded");
  else
    Serial.println("private key not loaded");

  // TODO - Load CA file
  File ca = SPIFFS.open("/AmazonRootCA1.pem", "r");
  if (!ca)
  {
    Serial.println("Failed to open ca ");
  }
  else
    Serial.println("Success to open ca");

  delay(1000);

  if (espClient.loadCACert(ca))
    Serial.println("ca loaded");
  else
    Serial.println("ca failed");

  Serial.print("Heap: ");
  Serial.println(ESP.getFreeHeap());

  reconnect();
}

void loop()
{
  //Serial.print("*");
  client.loop();

  //Serial.print(".");
  long now = millis();
  if (now - lastMsg > 30000)
  {
    lastMsg = now;
    ++value;
    snprintf(msg, 75, "OFF #%ld", value);
    Serial.print("Publish message: ");
    Serial.print(led_status);
    Serial.print(" to topic ");
    Serial.println(PUB_GPIO2_STATUS);
    client.publish(PUB_GPIO2_STATUS, (char *)led_status.c_str());
    Serial.print("Heap: ");
    Serial.println(ESP.getFreeHeap()); //Low heap can cause problems
  }
}

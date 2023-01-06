
#include <ESP8266WiFi.h>
#include "FS.h"

// You need to set certificates to All SSL cyphers and you may need to
// increase memory settings in Arduino/cores/esp8266/StackThunk.cpp:
//   https://github.com/esp8266/Arduino/issues/6811
#include "WiFiClientSecureBearSSL.h"
#include <time.h>

#include <MQTT.h>

#include <CloudIoTCore.h>
#include <CloudIoTCoreMqtt.h>
#include "ciotc_config.h" // Wifi configuration here


// !!REPLACEME!!
// The MQTT callback function for commands and configuration updates
// Place your message handler code here.
void messageReceivedAdvanced(MQTTClient *client, char topic[], char bytes[], int length)
{
  Serial.printf("Incoming Topic: %s", topic);
  if (length > 0)// On message
  {
    Serial.printf("\n\r   Data: %s", bytes);
  }
  Serial.println();
}
///////////////////////////////

// Initialize WiFi and MQTT for this board
static MQTTClient *mqttClient;
static BearSSL::WiFiClientSecure netClient;
static BearSSL::X509List certList;
static CloudIoTCoreDevice device(project_id, location, registry_id, device_id);
CloudIoTCoreMqtt *mqtt;

///////////////////////////////
// Helpers specific to this board
///////////////////////////////
String getTemperature()
{
  long sensorValue = random(10, 35); 
  char result[40];
  sprintf(result,"%u",sensorValue);

  return result;
}

String getHumidity()
{
  long sensorValue = random(10, 35); 
  char result[40];
  sprintf(result,"%u",sensorValue);

  return result;
}

String getJwt()
{
  // Disable software watchdog as these operations can take a while.
  ESP.wdtDisable();
  time_t iat = time(nullptr);
  String jwt = device.createJWT(iat, jwt_exp_secs);
  ESP.wdtEnable(0);
  Serial.println(jwt);
  return jwt;
}

static void setupCertAndPrivateKey()
{
  // Set CA cert on wifi client
  // If using a static (pem) cert, uncomment in ciotc_config.h:
  certList.append(primary_ca);
  certList.append(backup_ca);
  netClient.setTrustAnchors(&certList);
  //netClient.setInsecure();
  // netClient.setBufferSizes(1024, 1024);

  device.setPrivateKey(private_key);
}

static void setupWifi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("\n\r\n\rConnecting to WiFi");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
  }

  configTime(0, 0, ntp_primary, ntp_secondary);
  Serial.println("Waiting on time sync...");
  while (time(nullptr) < 1510644967)
  {
    delay(10);
  }
}

///////////////////////////////
// Orchestrates various methods from preceeding code.
///////////////////////////////

bool publishTelemetry(String subtopic,const String &data)
{
  // The result will be something like "/<device_id>/<subtopic_name>"
  String subtopic_full = "/";
  subtopic_full = subtopic_full + device_id;
  subtopic_full = subtopic_full + "/";
  subtopic_full = subtopic_full + subtopic;
  
  Serial.print("Outcoming: subtopic: ");
  Serial.print(subtopic_full);
  Serial.print(",data: ");
  Serial.println(data);

  return mqtt->publishTelemetry(subtopic_full,data);
}

// TODO: fix globals
void setupCloudIoT()
{
  // ESP8266 WiFi setup
  setupWifi();

  // ESP8266 WiFi secure initialization and device private key
  setupCertAndPrivateKey();

  mqttClient = new MQTTClient(512);
  mqttClient->setOptions(180, true, 1000); // keepAlive, cleanSession, timeout
  mqtt = new CloudIoTCoreMqtt(mqttClient, &netClient, &device);
  mqtt->setUseLts(true);
  mqtt->startMQTTAdvanced(); // Opens connection using advanced callback
}
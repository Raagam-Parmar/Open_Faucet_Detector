#include <ESP8266WiFi.h>

// Set WiFi credentials
#define WIFI_SSID "Raagam"
#define WIFI_PASS "50023082"

// Set IFTTT Webhooks event name and key
#define IFTTT_Key "cvUL7DlgrWMXicASaoPbKg"
#define IFTTT_Event "t1_high_flow"  // or whatever you have chosen
#define IFTTT_Value1 "Moin, hier ist dein ESP8266!"
#define IFTTT_Value2 "25"
#define IFTTT_Value3 "value 3"

int threshold_val = 100;
bool notified = false;
bool onTimeRec = false;
int on_time = 0;
int timer = 5000;
int on_duration;

WiFiClient client;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);


  Serial.begin(9600);  // Serial output only for information, you can also remove all Serial commands
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  // Connecting to WiFi...
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }

  // Connected to WiFi
  Serial.println();
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());

  // Send Webook to IFTTT
  // notify();
}

void loop() {

  int water_sensor = analogRead(A0);  //store the sensor value in a variable
  on_duration = millis() - on_time;

  if (water_sensor >= threshold_val) {

    digitalWrite(LED_BUILTIN, LOW);

    if (!onTimeRec) {
      on_time = millis();
      onTimeRec = true;
      on_duration = 0;
    }

    if (on_duration > timer) {

      if (!notified) {
        notify();
        notified = true;
      }
    }

  } else {

    digitalWrite(LED_BUILTIN, HIGH);
    onTimeRec = false;
    notified = false;
  }
  Serial.print(0);
  Serial.print(", ");
  Serial.print(float(on_duration) / 1000);
  Serial.print(", ");
  Serial.print(onTimeRec);
  Serial.print(", ");
  Serial.print(notified);
  Serial.print(", ");
  Serial.print(water_sensor);
  Serial.print(", ");
  Serial.println(1024);
  delay(10);
}

void notify() {
  // construct the JSON payload
  String jsonString = "";
  jsonString += "{\"value1\":\"";
  jsonString += IFTTT_Value1;
  jsonString += "\",\"value2\":\"";
  jsonString += IFTTT_Value2;
  jsonString += "\",\"value3\":\"";
  jsonString += IFTTT_Value3;
  jsonString += "\"}";
  int jsonLength = jsonString.length();
  String lenString = String(jsonLength);

  // connect to the Maker event server
  client.connect("maker.ifttt.com", 80);

  // construct the POST request
  String postString = "";
  postString += "POST /trigger/";
  postString += IFTTT_Event;
  postString += "/with/key/";
  postString += IFTTT_Key;
  postString += " HTTP/1.1\r\n";
  postString += "Host: maker.ifttt.com\r\n";
  postString += "Content-Type: application/json\r\n";
  postString += "Content-Length: ";
  postString += lenString + "\r\n";
  postString += "\r\n";
  postString += jsonString;  // combine post request and JSON

  client.print(postString);
  delay(500);
  client.stop();
}
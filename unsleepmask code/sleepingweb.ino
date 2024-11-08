#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include "time.h"

#include <ESPAsyncWebSrv.h>

AsyncWebServer server(80);

// REPLACE WITH YOUR NETWORK CREDENTIALS
const char* ssid = "Fulbright_Student1";
const char* password = "fulbright2018";

const char* PARAM_INPUT_1 = "input";

char timeHour[3];
char timeMinute[3];

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 25200;
const int   daylightOffset_sec = 0;

const int LED_PIN = 25;
const int LED_PIN2 = 26;
int speaker = 32;


const int WAITTIME = 50;
const int STEP = 5;

String inputMessage;
String inputParam;

bool lightable = false;

// HTML web page to handle 3 input fields (input1, input2, input3)
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>ESP Input Form</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <form action="/get">
    input: <input type="time" id="myTime" name="input">
    <input type="submit" value="Submit">
  </form><br>
 
  </form>
</body></html>)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void setup() {
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  pinMode( LED_PIN, OUTPUT );
  pinMode( LED_PIN2, OUTPUT ); 

  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed!");
    return;
  }
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  // //disconnect WiFi as it's no longer needed
  // WiFi.disconnect(true);
  // WiFi.mode(WIFI_OFF);

  // Send web page with input fields to client
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    lightable = false;
    request->send_P(200, "text/html", index_html);
  });

  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    
    // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1)) {
      lightable = true;
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
    } 
    Serial.println(inputMessage);
    request->send(200, "text/html", "HTTP GET request sent to your ESP on input field (" 
                                     + inputParam + ") with value: " + inputMessage +
                                     "<br><a href=\"/\">Return to Home Page</a>");
  });
  /*server.on("/wokeup", HTTP_GET, [] (AsyncWebServerRequest *request) {
    lightable = false;
    request->send(200, "text/html", "Good morning");
  });*/
  server.onNotFound(notFound);
  server.begin();
}

void loop(){
  delay(1000);
  printLocalTime();
}

void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  //Serial.println(&timeinfo, "%H:%M");
  strftime(timeHour,3, "%H", &timeinfo);
  strftime(timeMinute,3, "%M", &timeinfo);

  if ((timeHour[0] == inputMessage[0]) && (timeHour[1] == inputMessage[1]) && (timeMinute[0] == inputMessage[3]) && (timeMinute[1] == inputMessage[4])) {
    Serial.println(timeHour);
    Serial.println(timeMinute);
    // Serial.println("ddmdmdmdmdmdmd");

    int i;
    // gradually light on
    if (lightable) {
      i = 0;
      while ( i <= 255 ){
          analogWrite( LED_PIN, i );
          analogWrite( LED_PIN2, i );
          delay( WAITTIME );
          i = i + STEP;
      }

      while (lightable) {
        tone(speaker, 440, 300);
        delay(400);
        tone(speaker, 660, 300);
        delay(400);
        tone(speaker, 880, 300);
        delay(400);
      }
      analogWrite(LED_PIN, 0);
      analogWrite(LED_PIN2, 0);
    }
    
  }
}
  // gradually light off
    //   i = 255;
    //   while ( i >= 0 ){
    //       analogWrite( LED_PIN, i );
    //       analogWrite( LED_PIN2, i );
    //       delay( WAITTIME );
    //       i = i - STEP;
    //   }
    // }
    // digitalWrite(LED_PIN, HIGH); // turn the LED on
    // digitalWrite(LED_PIN2, HIGH); 

// }


    
// void loop() {
  
// }
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <FreeRTOS.h>
 
//#include <Adafruit_BME280.h>
//#include <Adafruit_Sensor.h>
//#include <Adafruit_NeoPixel.h>
 
const char *SSID = "HUAWEI Mate 20 lite";
const char *PWD = "190_b3R$";
  #define NUM_OF_LEDS 8 
  #define PIN 4
 
// Web server running on port 80
WebServer server(80);
 
//// Sensor
//Adafruit_BME280 bme;
//// Neopixel LEDs strip
//Adafruit_NeoPixel pixels(NUM_OF_LEDS, PIN, NEO_GRB + NEO_KHZ800);

int state = 0;

int post_attempts = 0;

//define motor pins
#define MOT1_IN1 36//IN1 of the L298 should be connected to this arduino pin
#define MOT1_IN2 39//IN2 of the L298 should be connected to this arduino pin
#define MOT1_EN 34 //enable
#define LED 2 //A led that blinks when receiving

//define motor movements
void motor_stop() {
  //Stop if received an wrong direction
    digitalWrite(MOT1_IN1, 0);
    digitalWrite(MOT1_IN2, 0);
    analogWrite(MOT1_EN, 0);
}

void motor_forward(unsigned int vel) {
    digitalWrite(MOT1_IN1, 1);
    digitalWrite(MOT1_IN2, 0);
    analogWrite(MOT1_EN, vel);
}


//define motor sequence
void sequenceRover()
  state = state + 1;
  

// JSON data buffer
StaticJsonDocument<250> jsonDocument;
char buffer[250];
 
// env variable
//float temperature;
//float humidity;
//float pressure;

char *alive;
 
void connectToWiFi() {
  Serial.print("Connecting to ");
  Serial.println(SSID);
  
  WiFi.begin(SSID, PWD);
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
    // we can even make the ESP32 to sleep
  }
 
  Serial.print("Connected. IP: ");
  Serial.println(WiFi.localIP());
}
void setup_routing() {     
//  server.on("/temperature", getTemperature);     
//  server.on("/pressure", getPressure);     
//  server.on("/humidity", getHumidity);     
//  server.on("/env", getEnv);     
//  server.on("/led", HTTP_POST, handlePost);    
  server.on("/ualive", getAlive);     
  server.on("/BCIprint", HTTP_POST, handlePost); 
  // start server    
  server.begin();    
}
 
void create_json(char *alive) {  
  jsonDocument.clear(); 
  jsonDocument["alive"] = alive;
  serializeJson(jsonDocument, buffer);
}
 
void add_json_object(char *alive) {
  JsonObject obj = jsonDocument.createNestedObject();
  obj["alive"] = alive; 
}
//void read_sensor_data(void * parameter) {
//   for (;;) {
//     temperature = bme.readTemperature();
//     humidity = bme.readHumidity();
//     pressure = bme.readPressure() / 100;
//     Serial.println("Read sensor data");
// 
//     // delay the task
//     vTaskDelay(60000 / portTICK_PERIOD_MS);
//   }
//}

void amIalive(void * parameter){
    alive = "yes";
    Serial.println("I AM ALIVE!");
    vTaskDelete(NULL);
    }

//void getTemperature() {
//  Serial.println("Get temperature");
//  create_json("temperature", temperature, "°C");
//  server.send(200, "application/json", buffer);
//}
// 
//void getHumidity() {
//  Serial.println("Get humidity");
//  create_json("humidity", humidity, "%");
//  server.send(200, "application/json", buffer);
//}
// 
//void getPressure() {
//  Serial.println("Get pressure");
//  create_json("pressure", pressure, "mBar");
//  server.send(200, "application/json", buffer);
//}
// 
//void getEnv() {
//  Serial.println("Get env");
//  jsonDocument.clear();
//  add_json_object("temperature", temperature, "°C");
//  add_json_object("humidity", humidity, "%");
//  add_json_object("pressure", pressure, "mBar");
//  serializeJson(jsonDocument, buffer);
//  server.send(200, "application/json", buffer);
//}

void getAlive() {
  Serial.println("GET ALIVE");
  create_json("alive");
  server.send(200, "application/json", buffer);
}

//void handlePost() {
//  if (server.hasArg("plain") == false) {
//    //handle error here
//  }
//  String body = server.arg("plain");
//  deserializeJson(jsonDocument, body);
//  
//  // Get RGB components
//  int red = jsonDocument["red"];
//  int green = jsonDocument["green"];
//  int blue = jsonDocument["blue"];
//  Serial.print("Red: ");
//  Serial.print(red);
//  
//  pixels.fill(pixels.Color(red, green, blue));
//  pixels.show();
//  // Respond to the client
//  server.send(200, "application/json", "{}");
//}

void handlePost() {
  if (server.hasArg("plain") == false) {
    //handle error here
  }
  String body = server.arg("plain");
  deserializeJson(jsonDocument, body);
  
  // Gcheck if BCI indicates movement
  int BCIprint = jsonDocument["BCIprint"];
  Serial.print("BCIprint: ");
  Serial.print(BCIprint);

  //write to motors
  post_attempts = post_attempts + 1;
  if(post_attempts == 10){
    //cambiar de estado
  }
  //
  delay(100);
  // Respond to the client
  server.send(200, "application/json", "{chi cheñol }");
}

//void setup_task() {    
//  xTaskCreate(     
//  read_sensor_data,      
//  "Read sensor data",      
//  1000,      
//  NULL,      
//  1,     
//  NULL     
//  );     
//}

void setup_task() {    
  xTaskCreate(     
  amIalive,      
  "amIalive",      
  1000,      
  NULL,      
  1,     
  NULL );     
}

void setup() {     
  Serial.begin(9600);    
     
  // Sensor setup    
//  if (!bme.begin(0x76)) {    
//    Serial.println("Problem connecting to BME280");    
//  }    
  connectToWiFi();     
  setup_task();    
  setup_routing();     
  // Initialize Neopixel     
//  pixels.begin();     
}    
       
void loop() {    
  server.handleClient();     
}

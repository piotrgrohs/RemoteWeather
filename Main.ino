#include <LiquidCrystal.h>
#include <WiFi.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; 
LiquidCrystal lcd(4,23,5,18,19,2);
int LED_BUILTIN=2;
IPAddress IP;
bool status;
String header;
int counter = 0;
unsigned long currentMillis = 0;
unsigned long previousMillis = 0; 

const char* ssid     = "NT";
const char* password = "";
WiFiServer server(80);


void setup()
{
    Serial.begin(9600);
    pinMode(15, INPUT_PULLUP);
    status = bme.begin(0x76);
    lcd.begin(16, 2);
    lcd.clear();
    lcd.print ("Welcome!");
    delay(2000);
    WiFi.begin(ssid, password);
    delay(2000);
    lcd.clear();
    server.begin();

}
 
void loop(){ 
  WiFiClient client = server.available(); 
  int button = digitalRead(15);
  lcd.clear();
  if (client) {                           
    String currentLine = "";                
    while (client.connected()) {          
      if (client.available()) {   
        char c = client.read();            
        Serial.write(c);                   
        header += c;
        if (c == '\n') {                   
          if (currentLine.length() == 0) {
 
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the table 
            client.println("<style>body { text-align: center; font-family: \"Trebuchet MS\", Arial;}");
            client.println("table { border-collapse: collapse; width:35%; margin-left:auto; margin-right:auto; }");
            client.println("th { padding: 12px; background-color: #0043af; color: white; }");
            client.println("tr { border: 1px solid #ddd; padding: 12px; }");
            client.println("tr:hover { background-color: #bcbcbc; }");
            client.println("td { border: none; padding: 12px; }");
            client.println(".sensor { color:white; font-weight: bold; background-color: #bcbcbc; padding: 1px; }");
            
            // Web Page Heading
            client.println("</style></head><body><h1>ESP32 with BME280</h1>");
            client.println("<table><tr><th>MEASUREMENT</th><th>VALUE</th></tr>");
            client.println("<tr><td>Temp. Celsius</td><td><span class=\"sensor\">");
            client.println(bme.readTemperature());
            client.println(" *C</span></td></tr>");  
            client.println("<tr><td>Temp. Fahrenheit</td><td><span class=\"sensor\">");
            client.println(1.8 * bme.readTemperature() + 32);
            client.println(" *F</span></td></tr>");       
            client.println("<tr><td>Pressure</td><td><span class=\"sensor\">");
            client.println(bme.readPressure() / 100.0F);
            client.println(" hPa</span></td></tr>");
            client.println("<tr><td>Approx. Altitude</td><td><span class=\"sensor\">");
            client.println(bme.readAltitude(SEALEVELPRESSURE_HPA));
            client.println(" m</span></td></tr>"); 
            client.println("<tr><td>Humidity</td><td><span class=\"sensor\">");
            client.println(bme.readHumidity());
            client.println(" %</span></td></tr>"); 
            client.println("</body></html>"); 
            client.println();
       
            break;
          } else { 
            currentLine = "";
          }
        } else if (c != '\r') {  
          currentLine += c;      
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
  }
  if (!status) {
    status = bme.begin(0x76);
    lcd.print("Sensor off");
    delay(500);

  }else{
    if(button==LOW){
      counter++;
      if(counter>=5){
        counter = 0;
      }
    }
    lcd.clear();
    prints(counter);
    delay(2000);
  }
}

void prints(int val){
  switch(val){
    case 0:
     printTemp();
     break;
    case 1:
     printPressure();
     break;
    case 2:
     printAltitude();
     break;
    case 3:
     printHumidity();
     break;
    case 4:
     printIP();
     break;
  }
}
void printIP(){
  lcd.setCursor(0,0);
  lcd.print("IP Address:");
  lcd.setCursor(0,1); 
  if(WiFi.status() == WL_CONNECTED){
    IP = WiFi.localIP();
    lcd.print(IP);
  }else{
    String message = "Not connected";
    lcd.print(message);
  }

  
}
void printTemp(){
    lcd.setCursor(0,0);
    lcd.print("Temperature:");
    lcd.setCursor(0,1);
    String temperature = String(bme.readTemperature()) + " *C";
    lcd.print(temperature);
}
void printPressure(){
    lcd.setCursor(0,0);
    lcd.print("Pressure:");
    lcd.setCursor(0,1);
    String pressure = String(bme.readPressure() / 100.0F) + " hPa";
    lcd.print(pressure);
}
void printAltitude(){
    lcd.setCursor(0,0);
    lcd.print("Altitude:");
    lcd.setCursor(0,1);
    String sealevel = String(bme.readAltitude(SEALEVELPRESSURE_HPA)) + " m";
    lcd.print(sealevel);
}
void printHumidity(){
    lcd.setCursor(0,0);
    lcd.print("Humidity:");
    lcd.setCursor(0,1);
    String humidity = String(bme.readHumidity()) + " %";
    lcd.print(humidity);
}

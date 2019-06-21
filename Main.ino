#include <LiquidCrystal.h>
#include <WiFi.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C 22 na 4 i 21 na 2
LiquidCrystal lcd(4,23,5,18,19,2);
int LED_BUILTIN=2;

bool status;
String header;
int counter = 0;

const char* ssid     = "ESP32-Access-Point";
const char* password = "123456789";
WiFiServer server(80);


void setup()
{
    pinMode(15, INPUT_PULLUP);
    status = bme.begin(0x76);
    lcd.begin(16, 2);
    lcd.clear();
    lcd.print ("Welcome!");
    delay(2000);
    lcd.setCursor(0,1);
    lcd.print("Setting AP");
    WiFi.softAP(ssid, password);
    IPAddress IP = WiFi.softAPIP();
    lcd.print(String(IP));
    delay(5000);
    lcd.clear();

}
 
void loop(){ 
  WiFiClient client = server.available(); 
  int button = digitalRead(15);
  lcd.clear();
  if (client) {                             // If a new client connects,
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
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
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
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
      if(counter>=4){
        counter = 0;
      }
    }
    lcd.clear();
    prints(counter);
    delay(1000);
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

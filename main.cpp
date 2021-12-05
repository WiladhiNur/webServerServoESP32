#include <Arduino.h>
#include <WiFi.h>
#include <Servo.h>

// Set name of ssid for ESP32-AP
const char* ssid = "MyESP32-AP";

// Set web server port number to 80
WiFiServer server(80);

// Set ip static
IPAddress local_IP(192,168,4,22);
IPAddress gateway(192,168,4,9);
IPAddress subnet(255,255,255,0);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String degree = "0";

// Assign output variables to GPIO pins
static const int servoPin = 4;

// Set funcion servo
Servo Myservo;

void setup() {
  Serial.begin(115200);
  // Initialize the output servo
  Myservo.attach(servoPin);
  // Set servo to 0 degree
  Myservo.write(0);

  // Connect to Wi-Fi network with SSID and password
  Serial.println(" ");
  Serial.print("Setting soft-AP configuration ... ");
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");
  Serial.print("Setting soft-AP ... ");
  Serial.println(WiFi.softAP(ssid) ? "Ready" : "Failed!");
  Serial.print("Soft-AP IP address = ");
  Serial.println(WiFi.softAPIP());
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
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
            
            // move the servo
            if (header.indexOf("GET /deg/0") >= 0) {
              Serial.println("servo move to 0 deg");
              degree = "0";
              Myservo.write(0);
            }
            if (header.indexOf("GET /deg/90") >= 0) {
              Serial.println("servo move to 90 deg");
              degree = "90";
              Myservo.write(90);
            }
            if (header.indexOf("GET /deg/180") >= 0) {
              Serial.println("servo move to 180 deg");
              degree = "180";
              Myservo.write(180);
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #0FA334; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #808080;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>My ESP32 Web Server</h1>");
            client.println("<p>Servo poss " + degree +" degrees</p>");
            // Display current state
            if (degree=="0") {
              client.println("<p><a href=\"/deg/0\"><button class=\"button\">0 deg</button></a></p>");
              client.println("<p><a href=\"/deg/90\"><button class=\"button button2\">90 deg</button></a></p>");
              client.println("<p><a href=\"/deg/180\"><button class=\"button button2\">180 deg</button></a></p>");
            }
            if (degree=="90") {
              client.println("<p><a href=\"/deg/0\"><button class=\"button button2\">0 deg</button></a></p>");
              client.println("<p><a href=\"/deg/90\"><button class=\"button\">90 deg</button></a></p>");
              client.println("<p><a href=\"/deg/180\"><button class=\"button button2\">180 deg</button></a></p>");
            } 
            if (degree=="180") {
              client.println("<p><a href=\"/deg/0\"><button class=\"button button2\">0 deg</button></a></p>");
              client.println("<p><a href=\"/deg/90\"><button class=\"button button2\">90 deg</button></a></p>");
              client.println("<p><a href=\"/deg/180\"><button class=\"button\">180 deg</button></a></p>");
            }
            client.println("<p>Embedded D4-3F</p>");
            client.println("<p>Wiladhi Nur Pratikto</p>");
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
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

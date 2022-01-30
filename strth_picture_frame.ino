#include <SPI.h>
#include <WiFiNINA.h>
#include "arduino_secrets.h" 
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                 // your network key index number (needed only for WEP)

int status = WL_IDLE_STATUS;
char hostname[] = "abc";
WiFiServer server(80);

#include <NTPClient.h>
#include <WiFiUdp.h>
char ntpserver[] = "0.europe.pool.ntp.org";
int timeoffset = 0; //seconds
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpserver, timeoffset);

// Variable to hold time ticks for triggerung the display of time
unsigned long tick;
unsigned int clock_intrvl = 5; // minutes

const String numbers_min[60] = {"null", "eins", "zwei", "drei", "vier", "fuenf", "sechs",
  "sieben", "acht", "neun", "zehn", "elf", "zwoelf", "dreizehn", "vierzehn", "fuenfzehn",
  "sechzehn", "siebzehn", "achtzehn", "neunzehn", "zwanzig", "einundzwanzig",
  "zweiundzwanzig", "dreiundzwanzig", "vierundzwanzig", "fuenfundzwanzig", "sechsundzwanzig",
  "siebenundzwanzig", "achtundzwanzig", "neunundzwanzig", "dreissig", "einunddreissig",
  "zweiunddreissig", "dreiunddreissig", "vierunddreissig", "fuenfunddreissig",
  "sechsunddreissig", "siebenunddreissig", "achtunddreissig", "neununddreissig", "vierzig",
  "einundvierzig", "zweiundvierzig", "dreiundvierzig", "vierundvierzig", "fuenfundvierzig",
  "sechsundvierzig", "siebenundvierzig", "achtundvierzig", "neunundvierzig", "fuenfzig",
  "einundfuenfzig", "zweiundfuenfzig", "dreiundfuenfzig", "vierundfuenfzig",
  "fuenfundfuenzig", "sechsundfuenfzig", "siebenundfuenfzig", "achtundfuenzig",
  "neunundfuenfzig"
};
const String numbers_hrs[25] = {"null", "ein", "zwei", "drei", "vier", "fuenf", "sechs",
  "sieben", "acht", "neun", "zehn", "elf", "zwoelf", "dreizehn", "vierzehn", "fuenfzehn",
  "sechzehn", "siebzehn", "achtzehn", "neunzehn", "zwanzig", "einundzwanzig",
  "zweiundzwanzig", "dreiundzwanzig", "vierundzwanzig"
};

#include <Adafruit_NeoPixel.h>

// Which pin on the Arduino is connected to the NeoPixels?
// See: https://content.arduino.cc/assets/Pinout_NanoRP2040_latest.png
#define PIN 17 // GPIO17 (D5)

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 27

// Define brightness levels for the LEDs (0...254)
// LED_BRIGHTN_STD is standard/day time brightness
// LED_BRIGHTN_RED is reduced brightness during night time (22:00 ... 06:00 local time)
#define LED_BRIGHTN_STD 254
#define LED_BRIGHTN_RED 10
int red_brightn_night = 1;

// Create instance for the NeoPixel strip
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// Define colors for later use
const uint32_t yellow = pixels.ColorHSV(10922, 255, 255);
const uint32_t blue   = pixels.ColorHSV(43691, 255, 255);
const uint32_t red    = pixels.ColorHSV(    0, 255, 255);
const uint32_t green  = pixels.ColorHSV(21845, 255, 255);
const uint32_t black  = pixels.ColorHSV(    0,   0,   0);

// Define the individual colors for the letters
const uint32_t colors[] = {
  yellow,   // Status
  yellow,   // A
  blue,     // B
  red,      // C
  green,    // ...
  yellow,
  yellow,
  red,
  blue,
  blue,
  red,
  blue,
  green,
  yellow,
  red,
  red,
  green,
  red,
  green,
  yellow,
  yellow,
  blue,
  red,
  blue,
  yellow,
  red,
  red       // Z
};

#define DELAYVAL 500 // Time (in milliseconds) to pause between pixels

// Button to show time
#define BUTTON 4 // GPIO16 (D4)

#include "scan_request.h"
#include "sanitize_msg.h"
#include "int_memory.h"

void setup() {
  
  Serial.begin(9600);      // initialize serial communication

  Serial.println("Stranger Things Picture Frame\n");

  // Set the initial brightness of the LEDs
  pixels.setBrightness(LED_BRIGHTN_STD);

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  } else {
    Serial.println("WiFi module detected.");
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  } else {
    Serial.println("WiFi firmware version check passed.");
  }

  // check for the presence of the shield:
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  } else {
    Serial.println("WiFi shield detected.");
  }

// Restore settings from the NINA internal memory partition if present
//  if (file_exists("clock_intrvl")) {
//    clock_intrvl = file_read("clock_intrvl").toInt();
//    Serial.print("Restored setting for clock_intrvl from int. memory: ");
//    Serial.println(clock_intrvl);
//  }
//  if (file_exists("ntpserver")) {
//    String ntpserver_str = file_read("ntpserver");
//    ntpserver_str.toCharArray(ntpserver, ntpserver_str.length() + 1);
//  }
//  if (file_exists("timeoffset")) {
//    timeoffset = file_read("timeoffset").toInt();
//  }
//  if (file_exists("red_brightn_night")) {
//    red_brightn_night = file_read("red_brightn_night").toInt();
//  }
  
  // attempt to connect to WiFi network:
  WiFi.setHostname(hostname);               // Set hostname
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);                   // print the network name (SSID);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    tick = millis();
    while((millis() - tick) < 9000) {
      // Flash status led (0)
      pixels.setPixelColor(0, red);
      pixels.show();
      delay(200);
      pixels.setPixelColor(0, black);
      pixels.show();
      delay(200);
    }
    // Finally show green led
    pixels.setPixelColor(0, green);
    pixels.show();
    delay(1000);
    pixels.setPixelColor(0, black);
    pixels.show();
    
  }
  
  server.begin();                           // start the web server on port 80
  printWifiStatus();                        // you're connected now, so print out the status

  timeClient.begin();

  // Button to show the time
  pinMode(BUTTON, INPUT);

  // Initialize the variable for triggering the output of the time
  tick = millis();
}

void loop() {
  // Define some variables
  String currentLine = "";                  // make a String to hold incoming data from the client
  char c;                                   // read the request from the client byte-by-byte
  String retval = "";                       // Variable to temporarily hold the return value from
                                            // the function scan_request().
  String raw_msg;                           // Raw message from the HTTP request
  String clean_msg;                         // Message string cleaned to the characters that can be
                                            // displayed on the picture frame.
  
  // Check if it is time to show the time or the key is pressed
  unsigned long clock_intrvl_millis = 60000 * (unsigned long) clock_intrvl;
  if(((millis() - tick) > clock_intrvl_millis) || HIGH == digitalRead(BUTTON)) {
    // Update tick
    tick = millis();
    timeClient.update();
    Serial.println(timeClient.getFormattedTime());
    String now = time_text(timeClient.getHours(),
      timeClient.getMinutes(),
      timeClient.getSeconds());
    Serial.println(now);
    display_msg(now);
  }
  
  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        c = client.read();             // read a byte, then
        //Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.println("<!DOCTYPE HTML>");
            client.println("<html><head><title>Stranger Things ABC</title></head><body>");
            client.println("<h1><a href=\"/\">Stranger Things ABC</a></h1>");
            client.println("<p><form action=\"/GET\">Message: <input type=\"text\" name=\"msg\">&nbsp;<input type=\"submit\" value=\"Submit\"></form></p>");
            
            client.println("<h3>Settings</h3>");
            client.print("<table><tr><td>Current time:</td><td>");
            timeClient.update();
            client.print(timeClient.getFormattedTime());
            client.println("</td></tr>");
            client.println("<form action=\"/GET\">");
            client.println("<tr><td>Clock update interval:</td><td><select name=\"intrvl\">");
            for(unsigned int intrvl = 1; intrvl < 16; intrvl++) {
              client.print("<option value=\"");
              client.print(intrvl);
              client.print("\"");
              if(intrvl == clock_intrvl) {
                client.print(" selected=\"selected\"");
              }
              client.print(">");
              client.print(intrvl);
              client.println(" minutes</option>");
            }
            client.println("<option value=\"0\">off</option>");
            client.println("</select></td></tr>");   
            client.print("<tr><td>NTP server:</td><td><input type=\"text\" value=\"");
            client.print(ntpserver);
            client.println("\" name=\"ntpsrv\"></td></tr>");
            client.println("<tr><td>Time zone:</td><td><select name=\"tz\">");
            for(int tz = -12; tz <= 12; tz++) {
              client.print("<option value=\"");
              client.print(tz);
              client.print("\"");
              if(tz == (int)(timeoffset / 3600.)) {
                client.print(" selected=\"selected\"");
              }
              client.print(">UTC");
              if(tz>=0){
                client.print("+");
              }
              client.print(tz);
              client.println("h</option>");
            }
            client.println("</select></td></tr>");
            client.println("<tr><td>Reduce LED brightness at night:</td><td><input type=\"radio\" name=\"night\" value=\"1\"");
            if (1 == red_brightn_night) {
              client.println("checked ");
            }
            client.println("><label for=\"night\">Yes</label>");
            client.println(", <input type=\"radio\" name=\"night\" value=\"0\"");
            if (0 == red_brightn_night) {
              client.println("checked ");
            }
            client.println("><label for=\"night\">No</label>");
            client.println("</td></tr></table>");
            client.println("<input type=\"submit\" value=\"Update settings\"></form></p>");
            client.print("<p>Connected to: ");
            client.print(WiFi.SSID());
            client.print(" (");
            client.print(WiFi.RSSI());
            client.print(" dBm)<br>Hostname: ");
            client.print(hostname);
            client.print(", IP: ");
            client.print(WiFi.localIP());
            client.println("</p>");
            client.println("</body></html>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // If the request ist complete, i.e. ends with "HTTP/1.1" then scan the request
        if (currentLine.endsWith("HTTP/1.1")) {
          // Time interval for displaying the time on the picture frame in minutes
          retval = scan_request(currentLine, "intrvl");
          if (retval != "") {
            clock_intrvl = retval.toInt();
            // Safe setting to NINA internal memory
            //file_write("clock_intrvl", retval);
            retval = "";
          }

          // NTP server to receive time information from
          retval = scan_request(currentLine, "ntpsrv");
          if (retval != "") {
            retval.toCharArray(ntpserver, retval.length() + 1);
            // Safe setting to NINA internal memory
            //file_write("ntpserver", retval);
            retval = "";
          }

          // Local time zone offset wrt. UTC in hours
          retval = scan_request(currentLine, "tz");
          if (retval != "") {
            timeoffset = retval.toInt();
            timeoffset *= 3600; // Convert to seconds
            // Apply new time offset
            timeClient.setTimeOffset(timeoffset);
            // Safe setting to NINA internal memory
            //file_write("timeoffset", String(timeoffset));
            retval = "";
          }

          // Setting to reduce LED brightness during night time (22:00 - 06:00)
          retval = scan_request(currentLine, "night");
          if (retval != "") {
            red_brightn_night = retval.toInt();
            // Safe setting to NINA internal memory
            //file_write("red_brightn_night", retval);
            retval = "";
          }

          // See if we have a message to display and retrieve the raw message
          raw_msg = scan_request(currentLine, "msg");
          if (raw_msg != "") {
            // Clean the message
            clean_msg = sanitize_msg(raw_msg);
            // Display the message on the picture frame
            display_msg(clean_msg);
          }

          // Now, the HTTP request is successfully processed, therefore we can
          // clear it
          currentLine = "";

        }
      }
    }
    
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}

void display_msg(String msg) {
  // Convert to upper case
  msg.toUpperCase();

  // Reduction of led brightness between 22:00 and 06:00 local time
  if(1 == red_brightn_night && (timeClient.getHours() > 21 || timeClient.getHours() < 6) ) {
    pixels.setBrightness(LED_BRIGHTN_RED);
  } else {
    pixels.setBrightness(LED_BRIGHTN_STD);
  }
  
  for(int i=0; i < msg.length(); i++) {
    int ascii = msg[i]; // Get ASCII code of character
    if(ascii >= 65 && ascii <= 90) {
      // A ... Z (ASCII 65 ... 90)
      // The first LED (led 0) is status, the second led (led 1) is for letter A
      int led = ascii - 64;
      //pixels.setPixelColor(led, pixels.Color(0, 150, 0));
      pixels.setPixelColor(led, colors[led]);
      pixels.show();   // Send the updated pixel colors to the hardware.
      delay(DELAYVAL); // Pause before next pass through loop
      // back to black
      pixels.setPixelColor(led, pixels.Color(0, 0, 0));
      pixels.show();   // Send the updated pixel colors to the hardware.
      delay(DELAYVAL); // Pause before next pass through loop
    } else if (43 == ascii) {
      // Blank character (plus sign in HTTP request)
      delay(2 * DELAYVAL);
    }
  }
  
  // Always end a message with a pause
  int last = msg[msg.length() - 1];
  if(43 != last ) {
    delay(2 * DELAYVAL);
  }
}

String time_text(int hours, int minutes, int seconds) {
  String retval = "ES+IST+";
  if(0 == minutes) {
    retval += (String(numbers_hrs[hours]) + "+UHR");
  } else {
    retval += (String(numbers_hrs[hours]) + "+UHR+" + String(numbers_min[minutes]));
  }
  retval.toUpperCase();
  return retval;
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}

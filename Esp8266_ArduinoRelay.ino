/* Create a WiFi access point and provide a web server on it. 
Author: Lucio A. Rocha
Last Update: 21 Aug 2016
*/
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <EEPROM.h>

/* Set these to your desired credentials. */
const char* ssid = "Aeitware Cloud";
const char* passphrase = "aeitwarecloud8";
const char* password = "aeitwarecloud8";

String st;
String content;
int statusCode;
int status = WL_IDLE_STATUS;  
ESP8266WebServer server(80);
const int led = (2, OUTPUT); //GPIO2
/* Just a little test message. Go to http://192.168.4.1 in a web browser
* connected to this access point to see it.
*/
void handleRoot() {
int size=1000;
char temp[size];
int sec = millis() / 1000;
int min = sec / 60;
int hr = min / 60;
snprintf ( temp, size,
"<html>\
<head>\ 
<title>Nithinvv</title>\
<style>\
body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
</style>\
</head>\
<body>\
<h3>Welcome ,vvnithin03@gmail.com!</h3>\
<h3>www.aeitware.com</h3>\
<p>Nithinvv: %02d:%02d:%02d</p>\ 
<p>Current Status Of Your Aeitware Smart Device:Device ON</h1></p>\
<p><a href=\"http://192.168.4.1/on\"><h1>Turn ON</h1></a></p>\
<p><a href=\"http://192.168.43.226/on\"><h1>Turn hotspot ON</h1></a></p>\
<p></p>\
<p><a href=\"http://192.168.4.1/off\"><h1>Turn OFF</h1></a></p>\ 
<p><a href=\"http://192.168.43.226/off\"><h1>Turn hotspot OFF</h1></a></p>\
</body>\
</html>",
hr, min % 60, sec % 60
);
server.send ( 200, "text/html", temp ); 
}
void setup() {
  Serial.begin(9600);
  EEPROM.begin(512);
delay(100);
Serial.begin(9600);
Serial.println();
Serial.print("Configuring access point...");
/* You can remove the password parameter if you want the AP to be open. */
WiFi.softAP(ssid, password);
IPAddress myIP = WiFi.softAPIP();
Serial.print("AP IP address: ");
Serial.println(myIP);
pinMode(2, OUTPUT);
digitalWrite ( led, HIGH );

Serial.println();
  Serial.println();
  Serial.println("Startup");
  // read eeprom for ssid and pass
  Serial.println("Reading EEPROM ssid");
  String esid;
  for (int i = 0; i < 32; ++i)
    {
      esid += char(EEPROM.read(i));
    }
  Serial.print("SSID: ");
  Serial.println(esid);
  Serial.println("Reading EEPROM pass");
  String epass = "";
  for (int i = 32; i < 96; ++i)
    {
      epass += char(EEPROM.read(i));
    }
  Serial.print("PASS: ");
  Serial.println(epass);  
  if ( esid.length() > 1 ) {
      WiFi.begin(esid.c_str(), epass.c_str());
      if (testWifi()) {
        launchWeb(0);
        return;
      } 
  }
  setupAP();
}

bool testWifi(void) {
  int c = 0;
  Serial.println("Waiting for Wifi to connect");  
  while ( c < 20 ) {
    if (WiFi.status() == WL_CONNECTED) { return true; } 
    delay(500);
    Serial.print(WiFi.status());    
    c++;
  }
  Serial.println("");
  Serial.println("Connect timed out, opening AP");
  return false;
} 

void launchWeb(int webtype) {
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("SoftAP IP: ");
  Serial.println(WiFi.softAPIP());
  createWebServer(webtype);
  // Start the server
  server.begin();
  Serial.println("Server started"); 
}

void setupAP(void) {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
    Serial.println("no networks found");
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i)
     {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*");
      delay(10);
     }
  }
  Serial.println(""); 
  st = "<ol>";
  for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      st += "<li>";
      st += WiFi.SSID(i);
      st += " (";
      st += WiFi.RSSI(i);
      st += ")";
      st += (WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*";
      st += "</li>";
    }
  st += "</ol>";
  delay(100);
  WiFi.softAP(ssid, passphrase, 6);
  Serial.println("softap");
  launchWeb(1);
  Serial.println("over");
}

void createWebServer(int webtype)
{
  if ( webtype == 1 ) {
    server.on("/", []() {
        IPAddress ip = WiFi.softAPIP();
        String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
        content = "<!DOCTYPE HTML>\r\n<html>Enter your wifi router USSID and Password,Our device will automatically connect to your router ";
        content += ipStr;
        content += "<p>";
        content += st;
        content += "</p><form method='get' action='setting'><label>SSID: </label><input name='ssid' length=32><input name='pass' length=64><input type='submit'></form>";
        content += "</html>";
        server.send(200, "text/html", content);  
    });
    server.on("/setting", []() {
        String qsid = server.arg("ssid");
        String qpass = server.arg("pass");
        if (qsid.length() > 0 && qpass.length() > 0) {
          Serial.println("clearing eeprom");
          for (int i = 0; i < 96; ++i) { EEPROM.write(i, 0); }
          Serial.println(qsid);
          Serial.println("");
          Serial.println(qpass);
          Serial.println("");
            
          Serial.println("writing eeprom ssid:");
          for (int i = 0; i < qsid.length(); ++i)
            {
              EEPROM.write(i, qsid[i]);
              Serial.print("Wrote: ");
              Serial.println(qsid[i]); 
            }
          Serial.println("writing eeprom pass:"); 
          for (int i = 0; i < qpass.length(); ++i)
            {
              EEPROM.write(32+i, qpass[i]);
              Serial.print("Wrote: ");
              Serial.println(qpass[i]); 
            }    
          EEPROM.commit();
          content = "{\"Success\":\"saved to your Aeitware Smart device. Restart your Smart device now\"}";
          statusCode = 200;
        } else {
          content = "{\"Error\":\"We are sorry\"}";
          statusCode = 404;
          Serial.println("Sending 404");
        }
        server.send(statusCode, "application/json", content);
    });
  } else if (webtype == 0) {
    server.on("/", []() {
      IPAddress ip = WiFi.localIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
      server.send(200, "application/json", "{\"IP\":\"" + ipStr + "\"}");
    });
    server.on("/cleareeprom", []() {
      content = "<!DOCTYPE HTML>\r\n<html>";
      content += "<p>Clearing the EEPROM</p></html>";
      server.send(200, "text/html", content);
      Serial.println("clearing eeprom");
      for (int i = 0; i < 96; ++i) { EEPROM.write(i, 0); }
      EEPROM.commit();
    });
  }




//URLs available to query
server.on("/", handleRoot);
server.on ( "/on", turnON ); 
server.on ( "/off", turnOFF );
server.on ( "/love", loveOFF );
server.begin();
Serial.println("HTTP server started");
}
void loveOFF(){
Serial.print("love off");


}
void turnON(){
digitalWrite ( 2, HIGH );
Serial.print("relayon"); //this will print relayon to the arduino serial monitor
int size=1000;
char temp[size];
int sec = millis() / 1000;
int min = sec / 60;
int hr = min / 60;
snprintf ( temp, size,
"<html>\
<head>\ 
<title>MicroCloud</title>\
<style>\
body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
</style>\
</head>\
<body>\
<h3>Request: Device ON</h3>\
<p>Nithinvv: %02d:%02d:%02d</p>\
<p></p>\
<p>Current Status Of Your Aeitware Smart Device: Device ON</h1></p>\
<p><a href=\"http://192.168.4.1/on\"><h1>Turn ON</h1></a></p>\
<p><a href=\"http://192.168.43.226/on\"><h1>Turn hotspot ON</h1></a></p>\
<p></p>\
<p><a href=\"http://192.168.4.1/off\"><h1>Turn OFF</h1></a></p>\
<p><a href=\"http://192.168.43.226/off\"><h1>Turn hotspot OFF</h1></a></p>\ 
</body>\
</html>",
 hr, min % 60, sec % 60
);
server.send ( 200, "text/html", temp);
}
void turnOFF(){
digitalWrite ( 2, LOW );
Serial.print("relayoff"); //this will print relayoff to the arduino serial monitor
int size=1000;
char temp[size];
int sec = millis() / 1000;
int min = sec / 60;
int hr = min / 60;
snprintf ( temp, size,
"<html>\
<head>\ 
<title>MicroCloud</title>\
<style>\
body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
</style>\
</head>\
<body>\
<h3>Request: Device OFF </h3>\
<p>Nithinvv: %02d:%02d:%02d</p>\
<p>Current Status Of Your Aeitware Smart Device: Device OFF</p>\
<p><a href=\"http://192.168.4.1/on\"><h1>Turn ON</h1></a></p>\
<p><a href=\"http://192.168.43.226/on\"><h1>Turn hotspot ON</h1></a></p>\
<p></p>\
<p><a href=\"http://192.168.4.1/off\"><h1>Turn OFF</h1></a></p>\ 
<p><a href=\"http://192.168.43.226/off\"><h1>Turn hotspot OFF</h1></a></p>\
</body>\
</html>",
hr, min % 60, sec % 60
);
server.send ( 200, "text/html", temp);
}
void loop() {
server.handleClient();
 delay(300);
 Serial.println(WiFi.status());

}

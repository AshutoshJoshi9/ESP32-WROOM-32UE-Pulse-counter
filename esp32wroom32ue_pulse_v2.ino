#include <WiFi.h>
#include <WebServer.h>

//Access Point 
const char *ssid = "ESP32-AP";
const char *password = "12345678";

WebServer server(80);

//ADC pin 
const int analogPin = 34;
const int threshold = 2000;  //adjust based on your signal

//variables for pulse counting
int pulseCount = 0;
int previousValue = 0;
int rotations = pulseCount / 12.0;

//HTML
const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Pulse Count</title>
  <script>
    function fetchPulseCount() {
      fetch("/pulsecount").then(response => response.text()).then((count) => {
        document.getElementById("pulseCount").innerText = count;
      });
    }
    setInterval(fetchPulseCount, 1000);
  </script>
</head>
<body>
  <h1>Pulse Count</h1>
  <p>Pulse Count: <span id="pulseCount">0</span></p>
  
</body>
</html>
)rawliteral";


void handleRoot() {
  server.send(200, "text/html", htmlPage);
}

void handlePulseCount() {
  server.send(200, "text/plain", String(pulseCount));
}

void setup() {
  Serial.begin(115200);

  //access point setup
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  //web server
  server.on("/", handleRoot);
  server.on("/pulsecount", handlePulseCount);
  server.begin();
}

void loop() {
  //analog value
  int currentValue = analogRead(analogPin);

  //detect rising edge
  if (currentValue > threshold && previousValue <= threshold) {
    pulseCount++;
  }
  previousValue = currentValue;

  //web server client handling
  server.handleClient();
  Serial.println(pulseCount-1);
  //delay
  delay(10);  //adjust based on signal frequency
}

#include <WiFi.h>
#include <WiFiClient.h>

const char* ssid = "Stephen";          // Your Wi-Fi SSID
const char* password = "emman2702";    // Your Wi-Fi Password
const char* server = "192.168.29.230"; // Flask server's local IP address
const int port = 5001;                 // Flask server port

void setup() {
  Serial.begin(115200);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  // Create a WiFi client
  WiFiClient client;

  // Connect to the server
  if (client.connect(server, port)) {
    Serial.println("Connected to server");

    // Prepare JSON data
    String jsonData = "{\"temperature\": 25, \"humidity\": 60}";

    // Send HTTP POST request to the /update endpoint
    client.println("POST /update HTTP/1.1");
    client.println("Host: " + String(server) + ":" + String(port));
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(jsonData.length());
    client.println();  // End of header
    client.println(jsonData);

    // Wait for server response
    while (client.available() == 0) {
      delay(100);
    }

    // Read and print the response
    while (client.available()) {
      String line = client.readStringUntil('\n');
      Serial.println(line);
    }
    client.stop();
  } else {
    Serial.println("Connection to server failed");
  }

  // Wait before the next loop
  delay(5000); // Send data every 5 seconds
}

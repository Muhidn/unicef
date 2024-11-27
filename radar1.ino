#include <WiFi.h>
#include "ThingSpeak.h"
#include <ESP32Servo.h>

// WiFi credentials
const char* ssid = "WPA2-Personal";        // Replace with your WiFi SSID
const char* password = "ICU22g@cu"; // Replace with your WiFi password

// ThingSpeak settings
const char* server = "api.thingspeak.com";
unsigned long channelID = 2748463;   // Replace with your Channel ID
const char* writeAPIKey = "EUDPCZOCXAPI6S5D"; // Replace with your Write API Key

WiFiClient client;

// Servo
Servo myServo;
const int servoPin = 14;

// Ultrasonic sensor pins
const int trigPin = 5;
const int echoPin = 18;

// Variables for ThingSpeak
long distance = 0;

// Variables for servo sweep
int currentAngle = 0;
bool forward = true;

void setup() {
  Serial.begin(115200);

  // Initialize servo
  myServo.attach(servoPin);

  // Initialize ultrasonic sensor pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Connect to WiFi
  connectToWiFi();

  // Initialize ThingSpeak
  ThingSpeak.begin(client);

  Serial.println("Setup complete.");
}

void loop() {
  // Measure distance
  distance = getDistance();

  // Send data to ThingSpeak
  sendToThingSpeak(distance);

  // Sweep the servo
  sweepServo();

  delay(1000); // Send data every second
}

long getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  long distance = duration * 0.034 / 2; // Distance in cm
  return distance;
}

void sweepServo() {
  if (forward) {
    currentAngle += 2;
    if (currentAngle >= 180) {
      forward = false;
    }
  } else {
    currentAngle -= 2;
    if (currentAngle <= 0) {
      forward = true;
    }
  }
  myServo.write(currentAngle);
}

void connectToWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void sendToThingSpeak(long distance) {
  ThingSpeak.setField(1, distance); // Set the first field to the distance value

  int response = ThingSpeak.writeFields(channelID, writeAPIKey);

  if (response == 200) {
    Serial.println("Data sent to ThingSpeak successfully.");
  } else {
    Serial.print("Failed to send data. HTTP error code: ");
    Serial.println(response);
  }
}

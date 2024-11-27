#include <ESP32Servo.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <ThingSpeak.h>

// Wi-Fi credentials
const char* ssid = "WPA2-Personal";
const char* password = "ICU22g@cu"; // Replace with your Wi-Fi password

// ThingSpeak settings
const char* thingSpeakHost = "api.thingspeak.com";
const char* thingSpeakApiKey = "EUDPCZOCXAPI6S5D"; // Replace with your ThingSpeak API key
WiFiClient client;

// Servo object and pin configuration
Servo myServo;
const int servoPin = 14;

// Ultrasonic sensor pins
const int trigPin = 5;
const int echoPin = 18;

// RGB LED pins
const int redPin = 21;
const int greenPin = 22;
const int bluePin = 23;

// Buzzer pin
const int buzzerPin = 19;

// Variables for radar sweep and alerts
int currentAngle = 0;       // Current angle of the servo
bool forward = true;        // Direction of servo movement
bool delayActive = false;   // Flag to handle the delay state

// Function to measure distance using the ultrasonic sensor
long getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  long distance = duration * 0.034 / 2; // Convert to cm
  return distance;
}

// Function to send data to ThingSpeak
void sendToThingSpeak(long distance, int angle) {
  int alertStatus = (distance > 0 && distance <= 40) ? 1 : 0; // Alert status: 1 if object is close, 0 otherwise

  if (client.connect(thingSpeakHost, 80)) {
    String postStr = "api_key=" + String(thingSpeakApiKey) +
                     "&field1=" + String(distance) +
                     "&field2=" + String(angle) +
                     "&field3=" + String(alertStatus);

    client.println("POST /update HTTP/1.1");
    client.println("Host: api.thingspeak.com");
    client.println("Connection: close");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Content-Length: " + String(postStr.length()));
    client.println();
    client.print(postStr);

    delay(100); // Ensure data is sent
    client.stop();
    Serial.println("Data sent to ThingSpeak.");
  } else {
    Serial.println("Failed to connect to ThingSpeak.");
  }
}

void setup() {
  // Initialize serial monitor
  Serial.begin(115200);

  // Initialize Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize ThingSpeak
  ThingSpeak.begin(client);

  // Attach servo and set GPIO modes
  myServo.attach(servoPin);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  Serial.println("Radar system initialized.");
}

void loop() {
  long distance = getDistance();

  // Check if delay is active (i.e., paused due to object detection)
  if (!delayActive) {
    // Check if the distance is within 40 cm
    if (distance > 0 && distance <= 40) {
      // Activate alert: red light and buzzer
      digitalWrite(redPin, HIGH);
      digitalWrite(greenPin, LOW);
      digitalWrite(bluePin, LOW);
      digitalWrite(buzzerPin, HIGH);

      // Pause servo for 2 seconds
      delayActive = true;
      delay(2000); // 2-second pause
      delayActive = false;

      // Deactivate alert: green light, buzzer off
      digitalWrite(redPin, LOW);
      digitalWrite(greenPin, HIGH);
      digitalWrite(bluePin, LOW);
      digitalWrite(buzzerPin, LOW);
    }
  }

  // Servo rotation logic
  if (forward) {
    currentAngle += 12; // Fast increment for forward rotation
    if (currentAngle >= 180) {
      forward = false; // Change direction at max angle
    }
  } else {
    currentAngle -= 12; // Fast decrement for backward rotation
    if (currentAngle <= 0) {
      forward = true; // Change direction at min angle
    }
  }
  myServo.write(currentAngle); // Move the servo

  // Send data to ThingSpeak
  sendToThingSpeak(distance, currentAngle);

  // Print data to Serial Monitor for debugging
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" cm, Angle: ");
  Serial.print(currentAngle);
  Serial.println();

  delay(10); // Very short delay for smooth rotation
}

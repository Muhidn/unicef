#include <ESP32Servo.h>

// Create a Servo object
Servo myServo;

// Define the GPIO pin connected to the servo signal pin
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

// Variables to control the servo sweep
int currentAngle = 0; // Current angle of the servo
bool forward = true;  // Sweep direction
bool delayActive = false; // Flag for delay

void setup() {
  // Initialize the serial monitor
  Serial.begin(115200);

  // Attach the servo to the defined pin
  myServo.attach(servoPin);

  // Set ultrasonic sensor pins as input/output
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Set RGB LED pins as output
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  // Set the buzzer pin as output
  pinMode(buzzerPin, OUTPUT);

  // Print a message to confirm initialization
  Serial.println("Radar system initialized.");
}

// Function to get distance from ultrasonic sensor
long getDistance() {
  // Send a 10-microsecond pulse to TRIG
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Measure the duration of the pulse on ECHO
  long duration = pulseIn(echoPin, HIGH);

  // Calculate the distance in cm
  long distance = duration * 0.034 / 2;
  return distance;
}

void loop() {
  long distance = getDistance();

  // Send data to the Processing sketch via Serial
  Serial.print(distance);
  Serial.print(",");
  Serial.print(currentAngle);
  Serial.print(",");
  Serial.println(forward ? "1" : "0");

  if (!delayActive) {
    // Check if the distance is within 40 cm
    if (distance > 0 && distance <= 40) {
      // Emit red light
      digitalWrite(redPin, HIGH);
      digitalWrite(greenPin, LOW);
      digitalWrite(bluePin, LOW);

      // Turn on the buzzer
      digitalWrite(buzzerPin, HIGH);

      // Set delay flag and wait for 2 seconds
      delayActive = true;
      delay(2000); // Pause rotation
      delayActive = false;
    } else {
      // Emit green light
      digitalWrite(redPin, LOW);
      digitalWrite(greenPin, HIGH);
      digitalWrite(bluePin, LOW);

      // Turn off the buzzer
      digitalWrite(buzzerPin, LOW);
    }
  }

  // Sweep the servo
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
  delay(100); // Delay for smoother servo movement
}
  
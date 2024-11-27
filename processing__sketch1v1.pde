import processing.serial.*;

// Serial communication setup
Serial port;

// Radar variables
float[] vals = new float[3];
float distance = 0; // Distance detected from radar
int readAngle = 0;  // Angle of the servo
boolean servoForward = true; // Direction of servo sweep

// Line class to represent radar lines
class Line {
  int x = 500;
  int y = 450;
  int R = 12, G = 252, B = 24; // RGB values for radar lines
  int angle;
  int len = 450;
  int detected = 0; // Opacity for detected objects
  float detected_x = 0, detected_y = 0;
  boolean isDetected = false;

  Line(int a) {
    this.angle = a;
  }
}

Line[] lines = new Line[180];
int i = 0; // Current radar angle

void setup() {
  size(1000, 500);
  port = new Serial(this, "COM9", 115200);
  port.bufferUntil('\n');
  background(0);

  for (int i = 0; i < 180; i += 2) {
    lines[i] = new Line(i);
  }

  delay(2000); // Allow time for initialization
}

void draw() {
  background(0);

  // Draw radar arcs and axes
  stroke(12, 252, 32);
  noFill();
  arc(500, 450, 901, 901, PI, 2 * PI);
  arc(500, 450, 600, 600, PI, 2 * PI);
  arc(500, 450, 300, 300, PI, 2 * PI);
  line(0, 450, 1000, 450); // Horizontal axis
  line(500, 0, 500, 450);  // Vertical axis

  fill(255);
  textSize(15);
  text("10 cm", 505, 320);
  text("20 cm", 505, 170);
  text("30 cm", 505, 15);

  // Draw radar line
  if (distance > 0 && distance <= 30) {
    lines[i].isDetected = true;
    lines[i].detected = 250;
    lines[i].detected_x = 500 + distance * 15 * cos(radians(i));
    lines[i].detected_y = 450 - distance * 15 * sin(radians(i));
  }

  for (int j = 0; j < 180; j += 2) {
    if (lines[j].isDetected && lines[j].detected > 0) {
      stroke(lines[j].detected, 0, 0);
      fill(lines[j].detected, 0, 0);
      circle(lines[j].detected_x, lines[j].detected_y, 20);
      lines[j].detected -= 10; // Gradually fade detected objects
    }

    stroke(lines[j].R, lines[j].G, lines[j].B);
    lineAngle(500, 450, j, 450);
  }

  stroke(12, 252, 32);
  lineAngle(500, 450, i, 450);

  // Adjust radar direction based on servo movement
  if (servoForward) {
    i += 2;
    if (i >= 180) {
      i = 178; // Reverse at end
      servoForward = false;
    }
  } else {
    i -= 2;
    if (i <= 0) {
      i = 0; // Reverse at start
      servoForward = true;
    }
  }

  delay(30);
}

// Draw radar line based on angle
void lineAngle(int x, int y, float angle, float length) {
  float radians = angle * PI / 180;
  line(x, y, x + cos(radians) * length, y - sin(radians) * length);
}

// Handle serial data from Arduino
void serialEvent(Serial port) {
  String input = port.readString().trim();
  String[] data = split(input, ',');
  if (data.length == 3) {
    distance = float(data[0]);
    readAngle = int(data[1]);
    servoForward = data[2].equals("1");
    i = readAngle; // Sync radar angle
  }
}

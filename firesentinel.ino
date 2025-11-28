#include <Servo.h>
#include <SoftwareSerial.h>

/*
Pin Configuration:

SERVO1_PIN (10)   → Servo motor 1: Flame scanning
SERVO2_PIN (9)    → Servo motor 2: Water aiming / nozzle
FLAME_ANALOG_PIN (A0) → Flame sensor analog output
RELAY_PIN (7)     → Relay module controlling the water pump
GSM_RX_PIN (2)    → Arduino RX to GSM TX
GSM_TX_PIN (3)    → Arduino TX to GSM RX

Notes:
- All grounds (Arduino GND, servo GND, relay GND, power supply GND) must be common.
- Relay HIGH → water pump ON, Relay LOW → water pump OFF
- Servo 1 rotates to scan for flame, Servo 2 mirrors Servo 1 to aim water.
*/

#define SERVO1_PIN 10
#define SERVO2_PIN 9
#define FLAME_ANALOG_PIN A0
#define RELAY_PIN 7

#define GSM_RX_PIN 2
#define GSM_TX_PIN 3
#define PHONE_NUMBER "09277133957"

Servo servo1;
Servo servo2;

SoftwareSerial gsm(GSM_RX_PIN, GSM_TX_PIN); // RX, TX

// Servo parameters
int angle = 0;
int step = 3;
int scanStep = 2;
int scanDelay = 50;
int noFlameDelay = 15;

// Flame sensor parameters
const int flameThreshold = 850;
const int readingsCount = 5;

// Sweep flag
bool flameSwept = false;

// Function to read flame sensor with averaging
int readFlame() {
  int sum = 0;
  for (int i = 0; i < readingsCount; i++) {
    sum += analogRead(FLAME_ANALOG_PIN);
    delay(2);
  }
  return sum / readingsCount;
}

// Function to send SMS via GSM module
void sendSMS(const char* number, const char* message) {
  gsm.print("AT+CMGS=\"");
  gsm.print(number);
  gsm.println("\"");
  delay(500);
  gsm.print(message);
  delay(500);
  gsm.write(26); // Ctrl+Z to send
  Serial.println("SMS SENT");
  delay(2000); // wait for sending
}

void setup() {
  Serial.begin(9600);
  gsm.begin(9600); // GSM module baud rate

  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Pump OFF initially

  // Initialize GSM (simulation)
  delay(1000);
  gsm.println("AT"); // Test GSM
  delay(500);
  gsm.println("AT+CMGF=1"); // Set SMS text mode
  delay(500);
}

void loop() {
  int flameValue = readFlame();
  Serial.print("Flame value: ");
  Serial.println(flameValue);

  // -------- NO FLAME → SERVO 1 SWEEPS --------
  if (flameValue > flameThreshold) {
    digitalWrite(RELAY_PIN, LOW);
    flameSwept = false;

    angle += step;
    if (angle >= 180) { angle = 180; step = -step; }
    else if (angle <= 0) { angle = 0; step = -step; }

    servo1.write(angle);
    delay(noFlameDelay);
    return;
  }

  // -------- FLAME DETECTED → SIMULATE SMS & DO ONE SWEEP --------
  if (!flameSwept && flameValue <= flameThreshold) {

    // Simulate sending SMS
    Serial.println("SIMULATION: Fire detected! Would send SMS to " PHONE_NUMBER " with message: 'Fire Detected, please check!'");

    // Uncomment this line if you have a SIM and want actual SMS
    // sendSMS(PHONE_NUMBER, "Fire Detected, please check!");

    int brightestAngle = 0;
    int minValue = 1023;

    for (int a = 0; a <= 180; a += scanStep) {
      servo1.write(a);
      delay(scanDelay);

      int sensorValue = readFlame();
      Serial.print("Angle: "); Serial.print(a);
      Serial.print(" Sensor: "); Serial.println(sensorValue);

      if (sensorValue < minValue) {
        minValue = sensorValue;
        brightestAngle = a;
      }
    }

    // Move both servos to target angle
    Serial.print("Brightest angle = ");
    Serial.println(brightestAngle);
    servo1.write(brightestAngle);
    servo2.write(brightestAngle);

    delay(500); // small pause before pumping

    // Turn on water pump
    digitalWrite(RELAY_PIN, HIGH);
    Serial.println("WATER PUMP ON");

    // Keep spraying until flame is gone
    while (readFlame() <= flameThreshold) {
      Serial.println("Extinguishing...");
      delay(200);
    }

    // Flame extinguished
    digitalWrite(RELAY_PIN, LOW);
    Serial.println("FIRE OUT — WATER OFF");

    flameSwept = true;
  }
}

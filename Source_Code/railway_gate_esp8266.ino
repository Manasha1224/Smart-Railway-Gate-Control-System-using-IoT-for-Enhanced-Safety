#define BLYNK_TEMPLATE_ID "TMPL3RUpfJ4IG"
#define BLYNK_TEMPLATE_NAME "Train Detection"
#define BLYNK_AUTH_TOKEN "g0Ia48hnTsyI7BFMTSUG-Rq2AMGeGqqO"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Servo.h>

// WiFi credentials
char ssid[] = "Train";            // Change this
char pass[] = "aswinsr57";        // Change this

// Sensor and Device Pins
#define trigPin1 D1 // GPIO5 (Sensor 1 - before gate)
#define echoPin1 D2 // GPIO4
#define trigPin2 D5 // GPIO14 (Sensor 2 - after gate)
#define echoPin2 D6 // GPIO12
#define buzzerPin D3 // GPIO0
#define servoPin1 D7 // GPIO13
#define servoPin2 D8 // GPIO15

Servo gate1;
Servo gate2;

void setup() {
  Serial.begin(9600);

  pinMode(trigPin1, OUTPUT); pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT); pinMode(echoPin2, INPUT);
  pinMode(buzzerPin, OUTPUT);

  gate1.attach(servoPin1);
  gate2.attach(servoPin2);

  gate1.write(90); // Gates closed
  gate2.write(90);

  // Connect to Wi-Fi & Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  Serial.println("🚦 Smart Railway Gate Ready with Blynk");
}

void loop() {
  Blynk.run();  // Required for Blynk connection

  int distance1 = getDistance(trigPin1, echoPin1);
  int distance2 = getDistance(trigPin2, echoPin2);

  Serial.print("Sensor 1: ");
  Serial.print(distance1);
  Serial.print(" cm | Sensor 2: ");
  Serial.println(distance2);

  // 🔁 Send values to Blynk Virtual Pins
  Blynk.virtualWrite(V1, distance1);  // Sensor 1 (Before gate)
  Blynk.virtualWrite(V2, distance2);  // Sensor 2 (After gate)

  // 🚆 Train Approaching (Sensor 1 Detect)
  if (distance1 > 0 && distance1 < 10) {
    digitalWrite(buzzerPin, HIGH);
    gate1.write(0); gate2.write(0);

    Blynk.virtualWrite(V0, "🚆 Train Approaching");
    Blynk.virtualWrite(V3, "Gate Closed");

    Serial.println("🚆 Train Approaching – Gates Closed, Buzzer ON 🔔");
  }

  // ✅ Train Crossed (Sensor 2 Detect)
  if (distance2 > 0 && distance2 < 10) {
    digitalWrite(buzzerPin, LOW);
    gate1.write(90); gate2.write(90);

    Blynk.virtualWrite(V0, "✅ Train Crossed");
    Blynk.virtualWrite(V3, "Gate Open");

    Serial.println("✅ Train Crossed – Gates Open, Buzzer OFF");
  }

  delay(1000);
}

// 📏 Function to Read Distance
int getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000); // Timeout 30ms
  if (duration == 0) return -1;

  int distance = duration * 0.034 / 2;
  return distance;
}

// Arduino Uno + L298N + 2 motors + 3 HC-SR04
// Simple, fast, and reliable baseline for static indoor obstacle tracks.

// -------- Motor driver pins --------
const uint8_t ENA = 5;   // PWM left
const uint8_t IN1 = 8;
const uint8_t IN2 = 9;
const uint8_t IN3 = 10;
const uint8_t IN4 = 11;
const uint8_t ENB = 6;   // PWM right

// -------- Ultrasonic pins --------
const uint8_t TRIG_LEFT   = A0;
const uint8_t ECHO_LEFT   = A1;
const uint8_t TRIG_CENTER = A2;
const uint8_t ECHO_CENTER = A3;
const uint8_t TRIG_RIGHT  = A4;
const uint8_t ECHO_RIGHT  = A5;

// -------- Tunables --------
const int BASE_SPEED = 160;
const int TURN_SPEED = 150;
const int CORRECT_SPEED_SLOW = 120;
const int OBSTACLE_CM = 15;
const unsigned long ECHO_TIMEOUT_US = 22000; // ~3.7 m max
const int MAX_DISTANCE_CM = 200;

uint8_t frontBlockCount = 0;
bool preferLeft = true;

void setup() {
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(TRIG_LEFT, OUTPUT);
  pinMode(ECHO_LEFT, INPUT);
  pinMode(TRIG_CENTER, OUTPUT);
  pinMode(ECHO_CENTER, INPUT);
  pinMode(TRIG_RIGHT, OUTPUT);
  pinMode(ECHO_RIGHT, INPUT);

  digitalWrite(TRIG_LEFT, LOW);
  digitalWrite(TRIG_CENTER, LOW);
  digitalWrite(TRIG_RIGHT, LOW);

  Serial.begin(9600);
}

void loop() {
  const int left = getDistanceCm(TRIG_LEFT, ECHO_LEFT);
  delay(6);
  const int center = getDistanceCm(TRIG_CENTER, ECHO_CENTER);
  delay(6);
  const int right = getDistanceCm(TRIG_RIGHT, ECHO_RIGHT);

  Serial.print("L:"); Serial.print(left);
  Serial.print(" C:"); Serial.print(center);
  Serial.print(" R:"); Serial.println(right);

  if (center <= OBSTACLE_CM) {
    frontBlockCount++;

    stopBot();
    delay(40);

    moveBackward(TURN_SPEED);
    delay(frontBlockCount >= 3 ? 260 : 170);

    stopBot();
    delay(30);

    if (left > right + 2) {
      pivotLeft(TURN_SPEED);
      delay(frontBlockCount >= 3 ? 480 : 320);
    } else if (right > left + 2) {
      pivotRight(TURN_SPEED);
      delay(frontBlockCount >= 3 ? 480 : 320);
    } else {
      // Tie breaker helps avoid repeated oscillation at 90-degree traps.
      if (preferLeft) {
        pivotLeft(TURN_SPEED);
      } else {
        pivotRight(TURN_SPEED);
      }
      preferLeft = !preferLeft;
      delay(frontBlockCount >= 3 ? 520 : 360);
    }

    stopBot();
    delay(30);
    return;
  }

  frontBlockCount = 0;

  if (left <= OBSTACLE_CM && right <= OBSTACLE_CM) {
    moveBackward(TURN_SPEED);
    delay(120);

    if (preferLeft) {
      pivotLeft(TURN_SPEED);
    } else {
      pivotRight(TURN_SPEED);
    }
    preferLeft = !preferLeft;
    delay(280);

    stopBot();
    delay(30);
    return;
  }

  if (left <= OBSTACLE_CM) {
    // Steer away from left obstacle
    setForwardSpeeds(BASE_SPEED, CORRECT_SPEED_SLOW);
    return;
  }

  if (right <= OBSTACLE_CM) {
    // Steer away from right obstacle
    setForwardSpeeds(CORRECT_SPEED_SLOW, BASE_SPEED);
    return;
  }

  setForwardSpeeds(BASE_SPEED, BASE_SPEED);
}

int getDistanceCm(uint8_t trigPin, uint8_t echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  unsigned long duration = pulseIn(echoPin, HIGH, ECHO_TIMEOUT_US);
  if (duration == 0) {
    return MAX_DISTANCE_CM;
  }

  int distance = (int)(duration * 0.0343f / 2.0f);
  if (distance <= 0 || distance > MAX_DISTANCE_CM) {
    return MAX_DISTANCE_CM;
  }
  return distance;
}

void setForwardSpeeds(int leftSpeed, int rightSpeed) {
  analogWrite(ENA, constrain(leftSpeed, 0, 255));
  analogWrite(ENB, constrain(rightSpeed, 0, 255));

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void moveBackward(int speedValue) {
  analogWrite(ENA, constrain(speedValue, 0, 255));
  analogWrite(ENB, constrain(speedValue, 0, 255));

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void pivotLeft(int speedValue) {
  analogWrite(ENA, constrain(speedValue, 0, 255));
  analogWrite(ENB, constrain(speedValue, 0, 255));

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void pivotRight(int speedValue) {
  analogWrite(ENA, constrain(speedValue, 0, 255));
  analogWrite(ENB, constrain(speedValue, 0, 255));

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void stopBot() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

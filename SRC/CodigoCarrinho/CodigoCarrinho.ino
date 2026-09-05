#include <DabbleESP32.h>
#define IN1 16
#define IN2 17
#define IN3 18
#define IN4 19
#define ENA 5
#define ENB 6

void setup() {
pinMode(IN1, OUTPUT);
pinMode(IN2, OUTPUT);
pinMode(IN3, OUTPUT);
pinMode(IN4, OUTPUT);
pinMode(ENA, OUTPUT);
pinMode(ENB, OUTPUT);
Dabble.begin("My Bluetooth Car");
}

void loop() {
Dabble.processInput();
}
GamePad.onPressed(GAMEPAD_UP, []() {
moveForward();
});
GamePad.onPressed(GAMEPAD_DOWN, 
[]() {
moveBackward();
});
GamePad.onPressed(GAMEPAD_LEFT, []() 
{
turnLeft();
});
GamePad.onPressed(GAMEPAD_RIGHT, 
[]() {
turnRight();
});
GamePad.onPressed(GAMEPAD_STOP, 
[]() {
stopMotors();
});

void moveForward() {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
}
 
void moveBackward() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
}
 
void turnLeft() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
}
 
void turnRight() {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
}
 
void stopMotors() {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
}
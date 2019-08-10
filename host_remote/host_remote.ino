
#include <rf24g.h>

// Button pin numbers
#define CORRECT_BUTTON 12
#define INCORRECT_BUTTON 11
#define READY_BUTTON 10
#define PAUSE_BUTTON 5

// Messages to send base station
#define CORRECT_ANSWER 3
#define INCORRECT_ANSWER 4
#define QUESTION_READY 5
#define PAUSE_GAME 6

// On-board LED pin
#define LED 13

// Create radio object
RF24_G radio;

void setup() {
  Serial.begin(9600);
  // Set up radio, tell it which pins we're using
  radio = RF24_G(2, 7, 9);
  pinMode(PAUSE_BUTTON, INPUT_PULLUP);
  pinMode(CORRECT_BUTTON, INPUT_PULLUP);
  pinMode(INCORRECT_BUTTON, INPUT_PULLUP);
  pinMode(READY_BUTTON, INPUT_PULLUP);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
}

// Sends the message passed in (messageValue) to the base station
void sendMessage(uint8_t messageValue) {
  uint8_t message = messageValue;
  packet sender;
  sender.setAddress(1);
  sender.addPayload(&message, sizeof(uint8_t));
  if (radio.write(&sender) == true) {
    Serial.println("Sent message.");
  }
}

void loop() {
  // Variable to hold message to send
  int message = 0;

  // If any button was pressed, set the message appropriately
  if (digitalRead(PAUSE_BUTTON) == LOW) {
    message = PAUSE_GAME;
  } else if (digitalRead(CORRECT_BUTTON) == LOW) {
    message = CORRECT_ANSWER;
  } else if (digitalRead(INCORRECT_BUTTON) == LOW) {
    message = INCORRECT_ANSWER;
  } else if (digitalRead(READY_BUTTON) == LOW) {
    message = QUESTION_READY;
  }

  // If a message was set, send it
  if (message) {
    sendMessage(message);
    digitalWrite(LED, HIGH);
    delay(1000);
    digitalWrite(LED, LOW);
  } else {
    delay(10);
  }
}

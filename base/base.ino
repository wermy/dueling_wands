#include <rf24g.h>
#include <FastLED.h>

// Messages from wands and host remote
#define PLAYER_LEFT_FLICK 1
#define PLAYER_RIGHT_FLICK 2
#define CORRECT_ANSWER 3
#define INCORRECT_ANSWER 4
#define QUESTION_READY 5
#define PAUSE_GAME 6

// LED stuff
#define NUM_LEDS 300
#define LED_DATA_PIN 5
#define SPEED 5

// Pin numbers for sound triggers
#define CORRECT_SOUND_PIN 10
#define INCORRECT_SOUND_PIN 11
#define SWOOSH_SOUND_PIN 12

// Game states
#define STATE_AWAITING_FLICK 0
#define STATE_PAUSED 1
uint8_t currentState = STATE_PAUSED;

// Radio and LED strip objects
RF24_G radio;
CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(9600);
  // Set up radio and LED strip, tell them which pins we're using
  radio = RF24_G(1, 6, 9);
  LEDS.addLeds<WS2812B,LED_DATA_PIN,GRB>(leds,NUM_LEDS);
  LEDS.setBrightness(255);

  // Set up sound trigger pins
  pinMode(SWOOSH_SOUND_PIN, OUTPUT);
  digitalWrite(SWOOSH_SOUND_PIN, HIGH);
  
  pinMode(CORRECT_SOUND_PIN, OUTPUT);
  digitalWrite(CORRECT_SOUND_PIN, HIGH);
  
  pinMode(INCORRECT_SOUND_PIN, OUTPUT);
  digitalWrite(INCORRECT_SOUND_PIN, HIGH);
}

// Fade all LEDs a little bit,
// this gives it the trail behind the brightest LED
void fadeall() { 
  for(int i = 0; i < NUM_LEDS; i+=SPEED) { 
    for (int j = 0; j < SPEED; j++) {
      leds[i+j].nscale8(250);
    }
   } 
}

// Triggers the corresponding pin on the sound board
void activateSound(int pin) {
  // Bring the pin low to trigger the sound
  digitalWrite(pin, LOW);
  // Hold it for 150ms
  delay(150);
  // Release it (i.e bring it high again)
  digitalWrite(pin, HIGH);
}

// Play a sound and "whoosh" a random color 
// from the left side of the stage
void leftPlayerFlick() {
  activateSound(SWOOSH_SOUND_PIN);
  uint8_t hue = random(0, 255);
  for(int i = 0; i < NUM_LEDS/2; i+=SPEED) {
   for (int j = 0; j < SPEED; j++) {
      leds[i+j] = CHSV(hue, 255, 255);
      leds[NUM_LEDS - i-j] = CHSV(hue, 255, 255);
    }
    FastLED.show(); 
    fadeall();
  }
  for(int i = 0; i < NUM_LEDS; i+=1) {
    FastLED.show();
    fadeall();
  }
}

// Play a sound and "whoosh" a random color 
// from the right side of the stage
void rightPlayerFlick() {
  activateSound(SWOOSH_SOUND_PIN);
  uint8_t hue = random(0, 255);
  for(int i = NUM_LEDS/2-1; i >= 0; i-=SPEED) {
   for (int j = 0; j < SPEED; j++) {
      leds[i-j] = CHSV(hue, 255, 255);
      leds[NUM_LEDS/2 + (NUM_LEDS/2 - i) + j] = CHSV(hue, 255, 255);
    }
    
    FastLED.show(); 
    fadeall();
  }

  for(int i = (NUM_LEDS)-1; i >= 0; i-=1) {
    FastLED.show();
    fadeall();
  }
}

// Trigger the correct sound pin, and 
// show a rainbow that goes back and forth
void correctAnswer() {
  activateSound(CORRECT_SOUND_PIN);
  int speed = SPEED/2;
  static uint8_t hue = 0;
  for(int i = 0; i < NUM_LEDS/2; i+=speed) {
   for (int j = 0; j < speed; j++) {
      leds[i+j] = CHSV(hue++, 255, 255);
      leds[NUM_LEDS - i-j] = CHSV(hue, 255, 255);
    }
    FastLED.show(); 
    fadeall();
  }

  for(int i = NUM_LEDS/2-1; i >= 0; i-=speed) {
   for (int j = 0; j < speed; j++) {
      leds[i-j] = CHSV(hue++, 255, 255);
      leds[NUM_LEDS/2 + (NUM_LEDS/2 - i) + j] = CHSV(hue, 255, 255);
    }
    
    FastLED.show(); 
    fadeall();
  }

  for(int i = (NUM_LEDS)-1; i >= 0; i-=1) {
    FastLED.show();
    fadeall();
  }
}

// Trigger the correct sound pin, and 
// show red going back and forth
void incorrectAnswer() {
  activateSound(INCORRECT_SOUND_PIN);
  static uint8_t hue = 0;
  int speed = SPEED/2;
  for(int i = 0; i < NUM_LEDS/2; i+=speed) {
   for (int j = 0; j < speed; j++) {
      leds[i+j] = CHSV(hue, 255, 255);
      leds[NUM_LEDS - i-j] = CHSV(hue, 255, 255);
    }
    FastLED.show(); 
    fadeall();
  }

  for(int i = NUM_LEDS/2-1; i >= 0; i-=speed) {
   for (int j = 0; j < speed; j++) {
      leds[i-j] = CHSV(hue, 255, 255);
      leds[NUM_LEDS/2 + (NUM_LEDS/2 - i) + j] = CHSV(hue, 255, 255);
    }
    
    FastLED.show(); 
    fadeall();
  }

  for(int i = (NUM_LEDS)-1; i >= 0; i-=1) {
    FastLED.show();
    fadeall();
  }
}

int direction = 0;

void loop() {
  packet receiver;
  uint8_t message;
  // Read from the radio if it has a message for us
  if (radio.available() == true) {
    radio.read(&receiver);
    // Load the payload into the message variable
    receiver.readPayload(&message, sizeof(uint8_t));

    // Check for host remote messages
    if (message == PAUSE_GAME) {
      currentState = STATE_PAUSED;
    } else if (message == QUESTION_READY) {
      currentState = STATE_AWAITING_FLICK;
    }else if (message == CORRECT_ANSWER) {
      correctAnswer();
    } else if (message == INCORRECT_ANSWER) {
      incorrectAnswer();
    }

    // Check for flicks from wands
    if (currentState == STATE_AWAITING_FLICK) {
      if (message == PLAYER_LEFT_FLICK) {
        currentState = STATE_PAUSED;
        leftPlayerFlick();
      } else if (message == PLAYER_RIGHT_FLICK) {
        currentState = STATE_PAUSED;
        rightPlayerFlick();
      }
    }
  }
}


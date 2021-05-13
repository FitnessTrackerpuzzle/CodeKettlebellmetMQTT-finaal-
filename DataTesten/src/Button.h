#include "Arduino.h"

const int PushButton = 15;

static bool buttonPressed = false;
// variables will change:
volatile int buttonState = 0;

void pin_ISR()
{
  buttonState = digitalRead(PushButton);
  buttonPressed = true;
}
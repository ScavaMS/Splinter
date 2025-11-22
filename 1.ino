#include <LedControl.h>

// DIN -> pin 12
// CLK -> pin 11
// CS  -> pin 10
LedControl lc = LedControl(7, 5, 6, 1);  // (DIN, CLK, CS, # of devices)

void setup() {
  lc.shutdown(0, false);      // Wake up MAX7219
  lc.setIntensity(0, 8);      // Brightness (0-15)
  lc.clearDisplay(0);         // Clear screen
}

void loop() {

  // Test 1: turn on each LED one by one
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      lc.setLed(0, row, col, true);
      delay(200);
      lc.setLed(0, row, col, false);
    }
  }

  delay(300);

  // Test 2: turn on all LEDs
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      lc.setLed(0, row, col, true);
    }
  }

  delay(800);

  // Test 3: turn everything off
  lc.clearDisplay(0);
  delay(800);
}

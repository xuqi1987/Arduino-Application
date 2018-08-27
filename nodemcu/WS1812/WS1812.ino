#include <WS2812FX.h>

#define LED_COUNT 8
#define LED_PIN 5

WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  ws2812fx.init();
  ws2812fx.setBrightness(10);
 // ws2812fx.setSpeed(4000);
  ws2812fx.setMode(FX_MODE_STATIC);
  ws2812fx.start();
}

void loop() {
  ws2812fx.service();
}

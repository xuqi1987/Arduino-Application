/* IRremoteESP8266: IRsendDemo - demonstrates sending IR codes with IRsend.
 *
 * Version 1.0 April, 2017
 * Based on Ken Shirriff's IrsendDemo Version 0.1 July, 2009,
 * Copyright 2009 Ken Shirriff, http://arcfn.com
 *
 * An IR LED circuit *MUST* be connected to the ESP8266 on a pin
 * as specified by IR_LED below.
 *
 * TL;DR: The IR LED needs to be driven by a transistor for a good result.
 *
 * Suggested circuit:
 *     https://github.com/markszabo/IRremoteESP8266/wiki#ir-sending
 *
 * Common mistakes & tips:
 *   * Don't just connect the IR LED directly to the pin, it won't
 *     have enough current to drive the IR LED effectively.
 *   * Make sure you have the IR LED polarity correct.
 *     See: https://learn.sparkfun.com/tutorials/polarity/diode-and-led-polarity
 *   * Typical digital camera/phones can be used to see if the IR LED is flashed.
 *     Replace the IR LED with a normal LED if you don't have a digital camera
 *     when debugging.
 *   * Avoid using the following pins unless you really know what you are doing:
 *     * Pin 0/D3: Can interfere with the boot/program mode & support circuits.
 *     * Pin 1/TX/TXD0: Any serial transmissions from the ESP8266 will interfere.
 *     * Pin 3/RX/RXD0: Any serial transmissions to the ESP8266 will interfere.
 *   * ESP-01 modules are tricky. We suggest you use a module with more GPIOs
 *     for your first time. e.g. ESP-12 etc.
 */

#ifndef UNIT_TEST
#include <Arduino.h>
#endif
#include <IRremoteESP8266.h>
#include <IRsend.h>

#define IR_LED 4 // ESP8266 GPIO pin to use. Recommended: 4 (D2).

IRsend irsend(IR_LED);  // Set the GPIO to be used to sending the message.

// Example of data captured by IRrecvDumpV2.ino
uint32_t rawData[141] = {8988,4493,643,1659,618,586,615,589,642,1659,642,561,642,562,641,563,640,563,642,1659,643,1657,642,562,643,1658,641,563,643,560,642,563,616,588,642,561,643,561,642,563,641,562,615,589,617,1683,643,561,641,564,642,561,621,583,621,583,615,588,644,1668,630,562,644,1657,642,562,616,588,641,1660,641,562,641,20011,643,561,641,563,641,563,641,563,627,577,643,561,642,562,643,561,616,588,642,561,640,563,643,562,640,564,642,1657,643,562,615,589,641,563,643,561,644,559,629,576,642,572,629,564,642,561,643,561,618,587,615,588,617,587,617,587,641,563,640,563,644,561,641,562,641};

void setup() {
  irsend.begin();
  Serial.begin(115200);
}

void loop() {
  Serial.println("Send");
  irsend.sendRaw2(rawData,141,38);
  delay(1000);
}

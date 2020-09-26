/*
                8 Input Bus Decoder
                 Copyright (c) 2020 Vernon Billingsley

                 Decode a 8 bit control bus using a AtTiny84

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission
    notice shall be included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.

          AtTiny84             Function
          chip pin
          ------------------------------
          6 - 13     POTRA     Data Input
          2          PB0       Freq change    GATE1
          3          PB1       Voice chnge    GATE2
          4          PB2       ACK Pin        ACK

*/

/************************** Defines **************************************/
/*define the input port */
#define DATA_IN PINA
/*define the gate1 high and low */
#define GATE1_HIGH PORTB |= _BV (0)
#define GATE1_LOW PORTB &= ~_BV (0)
/*define the gate2 high and low */
#define GATE2_HIGH PORTB |= _BV (1)
#define GATE2_LOW PORTB &= ~_BV (1)
/*define the ACK high and low */
#define ACK_HIGH PORTB |= _BV (2)
#define ACK_LOW PORTB &= ~_BV (2)

/*define the chip ID */
#define GATE1_ID 0x11     /*Voice freq program */
#define GATE2_ID 0x21     /*Waveform select */

boolean bus_busy = false;

void setup() {
  /*Set PORTA to input */
  DDRA = B00000000;
  /*Set gate1 to output */
  DDRB |= _BV (0);
  /*Set gate2 to output */
  DDRB |= _BV (1);
  /*Set ACK to Input */
  DDRB &= ~_BV (2);

}

void loop() {
  /*While the data port is low, poll the data port*/
  while (!DATA_IN && !bus_busy) {}
  /*If there is data on the bus */
  if (DATA_IN) {
    /*Check the data for a match */
    if (DATA_IN == GATE1_ID) {
      /*Take gate1 high */
      GATE1_HIGH;
      /*Set bus busy flag */
      bus_busy = true;
    }
    if (DATA_IN == GATE2_ID) {
      /*Put a 10 uS pulse on gate 2 */
      GATE2_HIGH;
      delayMicroseconds(10);
      GATE2_LOW;
      /*Send an ACK to controller to clear the bus */
      /*Wait for the ACK pin to clear */
      while(PINB & _BV (2)){}     
      /*Set the ACK pin to Output */
      DDRB |= _BV (2);
      /*Set the ACK pin High */
      ACK_HIGH;
      /*Wait a little */
      delayMicroseconds(10);
      /*Set the ACK pin Low */
      ACK_LOW;
      /*Set the ACK pin to Input */
      DDRB &= ~_BV (2);
      bus_busy = true;
    }
  }
  /*If the bus busy flag is set, wait for the bus to clear */
  while (DATA_IN && bus_busy) {}
  /*If the bus is clear, rest the flag and lower gate1 pin */
  if (bus_busy && !DATA_IN) {
    GATE1_LOW;
    bus_busy = false;
  }

}

/*************************************************************
digitalWrite.ino
SparkFun SX1509 I/O Expander Example: digital out (digitalWrite)
Jim Lindblom @ SparkFun Electronics
Original Creation Date: September 21, 2015
https://github.com/sparkfun/SparkFun_SX1509_Arduino_Library

This simple example demonstrates the SX1509's digital output 
functionality. Attach an LED to SX1509 IO 15, or just look at
it with a multimeter. We're gonna blink it!

Hardware Hookup:
	SX1509 Breakout ------ Arduino -------- Breadboard
	      GND -------------- GND
	      3V3 -------------- 3.3V
		  SDA ------------ SDA (A4)
		  SCL ------------ SCL (A5)
		  15 -------------------------------- LED+
		                                 LED- -/\/\/\- GND
                                                330

Development environment specifics:
	IDE: Arduino 1.6.5
	Hardware Platform: Arduino Uno
	SX1509 Breakout Version: v2.0

This code is beerware; if you see me (or any other SparkFun 
employee) at the local, and you've found our code helpful, 
please buy us a round!

Distributed as-is; no warranty is given.
*************************************************************/

#include <Wire.h> // Include the I2C library (required)
#include <SparkFunSX1509.h> // Include SX1509 library

SX1509 io0; SX1509 io1; SX1509 io2;

void setup() 
{
  if (!io0.begin(0x3E) || !io1.begin(0x3F) || !io2.begin(0x70))
  {
    while (1) ; // If we fail to communicate, loop forever.
  }
  
  // set all pins to output
  for ( unsigned int i = 0; i < 16; i += 1 )
  {
    io0.pinMode(i, OUTPUT);
  }
  for ( unsigned int i = 0; i < 16; i += 1 )
  {
    io1.pinMode(i, OUTPUT);
  }
  for ( unsigned int i = 0; i < 16; i += 1 )
  {
    io2.pinMode(i, OUTPUT);
  }
}

void loop() 
{
  // clear all
  for ( unsigned int i = 0; i < 16; i += 1 )
  {
    io0.digitalWrite( i, LOW );
  }
  for ( unsigned int i = 0; i < 16; i += 1 )
  {
    io1.digitalWrite( i, LOW );
  }
  for ( unsigned int i = 0; i < 16; i += 1 )
  {
    io2.digitalWrite( i, LOW );
  }

  // run through
  for ( unsigned int i = 0; i < 16; i += 1 )
  {
    io0.digitalWrite( i, HIGH );
    delay(75);
  }
  for ( unsigned int i = 0; i < 16; i += 1 )
  {
    io1.digitalWrite( i, HIGH );
    delay(75);
  }
  for ( unsigned int i = 0; i < 16; i += 1 )
  {
    io2.digitalWrite( i, HIGH );
    delay(75);
  }
}

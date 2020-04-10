#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

#define B_R		4
#define B_L		2
#define	B_T		3

Adafruit_BicolorMatrix matrix = Adafruit_BicolorMatrix();

//uint8_t display_map [12];
//initialize the map for testing purposes
uint8_t display_map [12] = {
	B00000000,
	B00000000,

	B00000000,
	B00000000,
	B00000000,
	B00000000,
	B00000000,
	B00000000,
	B00000000,
	B11111111,

	B00000000,
	B00000000
};

uint32_t thisTime, lastTime;		//takes care of interval timing
uint8_t row_count = 0;				//keeps track of which row is being addressed
uint8_t shift_left = 2;				//tracks horizonatal location of piece
uint8_t angle = 0;					//tracks four orienations of piece
uint8_t collision = false;			//indicates downward collision between pieces

const char piece_I[12] = {
	B00000000,
	B00000111,
	B00000000,

	B00000010,
	B00000010,
	B00000010,

	B00000000,
	B00000111,
	B00000000,

	B00000010,
	B00000010,
	B00000010,
};

const char piece_T[12] = {
	B00000001,
	B00000011,
	B00000001,

	B00000010,
	B00000111,
	B00000000,

	B00000010,
	B00000011,
	B00000010,

	B00000000, 
	B00000111,
	B00000010,
};

void no_collision() {
	for (int row = 0; row < 3; row++) {
		if (display_map[row + row_count] & (piece_T[row + angle] << shift_left)) {
			collision = true;
		}
	}

}

void place_piece2() {
	for (int row = 0; row < 3; row++) {
		display_map[row + row_count] |= (piece_T[row + angle] << shift_left);
	}
}

void place_piece() {
	for (int row = 0; row < 3; row++) {
		display_map[row + row_count] |= (piece_T[row + angle] << shift_left);
		//check for bottom collision
	 	if (display_map[row + row_count+1] & (piece_T[row + angle] << shift_left)) {
			collision = true;
			//break; //THIS WAS REMOVED TO ALLOW FINAL LAYER TO BE ADD CHECK FOR PROBLEMS IN ALL CASES 
		}
	}
}

void remove_piece() {
	for (int row = 0; row <3; row++) {
		display_map[row + row_count] &= ~(piece_T[row + angle] << shift_left);
	}
}

void draw_map() {
	for (int y = 0; y <= 8; y++) {
		for (int x = 0; x <= 8; x++) {
			if (display_map[y+2] & 1<<x) {
				matrix.drawPixel(y, x, LED_GREEN);  
			} else {
				matrix.drawPixel(y, x, LED_OFF);  
			}
		}
	}
}

void setup() {
	Serial.begin(9600);
	Serial.println("8x8 LED Matrix Test");

	pinMode(B_R, INPUT_PULLUP);
	pinMode(B_L, INPUT_PULLUP);
	pinMode(B_T, INPUT_PULLUP);

	matrix.begin(0x70);  // pass in the address
	matrix.writeDisplay();
}

void loop() {	
	thisTime = millis();
	if ((thisTime - lastTime) >= 1000) {
		lastTime = thisTime;
		row_count++;
	}
	if (!digitalRead(B_R)) {
		if (shift_left++ >= 7) shift_left= 7;
		delay(200);
	}
	if (!digitalRead(B_L)) {
		if (shift_left-- <= 0) shift_left= 0;
		delay(200);
	}
	if (!digitalRead(B_T)) {
		angle += 3;
		if (angle >= 12) angle = 0;
		delay(200);
	}
	no_collision();
	place_piece2();
	//place_piece();
	draw_map();
	if ((row_count < 8) && (collision == false)) {
		remove_piece();
		//row_count++;
	} else { 
		row_count = 0;
		collision = false;
	}
	matrix.writeDisplay();
}


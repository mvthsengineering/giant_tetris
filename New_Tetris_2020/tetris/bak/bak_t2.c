#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

#define B_R		4
#define B_L		2
#define	B_T		3

Adafruit_BicolorMatrix matrix = Adafruit_BicolorMatrix();

//initialize the map for testing purposes
/*
uint8_t current_map [12] = {
	B00000000,
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
	B00000000
};
*/

uint16_t current_map [12] = {
	0x0001,
	0x0001,

	0x0001,
	0x0001,
	0x0001,
	0x0001,
	0x0001,
	0x0001,
	0x0001,
	0x0001,
	
	0xFFFF,
	0x0000
};


uint8_t saved_map[12];

uint32_t thisTime, lastTime;		//takes care of interval timing
uint8_t row_count = 0;				//keeps track of which row is being addressed
uint8_t shift_left = 2;				//tracks horizonatal location of piece
uint8_t angle = 0;					//tracks four orienations of piece
bool dropping = false;
uint8_t piece_id = 0;

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

const char piece_L[12] = {
	B00000001,
	B00000001,
	B00000011,

	B00000100,
	B00000111,
	B00000000,

	B00000011,
	B00000010,
	B00000010,

	B00000000, 
	B00000111,
	B00000001,
};

const char piece_Z[12] = {
	B00000100,
	B00000110,
	B00000010,

	B00000011,
	B00000110,
	B00000000,

	B00000010,
	B00000011,
	B00000001,

	B00000000, 
	B00000011,
	B00000110,
};


const char *pieces[4] = {
	piece_T,
	piece_I,
	piece_Z,
	piece_L
};

const char *piece; 

bool collision() {
	int collision = false;
	for (int row = 0; row < 3; row++) {
		//if (current_map[row + row_count] & (piece_T[row + angle] << shift_left)) {
		if (current_map[row + row_count] & (piece[row + angle] << shift_left)) {
			collision = true;
		}
	}
	return collision;
}


void add_piece() {
	for (int row = 0; row < 3; row++) {
		//current_map[row + row_count] |= (piece_T[row + angle] << shift_left);
		current_map[row + row_count] |= (piece[row + angle] << shift_left);
	}
}

void remove_piece() {
	for (int row = 0; row < 3; row++) {
		//current_map[row + row_count] &= ~(piece_T[row + angle] << shift_left);
		current_map[row + row_count] &= ~(piece[row + angle] << shift_left);
	}
}

void print_map() {
	for (int y = 0; y <= 8; y++) {
		for (int x = 0; x <= 8; x++) {
			if (current_map[y+2] & 1<<x) {
				matrix.drawPixel(y, x, LED_GREEN);  
			} else {
				matrix.drawPixel(y, x, LED_OFF);  
			}
		}
	}
}

void record_map() {
	for (int row = 0; row < 12; row++) 
		saved_map[row] = current_map[row];
}

void revert_map() {
	for (int row = 0; row < 12; row++) 
		current_map[row] = saved_map[row];
}


void setup() {
	Serial.begin(9600);
	Serial.println("8x8 LED Matrix Test");

	pinMode(B_R, INPUT_PULLUP);
	pinMode(B_L, INPUT_PULLUP);
	pinMode(B_T, INPUT_PULLUP);

	matrix.begin(0x70);  
	matrix.writeDisplay();

	piece = pieces[piece_id];
}

void loop() {	
	thisTime = millis();
	if ((thisTime - lastTime) >= 1000) {
		lastTime = thisTime;
		row_count++;
		dropping = true;
	} 
	else {

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
	}
	
	if (collision() == false ) {
		add_piece();
		record_map();
		print_map();
		remove_piece();
	} 
	else { 
		revert_map();	
		print_map();
		if (dropping == true) { 
			row_count = 0;
			piece_id = random(0,4);
			piece = pieces[piece_id];
		}
	}
	dropping = false;
	matrix.writeDisplay();
}


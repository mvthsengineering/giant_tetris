#include <Wire.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"

//Buttons for controlling the pieces
#define B_R		4
#define B_L		2
#define	B_T		3

#define ROW_OFFSET	2 //this is a direct offset pushing current_map up two rows
#define COL_OFFSET	1 //this is direct offset pushing the current_map right one row 

Adafruit_BicolorMatrix matrix = Adafruit_BicolorMatrix();

//This map defines a boarder around an 8x8 tetris panel
uint16_t current_map [12] = {
	0x0201,
	0x0201,

	0x0201,
	0x0201,
	0x0201,
	0x0201,
	0x0201,
	0x0201,
	0x0201,
	0x0201,
	
	0xFFFF,
	0x0000
};

uint32_t thisTime, lastTime;		//takes care of interval timing
uint8_t row_count = 0;				//keeps track of which row is being addressed
uint8_t shift_left = 1;				//tracks horizonatal location of piece, starts with a 1 offset
uint8_t angle = 0;					//tracks four orienations of piece
uint8_t piece_id = 0;				//tracks which piece is being addressed

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

//This tests the placement of a piece by attempting to add it. You need to test collision()
//before adding a piece.  If you add a piece before running collision(), than collision 
//will always return true.
bool collision() {
	int collision = false;
	for (int row = 0; row < 3; row++) {
		if (current_map[row + row_count] & (piece[row + (angle*3)] << shift_left)) {
			collision = true;
		}
	}
	return collision;
}

void add_piece() {
	for (int row = 0; row < 3; row++) {
		current_map[row + row_count] |= (piece[row + (angle*3)] << shift_left);
	}
}

void remove_piece() {
	for (int row = 0; row < 3; row++) {
		current_map[row + row_count] &= ~(piece[row + (angle*3)] << shift_left);
	}
}

//This removes complete rows and shifts the map down one 
bool remove_row() {
	for (int row = 0; row < 8; row++) {
		if ((current_map[row+ROW_OFFSET] & (0x0FF<<COL_OFFSET)) == (0x0FF<<COL_OFFSET)) {
			for (int map_row = row+ROW_OFFSET; map_row > 0; map_row--) {
				current_map[map_row] = current_map[map_row-1];
			}
			return true;
		}
	}
}

//This prints the current_map to the display
void print_map() {
	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col++) {
			if (current_map[row+ROW_OFFSET] & 1<<col+COL_OFFSET) {
				matrix.drawPixel(row, col, LED_GREEN);  
			} else {
				matrix.drawPixel(row, col, LED_OFF);  
			}
		}
	}
}

//NEXT NEED TO ADD POINT SYSTEM AND GAME OVER SEQUENCE
void setup() {
	Serial.begin(9600);
	Serial.println("8x8 LED Matrix Test");

	pinMode(B_R, INPUT_PULLUP);
	pinMode(B_L, INPUT_PULLUP);
	pinMode(B_T, INPUT_PULLUP);

	matrix.begin(0x70);  
	matrix.writeDisplay();

	piece = pieces[piece_id];
	randomSeed(analogRead(0));
}

void loop() {	
	thisTime = millis();
	if ((thisTime - lastTime) >= 1000) {
		lastTime = thisTime;
		row_count++;
		if (collision() == false) {
			add_piece();
		}
		else {
			row_count--;
			add_piece();
			row_count = 0;
			piece_id = random(0,4);
			piece = pieces[piece_id];
			shift_left = random(1, 7);	
			remove_row();
		}
		print_map();
		remove_piece();
	} 
	else if (!digitalRead(B_R)) {
		shift_left++;
		delay(200);
		if (collision() == false) {
			add_piece();	
		}
		else {
			shift_left--;
			add_piece();
		}
		print_map();
		remove_piece();
	}
	else if (!digitalRead(B_L)) {
		shift_left--;
		delay(200);
		if (collision() == false) {
			add_piece();	
		}
		else {
			shift_left++;
			add_piece();
		}
		print_map();
		remove_piece();
	}
	else if (!digitalRead(B_T)) {
		if (++angle >= 4) angle = 0;
		delay(200);
		if (collision() == false) {
			add_piece();	
		}
		else {
			if (--angle < 0) angle = 0;
			add_piece();
		}
		print_map();
		remove_piece();
	}
	matrix.writeDisplay();
}


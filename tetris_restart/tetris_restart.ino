#include <Wire.h> //allows communication with I2C
#include "Adafruit_MCP23008.h" //Library for MCP23008 Microcontroller
#define FULL_ON LOW
#define FULL_OFF HIGH
Adafruit_MCP23008 mcp[8]; //0 through 7 index values

const int rows = 7;
const int columns = 5;
uint8_t row_count = 0;
uint8_t shift_left = 0;

uint16_t tetris_map [8] = { //Array holding values for each row on the tetris board (Hexadecimal)
  0x0000,  //0
  0x0000,  //1
  0x0000,  //2
  0x0000,  //3
  0x0000,  //4
  0x0000,  //5
  0x0000,  //6: Last Row
  0xFFFF   //7: Bottom Barrier
};

const char piece_T[12] = { //Piece "T"
  B00001110,
  B00000100,
  B00000000,

  B00000100,
  B00000110,
  B00000100,

  B00000100,
  B00001110,
  B00000000,

  B00000100,
  B00001100,
  B00000100
};
const char piece_Z[12] = { //Piece "Z"
  B00000100,
  B00000110,
  B00000010,

  B00000110,
  B00001100,
  B00000000,

  B00001000,
  B00001100,
  B00000100,

  B00000110,
  B00001100,
  B00000000
};
const char piece_I[12] = { //Piece "I"
  B00000100,
  B00000100,
  B00000100,

  B00000000,
  B00000111,
  B00000000,

  B00000100,
  B00000100,
  B00000100,

  B00000000,
  B00000111,
  B00000000
};
const char piece_L[12] = { //Piece "L"
  B00000100,
  B00000100,
  B00001100,

  B00000100,
  B00000111,
  B00000000,

  B00000011,
  B00000010,
  B00000010,

  B00000000,
  B00000111,
  B00000001
};

const char *pieces[4] = {
  piece_T,
  piece_Z,
  piece_I,
  piece_L
};
const char *piece;

void print_panel(uint8_t x, uint8_t y, uint8_t level) {
  mcp[y].digitalWrite(x, level); //turn on/off panel at (x, y)
}

void print_map() { //Turns on panels mapped to tetris_map
  for (int row = 0; row < rows; row++) { //While 0 < 7
    for (int col = 0; col < columns; col++) { //While 0 < 5
      if (tetris_map[row] & 1 << col) { //If a certain panel has a 1,
        print_panel(col, row, FULL_ON); //Turn on panel at the column and row
      } else {
        print_panel(col, row, FULL_OFF); //Turn off panel at the column and row
      }
    }
  }
}

void remove_piece() {
  for (int row = 0; row < 3; row++) {
    tetris_map[row + row_count] &= ~(piece[row]);
  }
}

void add_piece() {
  for (int num = 0; num < 3; num++) {
    tetris_map[num + row_count] |= piece[num]; //Adds Tetris piece to the tetris map
  }
}

/*void down() {
  for (int i = 6; i > 0; i--) {
    tetris_map[i] = tetris_map[i - 1]; //Sets each row to the row above
  }
  delay(1100);
  tetris_map[0] = 0; //Clears the first row of the tetris board
  print_map();
  } */

bool collision() {
  boolean collide = false;
  for (int row = 0; row < 3; row++) {
    if (tetris_map[row + row_count] &= piece[row]) { //This will read collision each time because the location of the piece is traveling with the rows of the tetris map
      collide = true;
    }
  }
  return collision;
}

void new_piece() {
  row_count = 0;
  shift_left = 0;
  piece = pieces[random(4)];
}
void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 7; i++) {
    mcp[i].begin(i + 1); //initializes each MCP23008
    for (int z = 0; z < 5; z++) {
      mcp[i].pinMode(z, OUTPUT); //sets up each pin output
    }
  }
  Serial.println("begin");
  new_piece();
  add_piece();
  print_map();
  delay(1100);
  remove_piece();
  print_map();
  delay(1100);
  if (collision() == true) {
    Serial.println("collision");
  }
}

void loop() {

  /*remove_piece();
  row_count++;
  if (collision() == true) {
    row_count--;
    add_piece();
    new_piece();
    Serial.println("collision");
  } else {
    add_piece();
  }
  print_map();

  delay(1100); */
}

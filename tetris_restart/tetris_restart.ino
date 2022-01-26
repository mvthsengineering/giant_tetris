
#include <Wire.h> //allows communication with I2C
#include "Adafruit_MCP23008.h" //Library for MCP23008 Microcontroller
#define FULL_ON LOW
#define FULL_OFF HIGH
#define ROW_OFFSET 2

Adafruit_MCP23008 mcp[8]; //0 through 7 index values

const int rows = 7;
const int columns = 5;
uint8_t row_count = 0;
uint8_t shift_right = 2;
int8_t rotate = 0;

int points = 0;

uint16_t tetris_map [10] = { //Array holding values for each row on the tetris board (Hexadecimal)
  0x0000,
  0x0000,

  0x0000,  //
  0x0000,  //
  0x0000,  //
  0x0000,  //
  0x0000,  //
  0x0000,  //
  0x0000,  //

  0xFFFF,  // Bottom Barrier
};

const char piece_T[12] = { //Piece "T"
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
  B00000010
};
const char piece_Z[12] = { //Piece "Z"
  B00000010,
  B00000011,
  B00000001,

  B00000011,
  B00000110,
  B00000000,

  B00000010,
  B00000011,
  B00000001,

  B00000000,
  B00000011,
  B00000110
};
const char piece_I[12] = { //Piece "I"
  B00000001,
  B00000001,
  B00000001,

  B00000000,
  B00000111,
  B00000000,

  B00000001,
  B00000001,
  B00000001,

  B00000000,
  B00000111,
  B00000000
};
const char piece_L[12] = { //Piece "L"
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

// A loop to test if the panels are working properly
// (Run this by itself if the code or panels are acting up)
void loop_test() {
  for (int row = 0; row < rows; row++) {
    for (int col = 0; col < columns; col++) {
      print_panel(col, row, FULL_ON);
      delay(250);
      print_panel(col, row, FULL_OFF);
      delay(250);
    }
  }
}

//Clears tetris map
void clear_map() {
  for (int row = 0; row <= 8; row++) {
    tetris_map[row] = 0; //Resets each row to 0
  }
}

//Fills tetris map
void fill_map() {
  for (int row = 0; row <= 8; row++) {
    tetris_map[row] = 0x001F; //Sets each row to full
  }
}

//Prints the game to the tetris map
void print_map() { //Turns on panels mapped to tetris_map
  for (int row = 0; row < rows; row++) { //While 0 < 7
    for (int col = 0; col < columns; col++) { //While 0 < 5
      if (tetris_map[row + ROW_OFFSET] & 1 << col) { //If a certain panel has a 1,
        print_panel(col, row, FULL_ON); //Turn on panel at the column and row
      } else {
        print_panel(col, row, FULL_OFF); //Turn off panel at the column and row
      }
    }
  }
}

//Removes piece from the tetris map so that a collision is not triggered
void remove_piece() {
  for (int row = 0; row < 3; row++) {
    tetris_map[row + row_count] &= ~(piece[row + (rotate * 3)] << shift_right);
  }
}

//Adds piece to tetris map and moves pieces down
void add_piece() {
  for (int num = 0; num < 3; num++) { //Sets the rows of the tetris map following the row count to the current piece
    tetris_map[num + row_count] |= (piece[num + (rotate * 3)] << shift_right);
  }
}

//Tests piece collision
bool collision() {
  bool collide = false;
  for (int row = 0; row < 3; row++) {
    if (tetris_map[row + row_count] & (piece[row + (rotate * 3)] << shift_right)) {
      collide = true;
    }
  }
  return collide;
}

//Animation for when a row is completed
void row_animation(int row) {
  for (int x = 0; x < 2; x++) {
    tetris_map[row] = 0;
    print_map();
    delay(200);
    tetris_map[row] = 0x001F;
    print_map();
    delay(200);
  }
}

//Tests if there is a complete row, and initiates a row clear
void complete_row() {
  int completed_rows = 0;
  for (int checks = 3; checks > 0; checks--) { //Checks for complete rows three times, the most amount of rows that can be completed at once
    for (int row = 8; row >= 0; row--) { //Checks each tetris row, starting with the bottom
      if (tetris_map[row] == 0x001F) { //If a row is full, the value will equal 0x001F
        completed_rows++;
        row_animation(row);
        for (int num = row; num >= 0; num--) {
          tetris_map[num] = tetris_map[num - 1]; //Make the rows fall to the detected row
        }
        tetris_map[0] = 0; //Ensures no extraneous pieces are added to the tetris board
      }
    }
  }
  
  if (completed_rows == 1) { //Point tracking system in accordance with amount of rows completed
    points += 1;
  }
  if (completed_rows == 2) {
    points += 4;
  }
  if (completed_rows == 3) {
    points += 7;
  }
  completed_rows = 0;
}

//Checks if the pieces exceed the top of the tetris map
bool game_over() {
  bool game = false;
  if (tetris_map[2] > 0) { //If the top row is equal to a piece, game over.
    game = true;
  }
  return game;
}

void ending() {
  Serial.println("Game over!");
  delay(1000);
  Serial.print("Player, you had ");
  delay(1000);
  Serial.print(points);
  Serial.println(" points!");
  Serial.println();
  delay(1000);
  for (int x = 0; x < 3; x++) {
    clear_map();
    print_map();
    delay(400);
    fill_map();
    print_map();
    delay(400);
  }
  clear_map();
  print_map();
  delay(1200);
}

//Initializes a new piece and resets increment values
void new_piece() {
  rotate = 0;
  row_count = 0;
  shift_right = 2;
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
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);

  new_piece();
  Serial.println("Tetris, by Dylan Smith");
  delay(790);
  Serial.println("2022");
  Serial.println();
  delay(790);
  Serial.println("--------New-Game--------");
}

void loop() {

  print_map();

  int left = digitalRead(4); //Move piece left
  int right = digitalRead(5); //Move piece right
  int rotatebutton = digitalRead(6); //Rotate piece

  //--------------------------Buttons-----------------------------------------

  //Left Shift Button
  if (left == LOW) {
    remove_piece();
    shift_right--;
    if (collision() == true) {
      shift_right++;
    }
    delay(10);
  }

  //Right Shift Button
  if (right == LOW) {
    remove_piece();
    shift_right++;
    if ((rotate == 1 || rotate == 3) && shift_right == 3) { //Stops piece from leaving boundary when rotated
      shift_right--;
    }
    if (shift_right > 3) { //If a piece is at the rightmost point of the tetris board, do not exceed the position limit of 3
      if (piece == pieces[2]) { //There was an issue pertaining to the "I" piece, this operator makes it so the "I" can go all the way right
        if (shift_right > 4) {
          shift_right--; //Ensures the "I" piece doesn't exceed the boundary
        }
        shift_right++;
      }
      shift_right--;
    }
    if (collision() == true) {
      shift_right--;
    }
    delay(10);
  }

  //Rotate Button
  if (rotatebutton == LOW) {
    remove_piece();
    rotate++;
    if (shift_right == 3) { //Fixes a bug where the piece is allowed to leave the boundary when rotated
      rotate--;
    }
    if (rotate > 3) { //Resets the piece to its original position after going through each rotation
      rotate = 0;
    }
    if (collision() == true) {
      rotate--;
    }
    delay(10);
  }

  //-----------The-Game-----------------------------------

  remove_piece(); // Remove piece to ensure no collision

  row_count++; // Moves the piece down

  if (collision() == true) {

    //game_over(); //Checks if game is over

    row_count--; // Stops the piece at the bottom
    add_piece(); // Sets piece to the tetris map
    complete_row(); // Checks and clears any full rows

    new_piece(); // Initializes new piece

    row_count--; // Correct position for the piece

    if (game_over() == true) {
      ending();
      points = 0;
      delay(790);
      Serial.println();
      Serial.println("--------New-Game--------");
    }

  } else {
    add_piece();
  }
  print_map();

  delay(1000);

}

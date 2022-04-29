#include <Wire.h> //allows communication with I2C
#include "Adafruit_MCP23008.h" //Library for MCP23008 Microcontroller
#include <EEPROM.h>
#include <avr/wdt.h>

#define REST 0
#define GAME 1
#define HIGHSCORE 2

#define FULL_ON LOW
#define FULL_OFF HIGH
#define ROW_OFFSET 2
#define COL_OFFSET 1

uint8_t state = REST;

Adafruit_MCP23008 mcp[8]; //0 through 7 index values

const int rows = 7;
const int columns = 5;
uint8_t row_count = 0;

int piece_id = 0;
int shift_right = 1;
int8_t rotate = 0;

char Incoming_value = 0; //Value that translates over Serial port to app

long previousMillis = 0;
long interval = 800;

int points = 0;
char initialValue[3];
char initials[3];

int placement = 0;
boolean highscoreCase = false;
int highscoreCode = 99;
int highscoreCode2 = 77;

uint16_t tetris_map [10] = { //Array holding values for each row on the tetris board (Hexadecimal)
  0x0041,
  0x0041,

  0x0041,
  0x0041,
  0x0041,
  0x0041,
  0x0041,
  0x0041,
  0x0041,

  0xFFFF,
};

uint16_t game_map [10] = { //This is the map with all the boundaries required for tetris to operate correctly
  0x0041,
  0x0041,

  0x0041,
  0x0041,
  0x0041,
  0x0041,
  0x0041,
  0x0041,
  0x0041,

  0xFFFF,  // Bottom Barrier
};

const char piece_T[12] = { //Piece "T"
  B00000010,
  B00000111,
  B00000000,

  B00000010,
  B00000011,
  B00000010,

  B00000000,
  B00000111,
  B00000010,

  B00000010,
  B00000110,
  B00000010,
};
const char piece_L[12] = { //Piece "L"
  B00000010,
  B00000010,
  B00000011,

  B00000001,
  B00000111,
  B00000000,

  B00000110,
  B00000010,
  B00000010,

  B00000000,
  B00000111,
  B00000100,
};
const char piece_I[12] = { //Piece "I"
  B00000010,
  B00000010,
  B00000010,

  B00000000,
  B00000111,
  B00000000,

  B00000010,
  B00000010,
  B00000010,

  B00000000,
  B00000111,
  B00000000,
};
const char piece_Z[12] = { //Piece "Z"
  B00000110,
  B00000011,
  B00000000,

  B00000001,
  B00000011,
  B00000010,

  B00000110,
  B00000011,
  B00000000,

  B00000001,
  B00000011,
  B00000010,
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
    tetris_map[row] = 0xFFFF; //Sets each row to full
  }
}

//Initializes tetris to game map with boundaries
void start_map() {
  for (int i = 0; i < 10; i++) {
    tetris_map[i] = game_map[i];
  }
}

//Prints the game to the tetris map
void print_map() { //Turns on panels mapped to tetris_map
  for (int row = 0; row < rows; row++) { //While 0 < 7
    for (int col = 0; col < columns; col++) { //While 0 < 5
      if (tetris_map[row + ROW_OFFSET] & 1 << col + COL_OFFSET) { //If a certain panel has a 1,
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
    tetris_map[row] = 0x00FF;
    print_map();
    delay(200);
  }
}

//Tests if there is a complete row, and initiates a row clear
void complete_row() {
  int completed_rows = 0;
  for (int checks = 3; checks > 0; checks--) { //Checks for complete rows three times, the most amount of rows that can be completed at once
    for (int row = 8; row >= 0; row--) { //Checks each tetris row, starting with the bottom
      if (tetris_map[row] == 0x007F) { //If a row is full, the value will equal 0x007F
        completed_rows++;
        row_animation(row);
        for (int num = row; num >= 0; num--) {
          tetris_map[num] = tetris_map[num - 1]; //Makes the rows fall to the detected row
        }
        for (int i = 0; i < 10; i++) {
          tetris_map[i] |= game_map[i]; //Contains the game maps boundaries
        }
        tetris_map[0] = 0; //Ensures no extraneous pieces are added to the tetris board
        row++;
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

//Displays upcoming piece to Serial port which is displayed by tetris app
void piece_display() {
  Serial.println();
  if (piece_id == 0) {
    Serial.println("Next piece:  ");
    Serial.println("ooo");
    Serial.println(" o ");
  }
  if (piece_id == 3) {
    Serial.println("Next piece:  ");
    Serial.println(" o ");
    Serial.println(" o ");
    Serial.println("oo ");
  }
  if (piece_id == 2) {
    Serial.println("Next piece:  ");
    Serial.println(" o ");
    Serial.println(" o ");
    Serial.println(" o ");
  }
  if (piece_id == 1) {
    Serial.println("Next piece:  ");
    Serial.println("     oo");
    Serial.println("oo ");
  }
}

//Checks if the pieces exceed the top of the tetris map
bool game_over() {
  bool gameOn = false;
  if (tetris_map[2] > 0x0041) { //If the top row has a value greater than it already has, game over.
    gameOn = true;
  }
  return gameOn;
}

//Game end event
void ending() {
  //Ending Message
  Serial.print(points);
  Serial.println(" points!");
  Serial.print("|");
  Serial.println();
  delay(1000);
  //Ending Animation
  for (int x = 0; x < 3; x++) {
    clear_map();
    print_map();
    delay(400);
    fill_map();
    print_map();
    delay(400);
  }
  start_map();
  print_map();
  delay(500);
}

//Game start animation
void game_start() {
  for (int x = 0; x < 3; x++) {
    clear_map();
    print_map();
    delay(100);
    fill_map();
    print_map();
    delay(100);
  }
  start_map();
  print_map();
  piece_id = random(3);
  new_piece();
  add_piece();
  print_map();
  delay(500);
}

//Initializes a new piece and resets increment values
void new_piece() {
  rotate = 0;
  row_count = 0;
  shift_right = 2;
  piece = pieces[piece_id];
}

void random_piece() {
  rotate = random(4);
  row_count = 0;
  shift_right = random(6);
  piece = pieces[random(4)];
}

void setup() {
  Serial.begin(9600);
  wdt_enable(WDTO_8S);
  for (int i = 0; i < 7; i++) {
    mcp[i].begin(i + 1); //initializes each MCP23008
    for (int z = 0; z < 5; z++) {
      mcp[i].pinMode(z, OUTPUT); //sets up each pin output
    }
  } 
  piece_id = random(3);
  new_piece();
  add_piece();
  print_map();
}

void loop() {
  /* * Tetris Game consists of three states
      REST: The Tetris main menu, you are able to start the game and view highscore.

      GAME: The Tetris game, where pieces are constantly being placed, removed, rotated, and shifted on the tetris map.
      When the game ends you may be able to put in your initials if a highscore is triggered.

      HIGHSCORE: A code is sent through the serial port and registered by the Tetris app.
      This causes the app to prompt the user for their initials, saving this information along with the amount of points.
  */
  switch (state) {

    case REST:
      {
        unsigned long currentMillis = millis();
        //Menu Buttons
        if (Serial.available() > 0)
        {
          Incoming_value = Serial.read();

          if (Incoming_value == '4') { //Reads when button is pressed
            state = GAME; //Starts Tetris Game
          }
          if (Incoming_value == '5') {
            for (int i = 0; i < 12; i += 4) { //Displays game highscores and initials
              Serial.print(EEPROM.read(i));
              Serial.print(" ");
              Serial.print((char)EEPROM.read(i + 1));
              Serial.print((char)EEPROM.read(i + 2));
              Serial.print((char)EEPROM.read(i + 3));
              Serial.println();
            }
            Serial.print("|");
            Serial.println();
          }
        }
        //Runs Ambient Unplayable Tetris
        if ((currentMillis - previousMillis) > interval) { //Timer used in replacement for delay, allowing game to run smoothly
          previousMillis = currentMillis;
          remove_piece(); //Removes piece so that there is no collision
          row_count++; //Moves pieces down by incrementing the row

          if (collision() == true) {

            game_over(); //Checks if game is over

            row_count--; // Stops the piece at the bottom
            add_piece(); // Sets piece to the tetris map
            complete_row(); // Checks and clears any full rows

            random_piece(); // Initializes new piece
            piece_id = random(4);

            row_count--; // Correct position for the piece

            if (game_over() == true) { //Game Over Event
              clear_map();
              print_map();
              start_map();
              print_map();
            }
          } else {
            add_piece();
          }
          wdt_reset();
          print_map();
        }
        if(state == GAME) {
          points = 0;
          game_start();
        }
      }
      break;

    case GAME:
      {
        unsigned long currentMillis = millis();

        //-------------------Buttons-------------------------
        if (Serial.available() > 0)
        {
          Incoming_value = Serial.read();

          //Shift left button
          if (Incoming_value == '8') { //Reads when button is pressed
            remove_piece();
            shift_right--;
            if (collision() == true) {
              shift_right++;
              //Serial.println("left collision");
            }
            add_piece();
            print_map();
            delay(180);
          }

          //Shift right button
          if (Incoming_value == '9') { //Reads when button is pressed
            remove_piece();
            shift_right++;
            if (collision() == true) {
              shift_right--;
              //Serial.println("right collision");
            }
            add_piece();
            print_map();
            delay(180);
          }

          //Rotate Button
          if (Incoming_value == '7') { //Reads when button is pressed
            remove_piece();
            rotate++;
            if (rotate > 3) {
              rotate = 0;
            }
            if (collision() == true) {
              rotate--;
              //Serial.println("rotate collision");
            }
            add_piece();
            print_map();
            delay(180);
          }
        }

        //-----------------Game-Mechanics----------------------
        if ((currentMillis - previousMillis) > interval) { //Timer used in replacement for delay, allowing game to run smoothly
          previousMillis = currentMillis;
          remove_piece(); //Removes piece so that there is no collision
          row_count++; //Moves pieces down by incrementing the row

          Serial.print(points); //Serial prints # of points which is read over app
          Serial.println(" points");
          piece_display();
          Serial.print("|");
          Serial.println();

          if (collision() == true) {

            game_over(); //Checks if game is over

            row_count--; // Stops the piece at the bottom
            add_piece(); // Sets piece to the tetris map
            complete_row(); // Checks and clears any full rows

            new_piece(); // Initializes new piece
            piece_id = random(4);

            row_count--; // Correct position for the piece

            if (game_over() == true) { //Game Over Event
              ending();
              delay(1000);
              //Highscore Tracker
              if (points > EEPROM.read(0)) {
                state = HIGHSCORE;
                highscoreCase = true; //This boolean variable is used in HIGHSCORE state
                placement = 0; //Placement value determines where values will be placed on the EEPROM
                for (int i = 1; i <= 4; i++) { //Bumps highscore placements down
                  EEPROM.write(7 + i, EEPROM.read(3 + i));
                  EEPROM.write(3 + i, EEPROM.read(i - 1));
                }
              }
              else if (points > EEPROM.read(4)) {
                state = HIGHSCORE;
                highscoreCase = true;
                placement = 4;
                for (int i = 1; i <= 4; i++) { //Bumps highscore placements down
                  EEPROM.write(7 + i, EEPROM.read(3 + i));
                }
              }
              else if (points > EEPROM.read(8)) {
                state = HIGHSCORE;
                highscoreCase = true;
                placement = 8;
              } else {
                Serial.print("Press Start");
                Serial.print("|");
                Serial.println();
                state = REST;
                points = 0;
              }
            }
            
          } else {
            add_piece();
            wdt_reset();
          }
          print_map();
        }
      }
      break;

    case HIGHSCORE:
      {
        wdt_reset();
        if (highscoreCase == true) { //This if statement ensures the highscore code is only sent once
          Serial.print(highscoreCode); //Serial prints a code which is parsed by app and initiates highscore event
          Serial.print("|");
          Serial.print(highscoreCode2);
          Serial.println();
          highscoreCase = false;
          delay(1000);
          Serial.print("Press Start");
          Serial.print("|");
          Serial.println();
        }
        if (Serial.available() > 0) { //Reads initials typed by user on app
          for (int i = 0; i < 3; i++) {
            initialValue[i] = Serial.read();
            initials[i] = initialValue[i]; //Collects each initial
            delay(50);
          } //Adds Highscore with initials
          EEPROM.write(placement, points);
          EEPROM.write(placement + 1, initials[0]);
          EEPROM.write(placement + 2, initials[1]);
          EEPROM.write(placement + 3, initials[2]);
          points = 0;
          state = REST;
        }
        wdt_reset();
        break;
      }
  }
}

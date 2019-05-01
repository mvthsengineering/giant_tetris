#include "Tetris.h"
Tetris tetris;

// ADC clock configurations
#define ADPS_16 _BV(ADPS2)
#define ADPS_32 _BV(ADPS2)|_BV(ADPS0)
#define ADPS_128 _BV(ADPS2)|_BV(ADPS1)|_BV(ADPS0)

#define PIN_LEFT  2
#define PIN_RIGHT 4
#define PIN_ROT   3

#define PIN_LEFT_LIT  5
#define PIN_RIGHT_LIT 7
#define PIN_ROT_LIT   6

// current and previous button states
uint8_t leftc;  uint8_t leftp  = LOW;
uint8_t rightc; uint8_t rightp = LOW;
uint8_t rotc;   uint8_t rotp   = LOW;
void buttons()
{
	leftcur =  digitalRead( TET_PIN_LEFT  );
	rightcur = digitalRead( TET_PIN_RIGHT );
	rotcur =   digitalRead( TET_PIN_ROT   );

	/* light buttons */
	if ( leftcur  ) digitalWrite( TET_PIN_LEFT_LIT,  HIGH );
	else digitalWrite( TET_PIN_LEFT_LIT,  LOW );
	if ( rightcur ) digitalWrite( TET_PIN_RIGHT_LIT, HIGH );
	else digitalWrite( TET_PIN_RIGHT_LIT, LOW );
	if ( rotcur   ) digitalWrite( TET_PIN_ROT_LIT,   HIGH );
	else digitalWrite( TET_PIN_ROT_LIT,   LOW );

	if ( leftcur  && !leftprev  ) tetris.movePiece( -1, 0 ); // move left
	leftprev = leftcur;
	if ( rightcur && !rightprev ) tetris.movePiece(  1, 0 ); // move right	
	rightprev = rightcur;
	if ( rotcur   && !rotprev   ) tetris.rotatePiece();	 // rotate
	rotprev = rotcur;
}

void copyBuffer( uint8_t * b )
{
}

ISR( TIMER1_OVF_vect )
{
	buttons();
	// TODO: COPY & DISPLAY THIS BUFFER
	copyBuffer( tetris.getActiveBoard() );
}

void setup()
{
	pinMode( TET_PIN_LEFT , INPUT );
	pinMode( TET_PIN_RIGHT, INPUT );
	pinMode( TET_PIN_ROT  , INPUT );
	pinMode( TET_PIN_LEFT_LIT , OUTPUT );
	pinMode( TET_PIN_RIGHT_LIT, OUTPUT );
	pinMode( TET_PIN_ROT_LIT  , OUTPUT );

	randomSeed(analogRead(A0));
	for ( uint8_t i = 0; i < 5; i++ ) random(); // burn off the first few bad random values

	tetris.init();

	ADCSRA &= ~ADPS_128; // clear ADPS
	ADCSRA |= ADPS_32;   // set division factor

	// Timer 1 interrupt setup
	TCNT1 = 0; // reset counter
	ICR1 = 19999; // top value of counter to achieve 50 Hz
	TCCR1A = 0; // clear unused register
	TCCR1B = _BV(WGM13) | _BV(CS11); // mode 8 (PWM), prescaler = 8
	TIMSK1 = _BV(TOIE1); // enable overflow interrupt

}
void loop()
{
	tetris.run();
}

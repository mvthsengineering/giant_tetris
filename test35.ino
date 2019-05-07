#include <Wire.h>
#include <SparkFunSX1509.h>
#include <string.h> // memset

SX1509 io0; SX1509 io1; SX1509 io2;
void setup() 
{
	// if we fail to communicate, halt
	if (!io0.begin(0x3E) || !io1.begin(0x3F) || !io2.begin(0x70)) while ( 1 );
  
  	// set all pins to output
  	for ( unsigned int i = 0; i < 16; i += 1 )
	{
    		io0.pinMode(i, OUTPUT);
    		io1.pinMode(i, OUTPUT);
    		io2.pinMode(i, OUTPUT);
	}
}

uint8_t dmap[ 35 ];
void mapdisplay()
{
  	for ( unsigned int i = 0; i < 16; i += 1 )
  	{
  		io0.digitalWrite( i, dmap[ i ] 			  );
		io1.digitalWrite( i, dmap[ i + 16 ] 		  );
		io2.digitalWrite( i, i > 3 ? LOW : dmap[ i + 32 ] );
	}
}

unsigned int iter = 0;
void loop() 
{
	memset( dmap, 0x00, 35 ); // clear display
	dmap[ iter % 35 ] = HIGH;
	mapdisplay();
	delay( 50 );
	iter++;
}

#include <Wire.h>
#include <SparkFunSX1509.h>
#include <string.h> // memset

SX1509 io0; SX1509 io1; SX1509 io2;
void setup() 
{
	if (!io0.begin(0x3E) || !io1.begin(0x3F) || !io2.begin(0x70))
	{
		while ( 1 ); // If we fail to communicate, loop forever.
	}
  
  	// set all pins to output
  	for ( unsigned int i = 0; i < 16; i += 1 )
	{
    		io0.pinMode(i, OUTPUT);
    		io1.pinMode(i, OUTPUT);
    		io2.pinMode(i, OUTPUT);
	}
}

uint8_t map[ 35 ];
void mapdisplay()
{
  	for ( unsigned int i = 0; i < 16; i += 1 )
  	{
  		io0.digitalWrite( i, map[ i ] 			   );
		io1.digitalWrite( i, map[ i + 16 ] 		   );
		io2.digitalWrite( i, i >= 35 ? LOW : map[ i + 32 ] );
	}
}

unsigned int itr = 0;
void loop() 
{
	memset( map, 0x00, 35 ); // clear display
	map[ iter % 35 ] = HIGH;
	mapdisplay();
	delay( 50 );
	iter++;
}

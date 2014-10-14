//
// Hi-Tech Lite include file for AVR board:
// using an (external) RFM70 transciever
//
// Loosely based on the example application provided by HopeRF
//
//
//
// (c) Wouter van Ooijen - wouter@voti.nl
//
// Copying and distribution of this file, with or without modification,
// are permitted in any medium without royalty provided the copyright
// notice and this notice are preserved.  This file is offered as-is,
// without any warranty.
//

#ifndef _AVR_RFM70_H_
#define _AVR_RFM70_H_

#include <avr/io.h>
#include <util/delay.h>

#define PIN_COPY(port, pin, val)	( (val==0) ? (port &= ~(1<<pin)) : (port |= (1<<pin))  )

#if defined(__AVR_ATtiny84__)

#define RFM70_SCK( x )   PIN_COPY( PORTA, PA3, (x) ) 
#define RFM70_MOSI( x )  PIN_COPY( PORTA, PA4, (x) )
#define RFM70_MISO       ( (PINA & (1<<PINA5)) >> PINA5 )
#define RFM70_CSN( x )   PIN_COPY( PORTA, PA2, (x) )
#define RFM70_CE( x )    PIN_COPY( PORTA, PA1, (x) )

#define RFM70_PIN_DIRECTION { \
   DDRA = 0x1E; \
}

#else

#define RFM70_SCK( x )   PIN_COPY( PORTC, PC3, (x) ) 
#define RFM70_MOSI( x )  PIN_COPY( PORTC, PC2, (x) )
#define RFM70_MISO       ( (PINC & (1<<PINC1)) >> PINC1 )
#define RFM70_CSN( x )   PIN_COPY( PORTC, PC4, (x) )
#define RFM70_CE( x )    PIN_COPY( PORTC, PC5, (x) )

#define RFM70_PIN_DIRECTION { \
   DDRC = 0x3C; \
}

#endif

#define RFM70_WAIT_US( x ) _delay_us( x )
#define RFM70_WAIT_MS( x ) _delay_ms( x )

#include "rfm70.h"

#endif

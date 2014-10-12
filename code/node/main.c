#define F_CPU 1000000L
#define BAUD 9600UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "rfm70-config.h"

unsigned char command = 0x00;


int main (void) {
	DDRC = 0xFF;
	PORTC = 0x00;
//	uart_init();
	DDRD = 0xFF;
	PORTD = 0xFF;

	unsigned char len, i, chksum, pipe;
	unsigned char rx_buf[32];
	unsigned char tx_buf[17]={
		0x01,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,
		0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,0x78};

	rfm70_init();
	_delay_ms(2000);

	//rfm70_mode_transmit();

	i = 0;

	while(1) {
		// receiver
		_delay_ms(20);
		if( rfm70_receive( &pipe, &rx_buf, &len )){
			PORTD = rx_buf[0];
			rfm70_register_write( RFM70_CMD_FLUSH_RX,0 );
		}

		/*
		// transmitter
		_delay_ms(500);
		tx_buf[0] = i++;
		rfm70_transmit_message_once( tx_buf, 17 );
		PORTD = 0x00;
		_delay_ms(500);
		PORTD = 0xFF;
		*/

	}
   
	return 0;
}

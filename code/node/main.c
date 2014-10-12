#define F_CPU 1000000L
#define BAUD 9600UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "rfm70-config.h"

unsigned char command = 0x00;


float vcc;//variable to hold the value of Vcc

void read_vcc(void)
{
	ADMUX = 0xE; //Set the Band Gap voltage as the ADC input
	ADCSRA = (1<<ADEN)|(1<<ADIE)|(1<<ADSC)|5;
}

ISR(ADC_vect) //ADC End of Conversion interrupt 
{
	unsigned char adc_data;
	adc_data = ADC>>2; //read 8 bit value
	vcc = 1.2 * 255 / adc_data * 10;
}

int main (void) {
	DDRC = 0xFF;
	PORTC = 0x00;
	DDRD = 0xFF;
	PORTD = 0xFF;

	unsigned char len, i, chksum, pipe;
	unsigned char rx_buf[32];
	unsigned char tx_buf[17]={
		0x01,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,
		0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,0x78};

	sei(); //Activate interrupts

	rfm70_init();
	_delay_ms(2000);

	rfm70_mode_transmit();

	i = 0;

	while(1) {
		read_vcc(); //setup the ADC
		// transmitter
		_delay_ms(500);
		tx_buf[0] = i++;
		tx_buf[1] = vcc;
		rfm70_transmit_message_once( tx_buf, 2 );
		PORTD = 0x00;
		_delay_ms(500);
		PORTD = 0xFF;
	}
   
	return 0;
}

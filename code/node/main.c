#define F_CPU 1000000L
#define BAUD 9600UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "rfm70-config.h"
#include "aes.h"

/* a sample key, key must be located in RAM */
uint8_t key[]  = { 0x01, 0x23, 0x45, 0x67,
                   0x89, 0xAB, 0xCD, 0xEF,
                   0x01, 0x23, 0x45, 0x67,
                   0x89, 0xAB, 0xCD, 0xEF };
/* sample data, you can encrypt what you want but keep in mind that only 128 bits (not less not more) get encrypted*/
uint8_t data[] = { 0x01, 0x02, 0x03, 0x04,
                   0x05, 0x06, 0x07, 0x08,
                   0x09, 0x0A, 0x0B, 0x0C,
                   0x0D, 0x0E, 0x0F, 0x00 };

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
	DDRD = 0xFF;
	PORTD = 0xFF;

	uint8_t j;

	aes128_ctx_t ctx; /* the context where the round keys are stored */
	aes128_init(key, &ctx); /* generating the round keys from the 128 bit key */
	sei(); //Activate interrupts

	rfm70_init();
	_delay_ms(2000);

	rfm70_mode_transmit();

	j = 0;

	while(1) {
		read_vcc(); //setup the ADC
		// transmitter
		_delay_ms(500);
		data[0] = j++;
		data[1] = vcc;
		for (uint8_t i=2;i<16;i++) {
			data[i] = 0;
		}
		aes128_enc(data, &ctx); /* encrypting the data block */
		rfm70_transmit_message_once( data, 16 );
		PORTD = 0x00;
		_delay_ms(500);
		PORTD = 0xFF;
	}
   
	return 0;
}

#define F_CPU 1000000L
#define BAUD 9600UL

#include <avr/sleep.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "rfm70-config.h"
#include "aes.h"

// Wake-up stuff
volatile uint8_t interrupt_flag = 0;

void wakeup_init(void) {
	OCR1A = 58593;
	// TCCR1B  = (1 << WGM12) | (1 << CS12) | (1 << CS10); // every minute
	TCCR1B  = (1 << WGM12) | (1 << CS11) | (1 << CS10);
	TIMSK = 1 << OCIE1A;
}

ISR(TIMER1_COMPA_vect) {
	interrupt_flag = 1;
}

// Encryption stuff
uint8_t key[]  = { 0x01, 0x23, 0x45, 0x67,
                   0x89, 0xAB, 0xCD, 0xEF,
                   0x01, 0x23, 0x45, 0x67,
                   0x89, 0xAB, 0xCD, 0xEF };
uint8_t data[16];


// Voltage stuff
float vcc;//variable to hold the value of Vcc
void read_vcc(void) {
	ADMUX = 0xE; //Set the Band Gap voltage as the ADC input
	ADCSRA = (1<<ADEN)|(1<<ADIE)|(1<<ADSC)|5;
}

//ADC End of Conversion interrupt 
ISR(ADC_vect) {
	unsigned char adc_data;
	adc_data = ADC>>2; //read 8 bit value
	vcc = 1.2 * 255 / adc_data * 10;
}

int main (void) {
	uint8_t j;

	aes128_ctx_t ctx; /* the context where the round keys are stored */
	aes128_init(key, &ctx); /* generating the round keys from the 128 bit key */

	wakeup_init();

	sei(); //Activate interrupts

	rfm70_init();

	j = 0;

	while(1) {
		set_sleep_mode(SLEEP_MODE_IDLE);
		sleep_mode();

		if (interrupt_flag == 1) {
			interrupt_flag = 0;
			rfm70_mode_transmit();
			_delay_ms(50);
			read_vcc(); //setup the ADC
			data[0] = j++;
			data[1] = vcc;
			for (uint8_t i=2;i<16;i++) {
				data[i] = 0;
			}
			aes128_enc(data, &ctx); /* encrypting the data block */
			rfm70_transmit_message( data, 16 );
			_delay_ms(100);
			rfm70_mode_powerdown();
		}
	}
   
	return 0;
}

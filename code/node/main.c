#define F_CPU 1000000L
#define BAUD 9600UL

#include <avr/sleep.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "rfm70-config.h"
#include "aes.h"
#include "../vars.h"

// Wake-up stuff
volatile uint8_t interrupt_flag = 0;

void wakeup_init(void) {
	OCR1A = 58593;
	TCCR1B  = (1 << WGM12) | (1 << CS11) | (1 << CS10);
	// TCCR1B  = (1 << WGM12) | (1 << CS12) | (1 << CS10); // every minute
#if defined(__AVR_ATtiny84__)
	TIMSK1  = 1 << OCIE1A;
#else
	TIMSK = 1 << OCIE1A;
#endif
}

#ifdef TIM1_COMPA_vect
ISR(TIM1_COMPA_vect) {
#else
ISR(TIMER1_COMPA_vect) {
#endif
	interrupt_flag = 1;
}


temp_package_t data;


// Voltage stuff
uint8_t vcc;//variable to hold the value of Vcc
void read_vcc(void) {
#if defined(__AVR_ATtiny84__)
	ADMUX = (1<<MUX5)|(1<<MUX0); //Set the Band Gap voltage as the ADC input
#else
	ADMUX = (1<<MUX3)|(1<<MUX2)|(1<<MUX1); //Set the Band Gap voltage as the ADC input
#endif
	ADCSRA = (1<<ADEN)|(1<<ADIE)|(1<<ADSC)|5;
}

//ADC End of Conversion interrupt 
ISR(ADC_vect) {
	unsigned char adc_data;
	adc_data = ADC>>2; //read 8 bit value
	vcc = 1.1 * 255.0 * VCC_SCALE_FACTOR / adc_data;
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
			rfm70_power(3);
			_delay_ms(50);
			read_vcc(); //setup the ADC
			data.info.type = 1;
			data.info.vcc = vcc;
			data.info.vcc_factor = VCC_SCALE_FACTOR;
			data.info.tempreture = 0;
			for (uint8_t i=0;i<11;i++) {
				data.info.data[i] = 0;
			}
			aes128_enc(data.raw, &ctx); /* encrypting the data block */
			rfm70_transmit_message( data.raw, 16 );
			_delay_ms(100);
			rfm70_mode_powerdown();
		}
	}
   
	return 0;
}

#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart.h"

void uart_init(void) {
	// enable uart
	UCSRB |= (1<<TXEN);                // UART TX einschalten
	UCSRC |= (1<<URSEL)|(3<<UCSZ0);    // Asynchron 8N1 

	UBRRH = UBRR_VAL >> 8;
	UBRRL = UBRR_VAL & 0xFF;

	UCSRB |= (1<<RXCIE) | (1<<RXEN);

	// enable interrupts
	sei();
}

int uart_putc(unsigned char c) {
	while (!(UCSRA & (1<<UDRE)))  /* warten bis Senden moeglich */
	{
	}
	UDR = c;                      /* sende Zeichen */
	return 0;
}

uint8_t uart_getc(void) {
	while (!(UCSRA & (1<<RXC)))   // warten bis Zeichen verfuegbar
		;
	return UDR;                   // Zeichen aus UDR an Aufrufer zurueckgeben
}

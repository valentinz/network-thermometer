#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "rfm70-config.h"
#include "uart.h"
#include "aes.h"

/* a sample key, key must be located in RAM */
uint8_t key[]  = { 0x01, 0x23, 0x45, 0x67,
                   0x89, 0xAB, 0xCD, 0xEF,
                   0x01, 0x23, 0x45, 0x67,
                   0x89, 0xAB, 0xCD, 0xEF };

int main (void) {
	uart_init();

	aes128_ctx_t ctx; /* the context where the round keys are stored */
	aes128_init(key, &ctx); /* generating the round keys from the 128 bit key */

	uint8_t len, i, pipe;
	uint8_t data[32];

	rfm70_init();
	_delay_ms(2000);

	while(1) {
		// receiver
		_delay_ms(200);
		if( rfm70_receive( &pipe, &data, &len )){
			aes128_dec(data, &ctx); /* decrypting the data block */
			for (i = 0; i < len; i++) {
				uart_putc(data[i]);
			}
			rfm70_register_write( RFM70_CMD_FLUSH_RX,0 );
		}
	}
   
	return 0;
}

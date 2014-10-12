#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "rfm70-config.h"
#include "uart.h"

int main (void) {
	uart_init();

	unsigned char len, i, chksum, pipe;
	unsigned char rx_buf[32];

	rfm70_init();
	_delay_ms(2000);

	while(1) {
		// receiver
		_delay_ms(2000);
		if( rfm70_receive( &pipe, &rx_buf, &len )){
			for (i = 0; i < len; i++) {
				uart_putc(rx_buf[i]);
			}
			rfm70_register_write( RFM70_CMD_FLUSH_RX,0 );
		}
	}
   
	return 0;
}

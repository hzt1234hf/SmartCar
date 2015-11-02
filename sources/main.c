/*
 * main implementation: use this sample to create your own application
 *
 */
#include "support_common.h" /* include peripheral declarations and more */
#if (CONSOLE_IO_SUPPORT || ENABLE_UART_SUPPORT) 
/* Standard IO is only possible if Console or UART support is enabled. */
#include <stdio.h>
#endif

int main(void)
{
	int counter = 0;
#if (CONSOLE_IO_SUPPORT || ENABLE_UART_SUPPORT) 
	printf("Hello World in C from MCF52259 derivative on MCF52259 board\n\r");
#endif
	for(;;) {	   
	   	counter++;
	}
}
	
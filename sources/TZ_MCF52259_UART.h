#ifndef TZ_MCF52259_UART
#define TZ_MCF52259_UART

extern void TUartx_INIT(uint8);
extern void TUart0_Putchar(char );
extern void TUart0_Puts(char *);
extern void TUart0_DMAPuts(char *,uint16 );
extern void TUart1_Putchar(char );
extern void TUart1_Puts(char *);
extern void TUart2_Putchar(char );
extern void TUart2_Puts(char *);

__declspec(interrupt) void UART0_DMA_interrupt(void);
__declspec(interrupt) void UART0_interrupt(void);
__declspec(interrupt) void UART1_interrupt(void);
__declspec(interrupt) void UART2_interrupt(void);

extern char TXBuffer[100];
extern char RXBuffer[100];
#endif
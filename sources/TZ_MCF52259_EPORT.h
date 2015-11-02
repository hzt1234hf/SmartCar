#ifndef TZ_MCF52259_EPORT
#define TZ_MCF52259_EPORT

extern void TEPORTx_Init(uint8);
__declspec(interrupt) void EPORT1_interrupt(void);
__declspec(interrupt) void EPORT3_interrupt(void);
__declspec(interrupt) void EPORT5_interrupt(void);
__declspec(interrupt) void EPORT7_interrupt(void);

extern vuint16 chang,hang;
extern vuint16 chang2,hang2;
extern vuint8 Cnt_HREF,Cnt_VSYN;
extern uint8 bool;

#endif
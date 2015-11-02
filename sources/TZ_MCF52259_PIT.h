
#ifndef TZ_MCF52259_PIT
#define TZ_MCF52259_PIT

#define TPIT0_SetPMR(x)     MCF_PIT0_PMR=(uint16)(19.531*x)//x的单位为ms
#define TPIT1_SetPMR(x)     MCF_PIT1_PMR=(uint16)(19.531*x)//x的单位为ms
#define TPIT0_DISABLE()     MCF_PIT0_PCSR &= ~MCF_PIT_PCSR_EN//关闭PIT0
#define TPIT1_DISABLE()     MCF_PIT1_PCSR &= ~MCF_PIT_PCSR_EN//关闭PIT1
#define TPIT0_ENABLE()      MCF_PIT0_PCSR |= MCF_PIT_PCSR_EN//打开PIT0
#define TPIT1_ENABLE()      MCF_PIT1_PCSR |= MCF_PIT_PCSR_EN//打开PIT1

extern void TPITx_Init(uint8 mode);
__declspec(interrupt:0) void TPIT0_interrupt(void);
__declspec(interrupt:0) void TPIT1_interrupt(void);
    
#endif
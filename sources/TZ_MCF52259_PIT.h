
#ifndef TZ_MCF52259_PIT
#define TZ_MCF52259_PIT

#define TPIT0_SetMode 2
#if(TPIT0_SetMode == 1)
    #define TPIT0_PRECLK 11
    #define TPIT0_ZQ 19.531
#elif (TPIT0_SetMode == 2)
    #define TPIT0_PRECLK 1
    #define TPIT0_ZQ 1
#endif


#define TPIT1_SetMode 2
#if(TPIT1_SetMode == 1)
    #define TPIT1_PRECLK 11
    #define TPIT1_ZQ 19.531
#elif (TPIT1_SetMode == 2)
    #define TPIT1_PRECLK 1
    #define TPIT1_ZQ 1
#endif

#define TPIT0_SetPMR(x)     MCF_PIT0_PMR=(uint16)(TPIT0_ZQ*x)//x�ĵ�λΪms
#define TPIT1_SetPMR(x)     MCF_PIT1_PMR=(uint16)(TPIT0_ZQ*x)//x�ĵ�λΪms
#define TPIT0_DISABLE()     MCF_PIT0_PCSR &= ~MCF_PIT_PCSR_EN//�ر�PIT0
#define TPIT1_DISABLE()     MCF_PIT1_PCSR &= ~MCF_PIT_PCSR_EN//�ر�PIT1
#define TPIT0_ENABLE()      MCF_PIT0_PCSR |= MCF_PIT_PCSR_EN//��PIT0
#define TPIT1_ENABLE()      MCF_PIT1_PCSR |= MCF_PIT_PCSR_EN//��PIT1


extern void TPITx_Init(uint8 mode);
__declspec(interrupt:0) void TPIT0_interrupt(void);
__declspec(interrupt:0) void TPIT1_interrupt(void);

#endif
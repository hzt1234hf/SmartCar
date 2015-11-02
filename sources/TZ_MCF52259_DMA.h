#ifndef TZ_MCF52259_DMA
#define TZ_MCF52259_DMA

#define SET_DMA0_DSR(x) MCF_DMA0_DAR = (vuint32)(x)
#define SET_DMA1_DSR(x) MCF_DMA1_DAR = (vuint32)(x)
#define SET_DMA2_DSR(x) MCF_DMA2_DAR = (vuint32)(x)
#define SET_DMA3_DSR(x) MCF_DMA3_DAR = (vuint32)(x)
#define SET_DMA0_SAR(x) MCF_DMA0_SAR = (vuint32)(x)
#define SET_DMA1_SAR(x) MCF_DMA1_SAR = (vuint32)(x)
#define SET_DMA2_SAR(x) MCF_DMA2_SAR = (vuint32)(x)
#define SET_DMA3_SAR(x) MCF_DMA3_SAR = (vuint32)(x)

extern void TDMAx_Init(uint8 mode);
__declspec(interrupt) void TDMA0_interrupt(void);
__declspec(interrupt) void TDMA1_interrupt(void);
__declspec(interrupt) void TDMA2_interrupt(void);
__declspec(interrupt) void TDMA3_interrupt(void);
__declspec(interrupt) void TDTIM0_interrupt(void);
__declspec(interrupt) void TDTIM1_interrupt(void);
__declspec(interrupt) void TDTIM2_interrupt(void);
__declspec(interrupt) void TDTIM3_interrupt(void);

extern uint8 aa1[12],aa2[5000],aa3,aa4[2];

#endif
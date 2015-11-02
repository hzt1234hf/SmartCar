#ifndef TZ_MCF52259_ADC
#define TZ_MCF52259_ADC

extern void TADCx_Init();
extern void TADC_Test(void);

__declspec(interrupt:0) void TADCA_interrupt(void);
__declspec(interrupt:0) void TADCB_interrupt(void);
__declspec(interrupt:0) void TADCINT_interrupt(void);

extern int16 ADC0_Data;
extern int16 ADC1_Data;
extern int16 ADC2_Data;
extern int16 ADC3_Data;
extern int16 ADC4_Data;
extern int16 ADC5_Data;
extern int16 ADC6_Data;
extern int16 ADC7_Data;

#endif
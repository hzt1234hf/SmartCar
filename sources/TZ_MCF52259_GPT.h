#ifndef TZ_MCF52259_GPT
#define TZ_MCF52259_GPT

#define TGPT0_ENINTER()   MCF_GPT_GPTIE |= MCF_GPT_GPTIE_CI0
#define TGPT1_ENINTER()   MCF_GPT_GPTIE |= MCF_GPT_GPTIE_CI1
#define TGPT3_ENINTER()   MCF_GPT_GPTIE |= MCF_GPT_GPTIE_CI3
#define TGPT0_DISINTER()   MCF_GPT_GPTIE &= ~MCF_GPT_GPTIE_CI0
#define TGPT1_DISINTER()   MCF_GPT_GPTIE &= ~MCF_GPT_GPTIE_CI1
#define TGPT3_DISINTER()   MCF_GPT_GPTIE &= ~MCF_GPT_GPTIE_CI3

extern void TGPTx_Init(uint8);
__declspec(interrupt) void TGPT0_interrupt(void);
__declspec(interrupt) void TGPT1_interrupt(void);
__declspec(interrupt) void TGPT3_interrupt(void);

extern uint16 cnt;

#endif
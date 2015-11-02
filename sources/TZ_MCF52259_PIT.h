
#ifndef TZ_MCF52259_PIT
#define TZ_MCF52259_PIT

#define TPIT0_SetMode 2
#if(TPIT0_SetMode == 1)
    #define TPIT0_PRECLK 12
    #define TPIT0_ZQ 15.625
#elif (TPIT0_SetMode == 2)
    #define TPIT0_PRECLK 1
    #define TPIT0_ZQ 1
#endif


#define TPIT1_SetMode 2
#if(TPIT1_SetMode == 1)
    #define TPIT1_PRECLK 12
    #define TPIT1_ZQ 15.625
#elif (TPIT1_SetMode == 2)
    #define TPIT1_PRECLK 1
    #define TPIT1_ZQ 1
#endif

#define TPIT0_SetPMR(x)     MCF_PIT0_PMR=(uint16)(TPIT0_ZQ*x)//x的单位为ms
#define TPIT1_SetPMR(x)     MCF_PIT1_PMR=(uint16)(TPIT0_ZQ*x)//x的单位为ms
#define TPIT0_DISABLE()     MCF_PIT0_PCSR &= ~MCF_PIT_PCSR_EN//关闭PIT0
#define TPIT1_DISABLE()     MCF_PIT1_PCSR &= ~MCF_PIT_PCSR_EN//关闭PIT1
#define TPIT0_ENABLE()      MCF_PIT0_PCSR |= MCF_PIT_PCSR_EN//打开PIT0
#define TPIT1_ENABLE()      MCF_PIT1_PCSR |= MCF_PIT_PCSR_EN//打开PIT1


extern void TPITx_Init(uint8 mode);
__declspec(interrupt:0) void TPIT0_interrupt(void);
__declspec(interrupt:0) void TPIT1_interrupt(void);
extern uint16 sqrt_16(unsigned int M);
extern int16 invSqrt(int a,int b);
extern int length(int a,int b);

extern int SetSpeed;             //定义设定值

extern float Kp,Ki,Kd;             //定义比例、积分、微分系数
extern float Kp_a,Ki_a,Kd_a;       //定义比例、积分、微分系数
extern float Kp_ab,Ki_ab,Kd_ab;       //基础比例、积分、微分系数
extern float Kp_ao,Ki_ao,Kd_ao;       //偏移比例、积分、微分系数


#endif
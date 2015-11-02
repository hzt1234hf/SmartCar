#ifndef TZ_MCF52259_PWM
#define TZ_MCF52259_PWM

#define TPWM0_SetDTY(x)     MCF_PWM_PWMDTY0=x
#define TPWM1_SetDTY(x)     MCF_PWM_PWMDTY1=x
#define TPWM45_SetDTY(x)    MCF_PWM_PWMDTY4 = (uint8)(x>>8),MCF_PWM_PWMDTY5 = (uint8)(x&0xff)

extern void TPWMx_INIT(uint8 mode);

#endif
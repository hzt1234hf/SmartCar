#ifndef TZ_MCF52259_PWM
#define TZ_MCF52259_PWM

#define TPWM0_SetDTY(x)     MCF_PWM_PWMDTY0=x
#define TPWM2_SetDTY(x)     MCF_PWM_PWMDTY2=x

#define LEFT_MOTOR_PWM(x)   TPWM2_SetDTY(x)     //�����ٶ�
#define RIGHT_MOTOR_PWM(x)   TPWM0_SetDTY(x)    //�ҵ���ٶ�

#define TPWM45_SetDTY(x)    MCF_PWM_PWMDTY4 = (uint8)(x>>8),MCF_PWM_PWMDTY5 = (uint8)(x&0xff)

#define STEER_PWM(x)        TPWM45_SetDTY(x)    //����ٶ�

extern void TPWMx_INIT(uint8 mode);

#endif
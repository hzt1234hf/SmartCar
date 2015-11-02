#include "support_common.h" /* include peripheral declarations and more */
#if (CONSOLE_IO_SUPPORT || ENABLE_UART_SUPPORT)
/* Standard IO is only possible if Console or UART support is enabled. */
#include <stdio.h>
#endif

#define DisableInterrupts() asm(move.w #0x2700,sr)
#define EnableInterrupts() asm(move.w #0x2000,sr)
//#define EnableInterrupts() asm { move.w SR,D0; andi.l #0xF8FF,D0; move.w D0,SR;  }

extern uint32 sysOsciFre;

#include "stdio.h"
#include "TZ_CAMERA_VAR.h"
#include "TZ_CONTROL_VAR.h"

#include "TZ_MCF52259_UART.h"
#include "TZ_MCF52259_PWM.h"
#include "DIY_FUNCTION.h"
#include "TZ_MCF52259_DMA.h"
#include "TZ_MCF52259_PIT.h"
#include "TZ_MCF52259_GPT.h"
#include "TZ_MCF52259_EPORT.h"
#include "TZ_MCF52259_USB.h"
#include "TZ_MCF52259_I2C.h"
#include "TZ_MCF52259_Delay.h"
#include "TZ_CAMERA_SCCB.h"
#include "TZ_CAMERA_CONFIG.h"
#include "TZ_MCF52259_ADC.h"


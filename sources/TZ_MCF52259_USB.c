#include "DIY_ComDef.h"
#ifdef TZ_MCF52259_USB

void TUSBx_Init(){
    MCF_USB_OTG_CTL |= MCF_USB_OTG_CTL_HOST_MODE_EN;
    
    MCF_USB_OTG_SOF_THLD = 12000;
}

#endif
#include "DIY_ComDef.h"
#ifdef TZ_MCF52259_DELAY
//1个nop是一个指令周期
void Delay1us(uint16 cnt){
    while(cnt--){
    asm(
        nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;
        nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;
        nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;
        nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;
        nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;
        nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;
        nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;
        nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;
        nop;nop;nop;nop;nop;nop;nop;nop;nop;nop;
        );
    }
}
void delay(void){
    uint32 i = 800000;
    while(i--){}
}
void delay2(void){
    uint32 i = 20000;
    while(i--){}
}
void delay3(void){
    uint32 i = 10000000;
    while(i--){}
}
#endif
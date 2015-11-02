#include "DIY_ComDef.h"
#ifdef TZ_MCF52259_EPORT
/*
*****2015.4.26 EPORT模块编写完毕
*****功能：EPORT模块主要用于外部中断
*****编写中遇到的主要问题：（
    1.注意GPIO配置
    2.注意边沿设置，是上升沿还是等等。。
）
*/

vuint16 chang,hang;
vuint16 chang2,hang2;
uint8 bool = 1;
vuint8 Cnt_HREF = 0,Cnt_VSYN = 0;
uint8 abc = 0x55;

void TEPORTx_Init(uint8 mode){
    switch(mode){
        case 1:{
            MCF_GPIO_PNQPAR |= MCF_GPIO_PNQPAR_IRQ1_IRQ1;               //使能引脚功能为外部中端引脚
            MCF_EPORT_EPPAR |= MCF_EPORT_EPPAR_EPPA1_RISING;            //设置上升沿触发
            MCF_EPORT_EPDDR &= ~MCF_EPORT_EPDDR_EPDD1;                  //设置为引脚输入
            MCF_EPORT_EPIER |= MCF_EPORT_EPIER_EPIE1;                   //时能中断

            MCF_EPORT_EPDR |= MCF_EPORT_EPDR_EPD1;
            //MCF_EPORT_EPFR |= MCF_EPORT_EPFR_EPF1;                    //设置输出值

            MCF_INTC0_IMRL &= ~MCF_INTC_IMRL_MASKALL;                   //开中断屏蔽
            MCF_INTC0_IMRL &= ~MCF_INTC_IMRL_INT_MASK1;
            MCF_INTC0_ICR01 = MCF_INTC_ICR_IP(3) |MCF_INTC_ICR_IL(1);   //设置中断优先级

        }break;
        case 3:{
            MCF_GPIO_PNQPAR |= MCF_GPIO_PNQPAR_IRQ3_IRQ3;               //使能引脚功能为外部中端引脚
            MCF_EPORT_EPPAR |= MCF_EPORT_EPPAR_EPPA3_RISING;            //设置上升沿触发
            MCF_EPORT_EPDDR &= ~MCF_EPORT_EPDDR_EPDD3;                  //设置为引脚输入
            MCF_EPORT_EPIER |= MCF_EPORT_EPIER_EPIE3;                   //时能中断

            MCF_EPORT_EPDR |= MCF_EPORT_EPDR_EPD3;
            //MCF_EPORT_EPFR |= MCF_EPORT_EPFR_EPF1;                    //设置输出值

            MCF_INTC0_IMRL &= ~MCF_INTC_IMRL_MASKALL;                   //开中断屏蔽
            MCF_INTC0_IMRL &= ~MCF_INTC_IMRL_INT_MASK3;
            MCF_INTC0_ICR03 = MCF_INTC_ICR_IP(5) |MCF_INTC_ICR_IL(3);   //设置中断优先级
        }break;
        case 5:{
            MCF_GPIO_PNQPAR |= MCF_GPIO_PNQPAR_IRQ5_IRQ5;               //使能引脚功能为外部中端引脚
            MCF_EPORT_EPPAR |= MCF_EPORT_EPPAR_EPPA5_RISING;            //设置上升沿触发
            MCF_EPORT_EPDDR &= ~MCF_EPORT_EPDDR_EPDD5;                  //设置为引脚输入
            MCF_EPORT_EPIER |= MCF_EPORT_EPIER_EPIE5;                   //时能中断

            MCF_EPORT_EPDR |= MCF_EPORT_EPDR_EPD5;
            //MCF_EPORT_EPFR |= MCF_EPORT_EPFR_EPF1;                    //设置输出值

            MCF_INTC0_IMRL &= ~MCF_INTC_IMRL_MASKALL;                   //开中断屏蔽
            MCF_INTC0_IMRL &= ~MCF_INTC_IMRL_INT_MASK5;
            MCF_INTC0_ICR05 = MCF_INTC_ICR_IP(5) |MCF_INTC_ICR_IL(3);   //设置中断优先级
        }break;
        case 7:{
            MCF_GPIO_PNQPAR |= MCF_GPIO_PNQPAR_IRQ7_IRQ7;               //使能引脚功能为外部中端引脚
            MCF_EPORT_EPPAR |= MCF_EPORT_EPPAR_EPPA7_RISING;            //设置上升沿触发
            MCF_EPORT_EPDDR &= ~MCF_EPORT_EPDDR_EPDD7;                  //设置为引脚输入
            //MCF_EPORT_EPIER |= MCF_EPORT_EPIER_EPIE7;                   //时能中断

            MCF_EPORT_EPDR |= MCF_EPORT_EPDR_EPD7;
            //MCF_EPORT_EPFR |= MCF_EPORT_EPFR_EPF1;                    //设置输出值

            MCF_INTC0_IMRL &= ~MCF_INTC_IMRL_MASKALL;                   //开中断屏蔽
            MCF_INTC0_IMRL &= ~MCF_INTC_IMRL_INT_MASK7;
            MCF_INTC0_ICR07 = MCF_INTC_ICR_IP(3) |MCF_INTC_ICR_IL(3);   //设置中断优先级
        }break;
    }
}
__declspec(interrupt) void EPORT1_interrupt(void){
    MCF_EPORT_EPFR |= MCF_EPORT_EPFR_EPF1;    //清中断标志位
    chang++;
    Cnt_HREF = 0;
#if 0
    if((chang%10)==1){
        MCF_EPORT_EPIER &= ~MCF_EPORT_EPIER_EPIE7;                  //关闭中断
    }else if((chang%10)==2){
        TPIT0_ENABLE();
    }else{
        chang2++;
        MCF_DMA3_SAR = (vuint32)0x40100030;

        while(Cnt_HREF<=60){

            MCF_DMA3_DSR |= MCF_DMA_DSR_DONE;//清除中断标志位
            MCF_DMA3_DAR = (vuint32)(&Image[Cnt_HREF][0]);
            MCF_DMA3_BCR = CAMERA_W_8;
            MCF_DTIM3_DTMR |= MCF_DTIM_DTMR_RST;

            MCF_DMA3_DCR |=  MCF_DMA_DCR_EEXT;
            Cnt_HREF++;
            while(!(MCF_EPORT_EPFR&MCF_EPORT_EPFR_EPF7))
            MCF_EPORT_EPFR |= MCF_EPORT_EPFR_EPF7;    //清中断标志位
        }
    }
#elif 0
    if((chang%10)==2){
        TPIT0_ENABLE();
    }else if((chang%10)==0){
        chang2++;
        MCF_DMA3_SAR = (vuint32)0x40100030;

        while(Cnt_HREF<=60){

            MCF_DMA3_DSR |= MCF_DMA_DSR_DONE;//清除中断标志位
            MCF_DMA3_DAR = (vuint32)(&Image[Cnt_HREF][0]);
            MCF_DMA3_BCR = CAMERA_W_8;
            MCF_DTIM3_DTMR |= MCF_DTIM_DTMR_RST;

            MCF_DMA3_DCR |=  MCF_DMA_DCR_EEXT;
            Cnt_HREF++;
            while(!(MCF_EPORT_EPFR&MCF_EPORT_EPFR_EPF7))
            MCF_EPORT_EPFR |= MCF_EPORT_EPFR_EPF7;    //清中断标志位
        }
        //MCF_EPORT_EPIER |= MCF_EPORT_EPIER_EPIE7;                   //时能中断
    }
#else
    if((chang%10)==2){
        TPIT0_ENABLE();
    }else if((chang%10)==0){
        chang2++;
        MCF_DMA3_SAR = (vuint32)0x40100030;
        MCF_EPORT_EPIER |= MCF_EPORT_EPIER_EPIE7;                   //时能中断
    }else{
        MCF_EPORT_EPIER &= ~MCF_EPORT_EPIER_EPIE7;                  //关闭中断
    }
#endif
}
__declspec(interrupt) void EPORT3_interrupt(void){
    MCF_EPORT_EPFR |= MCF_EPORT_EPFR_EPF3;    //清中断标志位
}
__declspec(interrupt) void EPORT5_interrupt(void){
    MCF_EPORT_EPFR |= MCF_EPORT_EPFR_EPF5;    //清中断标志位
}
__declspec(interrupt) void EPORT7_interrupt(void){
#if 0
    if(Cnt_HREF%2){
        MCF_DMA3_DAR = (vuint32)(&Image[Cnt_HREF/2][0]);
        MCF_DMA3_BCR = CAMERA_W_8 + CAMERA_OFFSET_8+1;
        MCF_DMA3_DSR |= MCF_DMA_DSR_DONE;//清除中断标志位
        //TUart0_Putchar('G');
    }else{
        MCF_DMA3_DCR |=  MCF_DMA_DCR_EEXT;
        MCF_DTIM3_DTMR |= MCF_DTIM_DTMR_RST;
        //TUart0_Putchar('H');
    }
    Cnt_HREF++;
#else
    MCF_DMA3_DSR |= MCF_DMA_DSR_DONE;       //清除中断标志位
    MCF_DMA3_DAR = (vuint32)(&Image[Cnt_HREF][0]);
    MCF_DMA3_BCR = CAMERA_W_8;
    MCF_DMA3_DCR |=  MCF_DMA_DCR_EEXT;
    MCF_DTIM3_DTMR |= MCF_DTIM_DTMR_RST;
    Cnt_HREF++;
#endif
    hang ++;
    MCF_EPORT_EPFR |= MCF_EPORT_EPFR_EPF7;    //清中断标志位
}
#endif
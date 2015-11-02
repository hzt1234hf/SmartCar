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
vuint8 Cnt_Output = 0;

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
            MCF_INTC0_ICR01 = MCF_INTC_ICR_IP(3) |MCF_INTC_ICR_IL(3);   //设置中断优先级

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
            MCF_EPORT_EPIER |= MCF_EPORT_EPIER_EPIE7;                   //时能中断

            MCF_EPORT_EPDR |= MCF_EPORT_EPDR_EPD7;
            //MCF_EPORT_EPFR |= MCF_EPORT_EPFR_EPF1;                    //设置输出值

            MCF_INTC0_IMRL &= ~MCF_INTC_IMRL_MASKALL;                   //开中断屏蔽
            MCF_INTC0_IMRL &= ~MCF_INTC_IMRL_INT_MASK7;
            MCF_INTC0_ICR07 = MCF_INTC_ICR_IP(3) |MCF_INTC_ICR_IL(1);   //设置中断优先级
        }break;
    }
}
__declspec(interrupt) void EPORT7_interrupt(void){
    chang++;
#if 0
//轮训部分采集
    if((chang%25)==2){
        TPIT0_ENABLE();
    }else if((chang%5)==0){
        chang2++;
        MCF_DMA3_SAR = (vuint32)0x40100030;

        while(Cnt_HREF<=120){

            MCF_DMA3_DSR |= MCF_DMA_DSR_DONE;//清除中断标志位
            MCF_DMA3_DAR = (vuint32)(&Image[Cnt_HREF][0]);
            MCF_DMA3_BCR = CAMERA_W_8;
            MCF_DTIM3_DTMR |= MCF_DTIM_DTMR_RST;
            hang2++;
            while(!(MCF_EPORT_EPFR&MCF_EPORT_EPFR_EPF7))
            MCF_DMA3_DCR |=  MCF_DMA_DCR_EEXT;
            Cnt_HREF++;
            MCF_EPORT_EPFR |= MCF_EPORT_EPFR_EPF7;    //清中断标志位
        }
    }
#elif 1
    switch(Cnt_VSYN){
        case 1:{
            MCF_DMA3_DSR |= MCF_DMA_DSR_DONE;       //清除中断标志位
            MCF_DMA3_DAR = (vuint32)&Image[0];      //重设数组地址
            MCF_DMA3_BCR = IMG_SIZE;                //重设大小
        }break;
        case 2:{
            MCF_DMA3_DCR |=  MCF_DMA_DCR_EEXT;      //开启一次场采集
        }break;
        case 3:{
            bool = 1;
            TPIT1_ENABLE();
        }break;
        case 4:{
            //sprintf(TXBuffer,"%u-",bool);
            //TUart0_Puts(TXBuffer);
            //TPIT0_ENABLE();
        }break;
        case 6:{
            if(chang >= 50){
                showImg();
                chang = 0;
            }
            Cnt_VSYN = 0;
        }break;
    }
    Cnt_VSYN++;

#elif 0
    if(0 == (chang%150)){
        TPIT0_ENABLE();
        chang = 1;
    }else{
        MCF_DMA3_DSR |= MCF_DMA_DSR_DONE;       //清除中断标志位
        MCF_DMA3_DAR = (vuint32)(Image[0]);
        MCF_DMA3_BCR = IMG_SIZE;
        MCF_DMA3_DCR |=  MCF_DMA_DCR_EEXT;
    }
#elif 0
//行中断全部采集
    if((chang%30)==2){
        MCF_EPORT_EPIER &= ~MCF_EPORT_EPIER_EPIE7;                  //关闭中断
        TPIT0_ENABLE();
        MCF_DMA3_SAR = (vuint32)0x40100030;
        MCF_EPORT_EPIER |= MCF_EPORT_EPIER_EPIE7;                   //时能中断
    }
#elif 0
//行中断部分采集
    if((chang%30)==2){
        MCF_EPORT_EPIER &= ~MCF_EPORT_EPIER_EPIE7;                  //关闭中断
        TPIT0_ENABLE();
        MCF_DMA3_SAR = (vuint32)0x40100030;
    }else if((chang%5)==0){
        MCF_EPORT_EPIER |= MCF_EPORT_EPIER_EPIE7;                   //时能中断
    }else if((chang%5)==1){
        MCF_EPORT_EPIER &= ~MCF_EPORT_EPIER_EPIE7;                  //关闭中断
    }
#endif
    MCF_EPORT_EPFR |= MCF_EPORT_EPFR_EPF1;    //清中断标志位

}
__declspec(interrupt) void EPORT5_interrupt(void){
    MCF_EPORT_EPFR |= MCF_EPORT_EPFR_EPF5;    //清中断标志位
}
__declspec(interrupt) void EPORT3_interrupt(void){
    MCF_EPORT_EPFR |= MCF_EPORT_EPFR_EPF3;    //清中断标志位
}
__declspec(interrupt) void EPORT1_interrupt(void){
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
    //MCF_DTIM3_DTMR |= MCF_DTIM_DTMR_RST;
    Cnt_HREF++;
#endif
    hang ++;
    MCF_EPORT_EPFR |= MCF_EPORT_EPFR_EPF7;    //清中断标志位
}

void showImg(void){
#if 0
    //DMA自动输出
    TUart0_Putchar(0x01);
    TUart0_Putchar(0xFE);

    TUart0_DMAPutBuffer(Image[0],IMG_SIZE);

    TUart0_Putchar(0xFE);
    TUart0_Putchar(0x01);
#elif 1
    //DMA自动输出
    Image_ToPC[0] = 0x01;
    Image_ToPC[1] = 0xFE;
    Image_ToPC[IMG_SIZE+2] = 0xFE;
    Image_ToPC[IMG_SIZE+3] = 0x01;
    TUart0_DMAPutBuffer(Image_ToPC,IMG_SIZE+4);

#elif 0

    TUart0_Putchar(0);
    TUart0_Putchar(255);
    TUart0_Putchar(1);
    TUart0_Putchar(0);
    TUart0_DMAPutBuffer(Image[0],IMG_SIZE);

#elif 0       //循环串口输出
    uint8 a,b;
    TUart0_Putchar(0x01);
    TUart0_Putchar(0xFE);

    for(a = 0;a<CAMERA_H;a++){
        for(b = 0;b<CAMERA_W_8;b++)
            TUart0_Putchar(Image[a][b]);
        //TUart0_Putchar(Image[a][0]);

    }

    TUart0_Putchar(0xFE);
    TUart0_Putchar(0x01);
#endif

}
#endif
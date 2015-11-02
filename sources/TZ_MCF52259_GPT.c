#include "DIY_ComDef.h"
#ifdef TZ_MCF52259_GPT
/*
*****2015.4.25 GPT模块编写完毕
*****功能：本GPT模块主要用于脉冲捕获
*****编写中遇到的主要问题：（
    1.引脚要配置为输入捕获模式
    2.使用时要注意此脉冲捕获模块与一般芯片的不一样(第一次见这种功能的脉冲捕获)
    3.注意要设置合理的预分频
）
*/

void TGPTx_Init(uint8 mode){
    switch(mode){
        case 0:{
            MCF_GPIO_PTAPAR |= MCF_GPIO_PTAPAR_ICOC0_ICOC0;

            MCF_GPT_GPTIOS &= ~MCF_GPT_GPTIOS_IOS0;                 //设置IOCO0端口为输入捕获模式
            MCF_GPT_GPTSCR1 |= MCF_GPT_GPTSCR1_TFFCA;	            //设置自动清除中断标志
            MCF_GPT_GPTCTL2 |= MCF_GPT_GPTCTL2_INPUT0_RISING;       //设置为下降沿检测
            MCF_GPT_GPTIE |= MCF_GPT_GPTIE_CI0;                     //使能输入捕获端口0的中断
            /*           设置预分频 pre = sys/2/（2^x）,这里x为3*/
            MCF_GPT_GPTSCR2 |= MCF_GPT_GPTSCR2_PR(6);
            MCF_GPT_GPTFLG1 = 0;                                    //清零中断标志寄存器
            MCF_GPT_GPTDDR &= ~MCF_GPT_GPTDDR_DDRT0;                //清零来设置为输入模式

            MCF_INTC0_IMRH &=~ MCF_INTC_IMRH_INT_MASK44;                //设置中断源号为44，实际位置为44+64
            MCF_INTC0_ICR44 = MCF_INTC_ICR_IP(4) |MCF_INTC_ICR_IL(3);   //设置中断优先级

            MCF_GPT_GPTSCR1 |= MCF_GPT_GPTSCR1_GPTEN;                   //使能输入捕获0
        }break;
        case 1:{
            MCF_GPIO_PTAPAR |= MCF_GPIO_PTAPAR_ICOC1_ICOC1;

            MCF_GPT_GPTIOS &= ~MCF_GPT_GPTIOS_IOS1;                 //设置IOCO1端口为输入捕获模式
            MCF_GPT_GPTSCR1 |= MCF_GPT_GPTSCR1_TFFCA;	            //设置自动清除中断标志
            MCF_GPT_GPTCTL2 |= MCF_GPT_GPTCTL2_INPUT1_RISING;       //设置为下降沿检测
            MCF_GPT_GPTIE |= MCF_GPT_GPTIE_CI1;                     //使能输入捕获端口0的中断
            /*           设置预分频 pre = sys/2/（2^x）,这里x为3*/
            MCF_GPT_GPTSCR2 |= MCF_GPT_GPTSCR2_PR(6);
            MCF_GPT_GPTFLG1 = 0;                                    //清零中断标志寄存器
            MCF_GPT_GPTDDR &= ~MCF_GPT_GPTDDR_DDRT1;                //清零来设置为输入模式

            MCF_INTC0_IMRH &=~ MCF_INTC_IMRH_INT_MASK45;                //设置中断源号为44，实际位置为44+64
            MCF_INTC0_ICR45 = MCF_INTC_ICR_IP(4) |MCF_INTC_ICR_IL(3);   //设置中断优先级

            MCF_GPT_GPTSCR1 |= MCF_GPT_GPTSCR1_GPTEN;                   //使能输入捕获0
        }break;
        case 3:{
            MCF_GPIO_PTAPAR |= MCF_GPIO_PTAPAR_ICOC3_ICOC3;

            MCF_GPT_GPTIOS &= ~MCF_GPT_GPTIOS_IOS3;                 //设置IOCO3端口为输入捕获模式
            MCF_GPT_GPTSCR1 |= MCF_GPT_GPTSCR1_TFFCA;	            //设置自动清除中断标志
            MCF_GPT_GPTCTL2 |= MCF_GPT_GPTCTL2_INPUT0_RISING;       //设置为下降沿检测
            MCF_GPT_GPTIE |= MCF_GPT_GPTIE_CI1;                     //使能输入捕获端口0的中断
            /*           设置预分频 pre = sys/2/（2^x）,这里x为3*/
            MCF_GPT_GPTSCR2 |= MCF_GPT_GPTSCR2_PR(6);
            MCF_GPT_GPTFLG1 = 0;                                    //清零中断标志寄存器
            MCF_GPT_GPTDDR &= ~MCF_GPT_GPTDDR_DDRT3;                //清零来设置为输入模式

            MCF_INTC0_IMRH &=~ MCF_INTC_IMRH_INT_MASK47;                //设置中断源号为44，实际位置为44+64
            MCF_INTC0_ICR47 = MCF_INTC_ICR_IP(4) |MCF_INTC_ICR_IL(3);   //设置中断优先级

            MCF_GPT_GPTSCR1 |= MCF_GPT_GPTSCR1_GPTEN;                   //使能输入捕获0
        }break;
    }

}
__declspec(interrupt) void TGPT0_interrupt(void){

    //MCF_GPIO_PORTTF |= MCF_GPIO_PORTTF_PORTTF3;//置位CD4520 Reset，清零Q0-Q3

    leftMotorSpeed = MCF_GPT_GPTC0 - leftMSCnt;
    leftMSCnt = MCF_GPT_GPTC0;
    //MCF_GPT_GPTFLG1 |= MCF_GPT_GPTFLG1_CF0;

    //sprintf(TXBuffer,"left:%u,%u\n",leftMotorSpeed,leftMSCnt);
    //TUart0_Puts(TXBuffer);

    //MCF_GPIO_PORTTF &= !MCF_GPIO_PORTTF_PORTTF3;//清零CD4520 Reset，开始计数
}
__declspec(interrupt) void TGPT1_interrupt(void){

    //MCF_GPIO_PORTTF |= MCF_GPIO_PORTTF_PORTTF3;//置位CD4520 Reset，清零Q0-Q3

    rightMotorSpeed = MCF_GPT_GPTC1 - rightMSCnt;
    rightMSCnt = MCF_GPT_GPTC1;

    //MCF_GPT_GPTFLG1 |= MCF_GPT_GPTFLG1_CF1;

    //sprintf(TXBuffer,"right:%u,%u\n",rightMotorSpeed,rightMSCnt);
    //TUart0_Puts(TXBuffer);

    //MCF_GPIO_PORTTF &= !MCF_GPIO_PORTTF_PORTTF3;//清零CD4520 Reset，开始计数

}
__declspec(interrupt) void TGPT3_interrupt(void){
    //sprintf(TXBuffer,"%u\r\n",MCF_GPT_GPTC3);
    //TUart1_Puts(TXBuffer);
}
#endif
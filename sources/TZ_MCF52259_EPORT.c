#include "DIY_ComDef.h"
#ifdef TZ_MCF52259_EPORT
/*
*****2015.4.26 EPORTģ���д���
*****���ܣ�EPORTģ����Ҫ�����ⲿ�ж�
*****��д����������Ҫ���⣺��
    1.ע��GPIO����
    2.ע��������ã��������ػ��ǵȵȡ���
��
*/

vuint16 chang,hang;
vuint16 chang2,hang2;
uint8 bool = 1;
vuint16 Cnt_HREF = 0,Cnt_VSYN = 0;

void TEPORTx_Init(uint8 mode){
    switch(mode){
        case 1:{
            MCF_GPIO_PNQPAR |= MCF_GPIO_PNQPAR_IRQ1_IRQ1;               //ʹ�����Ź���Ϊ�ⲿ�ж�����
            MCF_EPORT_EPPAR |= MCF_EPORT_EPPAR_EPPA1_RISING;            //���������ش���
            MCF_EPORT_EPDDR &= ~MCF_EPORT_EPDDR_EPDD1;                  //����Ϊ��������
            MCF_EPORT_EPIER |= MCF_EPORT_EPIER_EPIE1;                   //ʱ���ж�

            MCF_EPORT_EPDR |= MCF_EPORT_EPDR_EPD1;
            //MCF_EPORT_EPFR |= MCF_EPORT_EPFR_EPF1;                    //�������ֵ

            MCF_INTC0_IMRL &= ~MCF_INTC_IMRL_MASKALL;                   //���ж�����
            MCF_INTC0_IMRL &= ~MCF_INTC_IMRL_INT_MASK1;
            MCF_INTC0_ICR01 = MCF_INTC_ICR_IP(3) |MCF_INTC_ICR_IL(1);   //�����ж����ȼ�

        }break;
        case 3:{
            MCF_GPIO_PNQPAR |= MCF_GPIO_PNQPAR_IRQ3_IRQ3;               //ʹ�����Ź���Ϊ�ⲿ�ж�����
            MCF_EPORT_EPPAR |= MCF_EPORT_EPPAR_EPPA3_RISING;            //���������ش���
            MCF_EPORT_EPDDR &= ~MCF_EPORT_EPDDR_EPDD3;                  //����Ϊ��������
            MCF_EPORT_EPIER |= MCF_EPORT_EPIER_EPIE3;                   //ʱ���ж�

            MCF_EPORT_EPDR |= MCF_EPORT_EPDR_EPD3;
            //MCF_EPORT_EPFR |= MCF_EPORT_EPFR_EPF1;                    //�������ֵ

            MCF_INTC0_IMRL &= ~MCF_INTC_IMRL_MASKALL;                   //���ж�����
            MCF_INTC0_IMRL &= ~MCF_INTC_IMRL_INT_MASK3;
            MCF_INTC0_ICR03 = MCF_INTC_ICR_IP(5) |MCF_INTC_ICR_IL(3);   //�����ж����ȼ�
        }break;
        case 5:{
            MCF_GPIO_PNQPAR |= MCF_GPIO_PNQPAR_IRQ5_IRQ5;               //ʹ�����Ź���Ϊ�ⲿ�ж�����
            MCF_EPORT_EPPAR |= MCF_EPORT_EPPAR_EPPA5_RISING;            //���������ش���
            MCF_EPORT_EPDDR &= ~MCF_EPORT_EPDDR_EPDD5;                  //����Ϊ��������
            MCF_EPORT_EPIER |= MCF_EPORT_EPIER_EPIE5;                   //ʱ���ж�

            MCF_EPORT_EPDR |= MCF_EPORT_EPDR_EPD5;
            //MCF_EPORT_EPFR |= MCF_EPORT_EPFR_EPF1;                    //�������ֵ

            MCF_INTC0_IMRL &= ~MCF_INTC_IMRL_MASKALL;                   //���ж�����
            MCF_INTC0_IMRL &= ~MCF_INTC_IMRL_INT_MASK5;
            MCF_INTC0_ICR05 = MCF_INTC_ICR_IP(5) |MCF_INTC_ICR_IL(3);   //�����ж����ȼ�
        }break;
        case 7:{
            MCF_GPIO_PNQPAR |= MCF_GPIO_PNQPAR_IRQ7_IRQ7;               //ʹ�����Ź���Ϊ�ⲿ�ж�����
            MCF_EPORT_EPPAR |= MCF_EPORT_EPPAR_EPPA7_RISING;            //���������ش���
            MCF_EPORT_EPDDR &= ~MCF_EPORT_EPDDR_EPDD7;                  //����Ϊ��������
            //MCF_EPORT_EPIER |= MCF_EPORT_EPIER_EPIE7;                   //ʱ���ж�

            MCF_EPORT_EPDR |= MCF_EPORT_EPDR_EPD7;
            //MCF_EPORT_EPFR |= MCF_EPORT_EPFR_EPF1;                    //�������ֵ

            MCF_INTC0_IMRL &= ~MCF_INTC_IMRL_MASKALL;                   //���ж�����
            MCF_INTC0_IMRL &= ~MCF_INTC_IMRL_INT_MASK7;
            MCF_INTC0_ICR07 = MCF_INTC_ICR_IP(3) |MCF_INTC_ICR_IL(3);   //�����ж����ȼ�
        }break;
    }
}
__declspec(interrupt) void EPORT1_interrupt(void){
    MCF_EPORT_EPFR |= MCF_EPORT_EPFR_EPF1;    //���жϱ�־λ
    chang++;
    if(!(chang%10)){
        TPIT0_ENABLE();
    }
    if(chang%10){
        MCF_EPORT_EPIER &= ~MCF_EPORT_EPIER_EPIE7;                  //�ر��ж�

    }else{
        chang2++;
        Cnt_HREF = 0;
        MCF_EPORT_EPIER |= MCF_EPORT_EPIER_EPIE7;                   //ʱ���ж�
    }
}
__declspec(interrupt) void EPORT3_interrupt(void){
    MCF_EPORT_EPFR |= MCF_EPORT_EPFR_EPF3;    //���жϱ�־λ
}
__declspec(interrupt) void EPORT5_interrupt(void){
    MCF_EPORT_EPFR |= MCF_EPORT_EPFR_EPF5;    //���жϱ�־λ
}
__declspec(interrupt) void EPORT7_interrupt(void){
    MCF_EPORT_EPFR |= MCF_EPORT_EPFR_EPF7;    //���жϱ�־λ
    hang ++;

    bool = 0;
    //MCF_DMA3_DAR = (vuint32)&aa4;
    Cnt_HREF++;
    //MCF_DMA3_BCR = 2;
    //MCF_DMA3_BCR = 10;
    //MCF_DMA3_DCR |=  MCF_DMA_DCR_EEXT | MCF_DMA_DCR_START;
    //MCF_DTIM3_DTMR |= MCF_DTIM_DTMR_RST;    //ʹ��DMA1��ʱ��
    //TUart0_Putchar('G');
    //MCF_DMA3_DCR |= MCF_DMA_DCR_START;
}
#endif
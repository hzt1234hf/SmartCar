#include "DIY_ComDef.h"
#ifdef TZ_MCF52259_PIT
/*
*****2015.4.21 PITģ���д���
*****���ܣ�PIT�ɱ�̶�ʱ��ģ����ж�ʱ
*****��д����������Ҫ���⣺��
    1.Ԥ��Ƶ����MCF_PIT0_PMR����ֵ��Ҫע�����
��
*/
uint8 bool2 = 1;
vuint8 cc1[8]={0},cc2[8] = {0};

void TPITx_Init(uint8 mode){
    switch(mode){
        default:case 0:{
            MCF_PIT0_PCSR = 0;
            MCF_PIT0_PCSR = MCF_PIT_PCSR_RLD | MCF_PIT_PCSR_PIF | MCF_PIT_PCSR_PIE | MCF_PIT_PCSR_OVW | MCF_PIT_PCSR_PRE(11);
            //                 �Զ�����         ����Ϊ0�ж�         ʹ���ж�            ��������PMRֵ       80000000 / 2^10 = 78125 2^11 = 39062 2^12=19531
            MCF_PIT0_PMR = 19531 * 1;//���ü���ֵ
        	MCF_INTC0_IMRH &=~ MCF_INTC_IMRH_INT_MASK55;                //�����ж�Դ��Ϊ55��ʵ��λ��Ϊ55+64
        	MCF_INTC0_ICR55 = MCF_INTC_ICR_IP(4) |MCF_INTC_ICR_IL(2);   //�����ж����ȼ�
            //MCF_PIT0_PCSR |= MCF_PIT_PCSR_EN;
        }break;
        case 1:{
            MCF_PIT1_PCSR = 0;
            MCF_PIT1_PCSR = MCF_PIT_PCSR_RLD | MCF_PIT_PCSR_PIF | MCF_PIT_PCSR_PIE | MCF_PIT_PCSR_OVW | MCF_PIT_PCSR_PRE(11);
            //                 �Զ�����         ����Ϊ0�ж�         ʹ���ж�            ��������PMRֵ       80000000 / 2^10 = 78125 2^11 = 39062 2^12=19531
            MCF_PIT1_PMR = 19531 * 1;//���ü���ֵ
        	MCF_INTC0_IMRH &=~ MCF_INTC_IMRH_INT_MASK56;                //�����ж�Դ��Ϊ55��ʵ��λ��Ϊ55+64
        	MCF_INTC0_ICR56 = MCF_INTC_ICR_IP(4) |MCF_INTC_ICR_IL(2);   //�����ж����ȼ�
            //MCF_PIT1_PCSR |= MCF_PIT_PCSR_EN;
        }break;
    }
}
__declspec(interrupt:0) void TPIT0_interrupt(void){
#if 1
    //uint8 a,b;
    TUart0_Putchar(0x01);
    TUart0_Putchar(0xFE);

    TUart0_DMAPutBuffer(Image[0],CAMERA_W_8*CAMERA_H);
    /*
    for(a = 0;a<CAMERA_H;a++){
        for(b = 0;b<CAMERA_W_8;b++)
            TUart0_Putchar(Image[a][b]);
        //TUart0_Putchar(Image[a][0]);

    }
    */

    TUart0_Putchar(0xFE);
    TUart0_Putchar(0x01);
#endif
    MCF_PIT0_PCSR &= ~MCF_PIT_PCSR_EN;
    MCF_PIT0_PCSR |= MCF_PIT_PCSR_PIF;  //���жϱ�־
    //TUart0_Puts("PIT0!\r\n");

}
__declspec(interrupt:0) void TPIT1_interrupt(void){
    uint16 a;
    cc1[0]++;
    //float ls,rs;
    MCF_PIT1_PCSR |= MCF_PIT_PCSR_PIF;  //���жϱ�־
/*

    MCF_DMA3_SAR = (vuint32)0x40100030;
    MCF_DMA3_DAR = (vuint32)cc2;
    MCF_DMA3_BCR = 4;
    sprintf(TXBuffer,"%u,%u,%u==",MCF_DMA3_SAR,MCF_DMA3_DAR,MCF_DMA3_BCR);
    TUart0_Puts(TXBuffer);
    MCF_DMA3_DCR |= MCF_DMA_DCR_START;
*/
/*
    MCF_DMA3_DAR = (vuint32)(ImagePtr);
    MCF_DMA3_BCR = (vuint32)80;
    Cnt_HREF+=10;
    if(Cnt_HREF== 80){
        Cnt_HREF = 0;
    }
    MCF_DMA3_DCR |=  MCF_DMA_DCR_EEXT;
*/
#if 0
    TUart0_Puts("PIT1!\r\n");
    sprintf(TXBuffer,"%u,%u,%u,%u--",chang,hang,hang2,chang2);
    TUart0_Puts(TXBuffer);
    sprintf(TXBuffer,"%u,%u,%u,%u,%u,%u---",MCF_DTIM3_DTCR,MCF_DMA3_SAR,MCF_DMA3_DAR,Image,&Image[1],MCF_DMA3_BCR&0xffffff);
    TUart0_Puts(TXBuffer);
    sprintf(TXBuffer,"%u,%u,%u,%u,%u\r\n",&(Image[Cnt_HREF]),cc2[0],cc2[1],cc2[2],cc2[3]);
    TUart0_Puts(TXBuffer);
    //sprintf(TXBuffer,"--%u--%u\r\n",leftMSCnt,rightMSCnt);
    //TUart0_Puts(TXBuffer);
#endif
    chang = 0;
    hang = 0;
    hang2 = 0;
    chang2 = 0;
    /*
    if(leftMotorSpeed>0){
        //ls = leftMotorSpeed  / 625;
        ls = leftMotorSpeed;
        ls = ls  / 460;
    }
    if(rightMotorSpeed>0){
        //rs = rightMotorSpeed  / 625 * 200/1000*6.8;
        rs = rightMotorSpeed;
        rs = rs / 460;
    }
    */
    leftMSCnt = 0;
    rightMSCnt = 0;
    if(0){
        for(a = 0;a<50;a++){
            sprintf(TXBuffer,"%u-",aa2[a]);
            TUart0_Puts(TXBuffer);
        }
        bool2 = 0;
    }
}
#endif
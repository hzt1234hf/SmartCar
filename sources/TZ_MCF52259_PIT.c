#include "DIY_ComDef.h"
#ifdef TZ_MCF52259_PIT
/*
*****2015.4.21 PITģ���д���
*****���ܣ�PIT�ɱ�̶�ʱ��ģ����ж�ʱ
*****��д����������Ҫ���⣺��
    1.Ԥ��Ƶ����MCF_PIT0_PMR����ֵ��Ҫע�����
��
*/

void TPITx_Init(uint8 mode){
    switch(mode){
        default:case 0:{
            MCF_PIT0_PCSR = 0;
            //                 �Զ�����         ����Ϊ0�ж�         ʹ���ж�            ��������PMRֵ       80000000 / 2^10 = 78125 2^11 = 39062 2^12=19531
            MCF_PIT0_PCSR = MCF_PIT_PCSR_RLD | MCF_PIT_PCSR_PIF | MCF_PIT_PCSR_PIE | MCF_PIT_PCSR_OVW | MCF_PIT_PCSR_PRE(11);
            MCF_PIT0_PMR = 19531 * 1;   //���ü���ֵ
        	MCF_INTC0_IMRH &=~ MCF_INTC_IMRH_INT_MASK55;                //�����ж�Դ��Ϊ55��ʵ��λ��Ϊ55+64
        	MCF_INTC0_ICR55 = MCF_INTC_ICR_IP(4) |MCF_INTC_ICR_IL(2);   //�����ж����ȼ�
            //MCF_PIT0_PCSR |= MCF_PIT_PCSR_EN;
        }break;
        case 1:{
            MCF_PIT1_PCSR = 0;
            //                 �Զ�����         ����Ϊ0�ж�         ʹ���ж�            ��������PMRֵ       80000000 / 2^10 = 78125 2^11 = 39062 2^12=19531
            MCF_PIT1_PCSR = MCF_PIT_PCSR_RLD | MCF_PIT_PCSR_PIF | MCF_PIT_PCSR_PIE | MCF_PIT_PCSR_OVW | MCF_PIT_PCSR_PRE(11);
            MCF_PIT1_PMR = 19531 * 1;   //���ü���ֵ
        	MCF_INTC0_IMRH &=~ MCF_INTC_IMRH_INT_MASK56;                //�����ж�Դ��Ϊ55��ʵ��λ��Ϊ55+64
        	MCF_INTC0_ICR56 = MCF_INTC_ICR_IP(4) |MCF_INTC_ICR_IL(2);   //�����ж����ȼ�
            //MCF_PIT1_PCSR |= MCF_PIT_PCSR_EN;
        }break;
    }
}
__declspec(interrupt:0) void TPIT0_interrupt(void){

#if 1
    //DMA�Զ����
    TUart0_Putchar(0x01);
    TUart0_Putchar(0xFE);

    TUart0_DMAPutBuffer(Image[0],IMG_SIZE);

    TUart0_Putchar(0xFE);
    TUart0_Putchar(0x01);
#elif 0
    //DMA�Զ����

    TUart0_DMAPutBuffer(Image_ToPC,IMG_SIZE+4);

#elif 0

    TUart0_Putchar(0);
    TUart0_Putchar(255);
    TUart0_Putchar(1);
    TUart0_Putchar(0);
    TUart0_DMAPutBuffer(Image[0],IMG_SIZE);

#elif 0       //ѭ���������
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
    //TUart0_Puts("PIT0!\r\n");
    MCF_PIT0_PCSR &= ~MCF_PIT_PCSR_EN;
    MCF_PIT0_PCSR |= MCF_PIT_PCSR_PIF;  //���жϱ�־

}
__declspec(interrupt:0) void TPIT1_interrupt(void){


#if 0
    TUart0_Puts("PIT1!\r\n");
    sprintf(TXBuffer,"%u,%u,%u,%u--",chang,hang,hang2,chang2);
    TUart0_Puts(TXBuffer);
    sprintf(TXBuffer,"%u,%u,%u,%u,%u,%u---",MCF_DTIM3_DTCR,MCF_DMA3_SAR,MCF_DMA3_DAR,Image,&Image[1],MCF_DMA3_BCR&0xffffff);
    TUart0_Puts(TXBuffer);
    sprintf(TXBuffer,"%u\r\n",&(Image[Cnt_HREF]));
    TUart0_Puts(TXBuffer);
    //sprintf(TXBuffer,"--%u--%u\r\n",leftMSCnt,rightMSCnt);
    //TUart0_Puts(TXBuffer);
#endif
    hang = 0;
    hang2 = 0;
    chang = 0;
    chang2 = 0;

    leftMSCnt = 0;
    rightMSCnt = 0;

    //TUart0_Puts("PIT1!\r\n");
    MCF_PIT1_PCSR |= MCF_PIT_PCSR_PIF;  //���жϱ�־
}
#endif
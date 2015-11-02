#include "DIY_ComDef.h"
#ifdef TZ_MCF52259_DMA
/*
*****2015.4.29 DMAģ���д���
*****���ܣ�DMAģ����п������ݴ���
*****��д����������Ҫ���⣺��
    1.��Ҫע��Դ��Ŀ��֮�����ݳ��ȡ����ݵ�ַ������ʹ���������������
��
*/
#define DEBUG_ENABLE        1
#define DEBUG_DISABLE       0
#define TZDEBUG_MCF52259_DMA    DEBUG_DISABLE

uint8 aa1[12],aa2[5000],aa3 = 1,aa4[2];

void TDMAx_Init(uint8 mode){
    MCF_SCM_MPR = MCF_SCM_MPR_MPR(0x05);
    switch(mode){
        case 0:{

            /*                 ����ֵ��������       �ڲ�����ʱ��16��Ƶ          ʹ��DMA������ж�       1��Ƶ */
            MCF_DTIM0_DTMR |= MCF_DTIM_DTMR_FRR | MCF_DTIM_DTMR_CLK_DIV16 | MCF_DTIM_DTMR_ORRI | MCF_DTIM_DTMR_PS(99);
            /*                    ʹ��DMA����           ֹͣDMA������CPUֹͣ����ʱ*/
            MCF_DTIM0_DTXMR |= MCF_DTIM_DTXMR_DMAEN;// | MCF_DTIM_DTXMR_HALTED;//ʹ��DMA�����²������DMA�жϵ���ֱ�ӽ���DMA����
            /*                  ��ռĴ���*/
            MCF_DTIM0_DTER = MCF_DTIM_DTER_CAP | MCF_DTIM_DTER_REF;
            MCF_DTIM0_DTCR = 0;             //��������Ĵ���ֵ �ò�������
            MCF_DTIM0_DTCN = 0;             //��ʱ�����Ĵ���ֵ��ʼΪ0
            MCF_DTIM0_DTRR = 0xC34F;        //�������ֵ��ʼΪ����
            //ʱ��Ϊ��1/80M * 16 * (99+1) * ��0xC34F+1) = 1s

            MCF_SCM_MPR = MCF_SCM_MPR_MPR(0x05);                //DMA�ɹ�������
            MCF_SCM_DMAREQC |= MCF_SCM_DMAREQC_DMAC0(0x08);     //����Ϊʹ��UART0��Ϊ�ⲿ����
            MCF_DMA0_SAR = (vuint32)0;
            MCF_DMA0_DAR = (vuint32)0;
            MCF_DMA0_DSR = 0;
            MCF_DMA0_BCR = 0;
            /*               ʹ�ܲ����ж�      Դ��ַ�Զ�����      Դ��ַ����Ϊ�ֽ���      Ŀ�ĵ�ַ����Ϊ�ֽ���      */
            MCF_DMA0_DCR =  MCF_DMA_DCR_INT | MCF_DMA_DCR_SINC | MCF_DMA_DCR_SSIZE_BYTE | MCF_DMA_DCR_DSIZE_BYTE;
            MCF_DMA0_DCR |= MCF_DMA_DCR_D_REQ;	//�Զ�����

            MCF_DTIM0_DTMR |= MCF_DTIM_DTMR_RST;    //ʹ��DMA0��ʱ��

        	MCF_INTC0_IMRL &=~ MCF_INTC_IMRL_MASKALL;                   //�������������ж�
        	MCF_INTC0_IMRL &=~ MCF_INTC_IMRL_INT_MASK9;                 //�����ж�Դ��Ϊ9��ʵ��λ��Ϊ9+64
        	MCF_INTC0_ICR09 = MCF_INTC_ICR_IP(4) |MCF_INTC_ICR_IL(2);   //�����ж����ȼ�
        	MCF_INTC0_IMRL &=~ MCF_INTC_IMRL_MASKALL;                   //�������������ж�
        	MCF_INTC0_IMRL &=~ MCF_INTC_IMRL_INT_MASK19;                 //�����ж�Դ��Ϊ9��ʵ��λ��Ϊ9+64
        	MCF_INTC0_ICR19 = MCF_INTC_ICR_IP(4) |MCF_INTC_ICR_IL(2);   //�����ж����ȼ�

        }break;
        case 1:{

            /*                 ����ֵ��������       �ڲ�����ʱ��16��Ƶ          ʹ��DMA������ж�       1��Ƶ */
            MCF_DTIM1_DTMR = MCF_DTIM_DTMR_FRR | MCF_DTIM_DTMR_CLK_DIV16 | MCF_DTIM_DTMR_ORRI | MCF_DTIM_DTMR_PS(99);
            /*                    ʹ��DMA����           ֹͣDMA������CPUֹͣ����ʱ*/
            MCF_DTIM1_DTXMR = MCF_DTIM_DTXMR_DMAEN;// | MCF_DTIM_DTXMR_HALTED;//ʹ��DMA�����²������DMA�жϵ���ֱ�ӽ���DMA����
            /*                  ��ռĴ���*/
            MCF_DTIM1_DTER = MCF_DTIM_DTER_CAP | MCF_DTIM_DTER_REF;
            MCF_DTIM1_DTCR = 0;             //��������Ĵ���ֵ �ò�������
            MCF_DTIM1_DTCN = 0;             //��ʱ�����Ĵ���ֵ��ʼΪ0
            MCF_DTIM1_DTRR = 0xC34F;        //�������ֵ��ʼΪ����
            //ʱ��Ϊ��1/80M * 16 * (99+1) * ��0xC34F+1) = 1s

            //MCF_SCM_DMAREQC |= 0x05<<1;
            MCF_DMA1_SAR = (vuint32)0x40100030;
            MCF_DMA1_DAR = (vuint32)aa2;
            //MCF_DMA1_DSR = 0;
            MCF_DMA1_BCR = 12;
            /*               ʹ�ܲ����ж�      Ŀ�ĵ�ַ�Զ�����      Դ��ַ����Ϊ�ֽ���      Ŀ�ĵ�ַ����Ϊ�ֽ���      */
            MCF_DMA1_DCR =  MCF_DMA_DCR_INT | MCF_DMA_DCR_DINC | MCF_DMA_DCR_SSIZE_BYTE | MCF_DMA_DCR_DSIZE_BYTE;
            //MCF_DMA1_DCR |= MCF_DMA_DCR_D_REQ;	//�Զ�����

            //MCF_DTIM1_DTMR |= MCF_DTIM_DTMR_RST;    //ʹ��DMA1��ʱ��

        	MCF_INTC0_IMRL &=~ MCF_INTC_IMRL_MASKALL;                   //�������������ж�
        	MCF_INTC0_IMRL &=~ MCF_INTC_IMRL_INT_MASK10;                 //�����ж�Դ��Ϊ9��ʵ��λ��Ϊ9+64
        	MCF_INTC0_ICR10 = MCF_INTC_ICR_IP(4) |MCF_INTC_ICR_IL(2);   //�����ж����ȼ�
        	MCF_INTC0_IMRL &=~ MCF_INTC_IMRL_MASKALL;                   //�������������ж�
        	MCF_INTC0_IMRL &=~ MCF_INTC_IMRL_INT_MASK20;                 //�����ж�Դ��Ϊ9��ʵ��λ��Ϊ9+64
        	MCF_INTC0_ICR20 = MCF_INTC_ICR_IP(4) |MCF_INTC_ICR_IL(2);   //�����ж����ȼ�
        }break;
        case 2:{

            /*                 ����ֵ��������       �ڲ�����ʱ��16��Ƶ          ʹ��DMA������ж�       100��Ƶ */
            MCF_DTIM2_DTMR = MCF_DTIM_DTMR_FRR | MCF_DTIM_DTMR_CLK_DIV16 | MCF_DTIM_DTMR_ORRI | MCF_DTIM_DTMR_PS(99);
            /*                    ʹ��DMA����           ֹͣDMA������CPUֹͣ����ʱ*/
            MCF_DTIM2_DTXMR = MCF_DTIM_DTXMR_DMAEN;// | MCF_DTIM_DTXMR_HALTED;//ʹ��DMA�����²������DMA�жϵ���ֱ�ӽ���DMA����
            /*                  ��ռĴ���*/
            MCF_DTIM2_DTER = MCF_DTIM_DTER_CAP | MCF_DTIM_DTER_REF;
            MCF_DTIM2_DTCR = 0;             //��������Ĵ���ֵ �ò�������
            //MCF_DTIM2_DTCN = 0;             //��ʱ�����Ĵ���ֵ��ʼΪ0
            //MCF_DTIM2_DTRR = 0xC34F;        //�������ֵ��ʼΪ����
            MCF_DTIM2_DTRR = 10000;        //�������ֵ��ʼΪ����
            //ʱ��Ϊ��1/80M / 16 / (99+1) * ��0xC34F+1) = 1s

            MCF_SCM_DMAREQC |= MCF_SCM_DMAREQC_DMAC2(0x07);//����Ϊʹ��DTIME2��Ϊ�ⲿ����
            MCF_DMA2_SAR = (vuint32)0x40100030;
            MCF_DMA2_DAR = (vuint32)aa2;
            //MCF_DMA2_DSR = 0;
            MCF_DMA2_BCR = 0;
            /*                 Դ��ַ����Ϊ�ֽ���      Ŀ�ĵ�ַ����Ϊ�ֽ���       ������ȡ        Դ��ַ�Զ�����    ʹ���ж�*/
            MCF_DMA2_DCR =  MCF_DMA_DCR_INT | MCF_DMA_DCR_SSIZE_BYTE | MCF_DMA_DCR_DSIZE_BYTE | MCF_DMA_DCR_CS | MCF_DMA_DCR_INT;//| MCF_DMA_DCR_SINC | MCF_DMA_DCR_INT;

            MCF_DTIM2_DTMR |= MCF_DTIM_DTMR_RST;    //ʹ��DMA1��ʱ��

        	MCF_INTC0_IMRL &=~ MCF_INTC_IMRL_MASKALL;                   //�������������ж�
        	MCF_INTC0_IMRL &=~ MCF_INTC_IMRL_INT_MASK11;                 //�����ж�Դ��Ϊ9��ʵ��λ��Ϊ9+64
        	MCF_INTC0_ICR11 = MCF_INTC_ICR_IP(4) |MCF_INTC_ICR_IL(2);   //�����ж����ȼ�
        	MCF_INTC0_IMRL &=~ MCF_INTC_IMRL_MASKALL;                   //�������������ж�
        	MCF_INTC0_IMRL &=~ MCF_INTC_IMRL_INT_MASK21;                 //�����ж�Դ��Ϊ9��ʵ��λ��Ϊ9+64
        	MCF_INTC0_ICR21 = MCF_INTC_ICR_IP(4) |MCF_INTC_ICR_IL(2);   //�����ж����ȼ�

        	//MCF_DMA2_DCR |= MCF_DMA_DCR_START;
        }break;
        case 3:{
            MCF_GPIO_PTCPAR |= MCF_GPIO_PTCPAR_DTIN3_DTIN3;
            /*                 ����ֵ��������       DTIN�ⲿ����ʱ��        ʹ��DMA������ж�       1��Ƶ             �����ش���    */
            MCF_DTIM3_DTMR = /*MCF_DTIM_DTMR_FRR |*/ MCF_DTIM_DTMR_CLK_DTIN /*| MCF_DTIM_DTMR_ORRI | MCF_DTIM_DTMR_PS(0) */| MCF_DTIM_DTMR_CE_RISE;
            /*                    ʹ��DMA����           ֹͣDMA������CPUֹͣ����ʱ*/
            MCF_DTIM3_DTXMR = MCF_DTIM_DTXMR_DMAEN;// | MCF_DTIM_DTXMR_HALTED;//ʹ��DMA�����²������DMA�жϵ���ֱ�ӽ���DMA����
            /*                  ��ռĴ���*/
            MCF_DTIM3_DTER = MCF_DTIM_DTER_CAP;
            MCF_DTIM3_DTCR = 0;             //��������Ĵ���ֵ �ò�������
            MCF_DTIM3_DTCN = 0;             //��ʱ�����Ĵ���ֵ��ʼΪ0
            MCF_DTIM3_DTRR = 0;             //ʱ�Ӽ������ֵ��ʼΪ����

            MCF_DTIM3_DTMR |= MCF_DTIM_DTMR_RST;    //ʹ��DMA3��ʱ��

            //MCF_SCM_MPR = MCF_SCM_MPR_MPR(0x05);            //DMA�ɹ�������
            //MCF_SCM_DMAREQC |= MCF_SCM_DMAREQC_DMAC3(0x07); //����Ϊʹ��DTIME3��Ϊ�ⲿ����
            MCF_DMA3_SAR = (vuint32)0;
            MCF_DMA3_DAR = (vuint32)0;
            MCF_DMA3_BCR = 0;
            MCF_DMA3_DSR = MCF_DMA_DSR_DONE;

            /*                 Դ��ַ����Ϊ�ֽ���      Ŀ�ĵ�ַ����Ϊ�ֽ���       ������ȡ       Ŀ�ĵ�ַ�Զ�����  ������Ϻ�ر��ⲿ����     ʹ���ж�*/
            //MCF_DMA3_DCR = MCF_DMA_DCR_SSIZE_BYTE | MCF_DMA_DCR_DSIZE_BYTE  | MCF_DMA_DCR_CS | MCF_DMA_DCR_DINC | MCF_DMA_DCR_D_REQ | MCF_DMA_DCR_INT | MCF_DMA_DCR_AA;
            MCF_DMA3_DCR = MCF_DMA_DCR_SSIZE_BYTE | MCF_DMA_DCR_DSIZE_BYTE | MCF_DMA_DCR_INT | MCF_DMA_DCR_DINC | MCF_DMA_DCR_AA;

        	MCF_INTC0_IMRL &=~ MCF_INTC_IMRL_MASKALL;                   //�������������ж�
        	MCF_INTC0_IMRL &=~ MCF_INTC_IMRL_INT_MASK12;                 //�����ж�Դ��Ϊ9��ʵ��λ��Ϊ9+64
        	MCF_INTC0_ICR12 = MCF_INTC_ICR_IP(4) |MCF_INTC_ICR_IL(2);   //�����ж����ȼ�
        	MCF_INTC0_IMRL &=~ MCF_INTC_IMRL_MASKALL;                   //�������������ж�
        	MCF_INTC0_IMRL &=~ MCF_INTC_IMRL_INT_MASK22;                 //�����ж�Դ��Ϊ9��ʵ��λ��Ϊ9+64
        	MCF_INTC0_ICR22 = MCF_INTC_ICR_IP(4) |MCF_INTC_ICR_IL(2);   //�����ж����ȼ�
        }break;
    }
}
#if (TZDEBUG_MCF52259_DMA == 1)
__declspec(interrupt) void TDMA0_interrupt(void){
	if(MCF_DMA0_DSR & MCF_DMA_DSR_DONE)
	{
		if(MCF_DMA0_DSR & MCF_DMA_DSR_CE)
		{
			TUart1_Puts("configuration error\n");
		}
		else if(MCF_DMA0_DSR & MCF_DMA_DSR_BED)
		{
			TUart1_Puts("destination bus error\n");
		}
		else if(MCF_DMA0_DSR & MCF_DMA_DSR_BES)
		{
			TUart1_Puts("source bus error\n");
		}
		else
		{
			TUart1_Puts("dma0 transfer done\n");
		}
		//clear DMA0 interrupt
		MCF_DMA0_DSR |= MCF_DMA_DSR_DONE;
	}
	if(MCF_DMA0_DSR & MCF_DMA_DSR_BSY)
	{
		TUart1_Puts("busy\n");
	}
	if(MCF_DMA0_DSR & MCF_DMA_DSR_REQ)
	{
		TUart1_Puts("transfer remaining but channel not selected\n");
	}
}
__declspec(interrupt) void TDMA1_interrupt(void){

	if(MCF_DMA1_DSR & MCF_DMA_DSR_DONE)
	{
		if(MCF_DMA1_DSR & MCF_DMA_DSR_CE)
		{
			TUart1_Puts("configuration error\n");
		}
		else if(MCF_DMA1_DSR & MCF_DMA_DSR_BED)
		{
			TUart1_Puts("destination bus error\n");
		}
		else if(MCF_DMA1_DSR & MCF_DMA_DSR_BES)
		{
			TUart1_Puts("source bus error\n");
		}
		else
		{
			TUart1_Puts("dma0 transfer done\n");
		}
		//clear DMA0 interrupt
		MCF_DMA1_DSR |= MCF_DMA_DSR_DONE;
	}
	if(MCF_DMA1_DSR & MCF_DMA_DSR_BSY)
	{
		TUart1_Puts("busy\n");
	}
	if(MCF_DMA1_DSR & MCF_DMA_DSR_REQ)
	{
		TUart1_Puts("transfer remaining but channel not selected\n");
	}
}
__declspec(interrupt) void TDTIM0_interrupt(void){
    MCF_DTIM0_DTER = MCF_DTIM_DTER_CAP | MCF_DTIM_DTER_REF;
    TUart0_Puts("DTIM0\r\n");
}
__declspec(interrupt) void TDTIM1_interrupt(void){
    MCF_DTIM1_DTER = MCF_DTIM_DTER_CAP | MCF_DTIM_DTER_REF;
    TUart0_Puts("DTIM1\r\n");
}
#else
__declspec(interrupt) void TDMA0_interrupt(void){
    MCF_DMA0_DSR |= MCF_DMA_DSR_DONE;
    TUart0_Puts("DMA0\r\n");
}
__declspec(interrupt) void TDMA1_interrupt(void){
    sprintf(TXBuffer,"%u-",MCF_DMA1_DSR);
    TUart0_Puts(TXBuffer);
    hang2++;
    //bool = 1;
    MCF_DMA1_DSR |= MCF_DMA_DSR_DONE;
    TUart0_Puts("DMA1\r\n");
}
__declspec(interrupt) void TDMA2_interrupt(void){
    MCF_DMA2_DSR |= MCF_DMA_DSR_DONE;
    TUart0_Puts("DMA2\r\n");
}
__declspec(interrupt) void TDMA3_interrupt(void){
    sprintf(TXBuffer,"%u-",MCF_DMA3_DSR);
    TUart0_Puts(TXBuffer);

    TUart0_Puts("DMA3\r\n");
    MCF_DMA3_DSR |= MCF_DMA_DSR_DONE;
    //bool = 1;
}
__declspec(interrupt) void TDTIM0_interrupt(void){
    MCF_DTIM0_DTER = MCF_DTIM_DTER_CAP | MCF_DTIM_DTER_REF;
    TUart0_Puts("DTIM0");
}
__declspec(interrupt) void TDTIM1_interrupt(void){
    MCF_DTIM1_DTER = MCF_DTIM_DTER_CAP | MCF_DTIM_DTER_REF;
    TUart0_Puts("DTIM1");
}
__declspec(interrupt) void TDTIM2_interrupt(void){
    MCF_DTIM2_DTER = MCF_DTIM_DTER_CAP | MCF_DTIM_DTER_REF;
    TUart0_Puts("DTIM2");
}
__declspec(interrupt) void TDTIM3_interrupt(void){
    MCF_DTIM3_DTER = MCF_DTIM_DTER_CAP;
    TUart0_Puts("DTIM3");
}
#endif

#endif

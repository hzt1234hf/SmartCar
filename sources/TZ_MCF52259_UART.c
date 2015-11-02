#include "DIY_ComDef.h"
#ifdef TZ_MCF52259_UART
/*
*****2015.4.21����ģ���д���
*****���ܣ�UART0-2�ܹ�ʹ���жϣ�UART0�ݲ��ܹ�ʹ��DMA����
*****��д����������Ҫ���⣺��
    1.�ж���������Ϥ�����жϺ����޷����루��MCF_INTC0_IMRL�Ĵ������ж������ļ�exceptions.c�йأ�
    2.UMR1�Ĵ���ʹ����MCF_UART_UMR_RXIRQ���²��ܽ����ж�
    3.MCF_UART2_UTB��MCF_UART2_URB���������жϺ������ܷ�����ȷ����
    4.��������⣩���ŽӴ�������һģ���޷������жϣ��ѩn��b��-Ҳ�����ˣ�
��
*/

/*
*****2015.5.17����DMA���ܱ�д���
*****���ܣ�UART0ʹ�ô���DMA��������
*****��д����������Ҫ���⣺��
    1.�ھ��������DMA����о�������DMAģ��д���ˣ�����UART������ȻҲҪ��һ������
        ����DMA UART����ô�����
��
*/
char TXBuffer[100];
char RXBuffer[100];
void TUartx_INIT(uint8 x){
    uint16 ubgs;
    switch(x){
        case 0:default:{
        	MCF_GPIO_PUAPAR |= (MCF_GPIO_PUAPAR_UTXD0_UTXD0|MCF_GPIO_PUAPAR_URXD0_URXD0);//����GPIO��ΪUARTģʽ
        	//MCF_UART0_UCR = 010;	//��λ������
        	//MCF_UART0_UCR = 011;	//��λ������
        	//MCF_UART0_UCR = 001;	//��λģʽ�Ĵ���ָ��
        	MCF_UART0_UCR = MCF_UART_UCR_RESET_MR | MCF_UART_UCR_RESET_RX | MCF_UART_UCR_RESET_TX;
        	MCF_UART0_UMR1 = MCF_UART_UMR_BC_8 |MCF_UART_UMR_PM_NONE;//�����ж�ʹ�ܣ�8λ����λ������żУ��
        	MCF_UART0_UMR2 = MCF_UART_UMR_CM_NORMAL | MCF_UART_UMR_SB_STOP_BITS_1;  //����ͨ��ģʽ��1λֹͣλ
        	MCF_UART0_UCSR = MCF_UART_UCSR_TCS_SYS_CLK | MCF_UART_UCSR_RCS_SYS_CLK; //TXD��RXD������ϵͳʱ��
        	MCF_UART0_UIMR = MCF_UART_UIMR_DB | MCF_UART_UIMR_COS;
        	//MCF_UART0_UIMR = MCF_UART_UIMR_FFULL_RXRDY;		//�ر������ж�
        	//               ����Ƶ�� *1000000/��Ҫ�Ĳ�����*32
        	ubgs = (uint16)(sysOsciFre/(115200*32));
        	MCF_UART0_UBG1 = (uint8)((ubgs&0xFF00)>>8);
        	MCF_UART0_UBG2 = (uint8)(ubgs&0x00FF);

        	//MCF_INTC0_IMRL &=~ MCF_INTC_IMRL_MASKALL;       //�������������ж�
            //MCF_INTC0_IMRL &=~ MCF_INTC_IMRL_INT_MASK13;    //�����ж�Դ��Ϊ13��ʵ��λ��Ϊ��13+64
            /*              �ж����ȼ������7�����жϼ������3��          */
            //MCF_INTC0_ICR13 = MCF_INTC_ICR_IP(4) |MCF_INTC_ICR_IL(2);

        	//MCF_UART0_UIMR = 0;		            //�ر�����UART��CPU�ж�
        	MCF_UART0_UCR = MCF_UART_UCR_RX_ENABLED | MCF_UART_UCR_TX_ENABLED;//ʹ�ܴ��ڽ��շ���

        }break;
        case 1:{
        	MCF_GPIO_PUBPAR |= (MCF_GPIO_PUBPAR_UTXD1_UTXD1|MCF_GPIO_PUBPAR_URXD1_URXD1);//����GPIO��ΪUARTģʽ
        	MCF_UART1_UCR = 010;	//��λ������
        	MCF_UART1_UCR = 011;	//��λ������
        	MCF_UART1_UCR = 001;	//��λģʽ�Ĵ���ָ��
        	MCF_UART1_UCR = MCF_UART_UCR_RESET_MR | MCF_UART_UCR_RESET_RX | MCF_UART_UCR_RESET_TX;
        	MCF_UART1_UMR1 = MCF_UART_UMR_BC_8 |MCF_UART_UMR_PM_NONE;//�����ж�ʹ�ܣ�8λ����λ������żУ��
        	MCF_UART1_UMR2 = MCF_UART_UMR_CM_NORMAL | MCF_UART_UMR_SB_STOP_BITS_1;  //����ͨ��ģʽ��1λֹͣλ
        	MCF_UART1_UCSR = MCF_UART_UCSR_TCS_SYS_CLK | MCF_UART_UCSR_RCS_SYS_CLK; //TXD��RXD������ϵͳʱ��
        	MCF_UART1_UIMR = MCF_UART_UIMR_FFULL_RXRDY;		//�򿪽��ܾ����ж�
        	//               ����Ƶ�� *1000000/��Ҫ�Ĳ�����*32
        	ubgs = (uint16)(sysOsciFre/(115200*32));
        	MCF_UART1_UBG1 = (uint8)((ubgs&0xFF00)>>8);
        	MCF_UART1_UBG2 = (uint8)(ubgs&0x00FF);

        	MCF_INTC0_IMRL &=~ MCF_INTC_IMRL_MASKALL;//�������������ж�
            MCF_INTC0_IMRL &=~ MCF_INTC_IMRL_INT_MASK14;//�����ж�Դ��Ϊ14��ʵ��λ��Ϊ��14+64
            //              �ж����ȼ������7�����жϼ������3��
            MCF_INTC0_ICR14 = MCF_INTC_ICR_IP(4) |MCF_INTC_ICR_IL(2);

        	MCF_UART1_UCR = MCF_UART_UCR_RX_ENABLED | MCF_UART_UCR_TX_ENABLED;//ʹ�ܴ��ڽ��շ���
        }break;
        case 2:{
        	MCF_GPIO_PUCPAR |= (MCF_GPIO_PUCPAR_UTXD2_UTXD2|MCF_GPIO_PUCPAR_URXD2_URXD2);//����GPIO��ΪUARTģʽ
        	MCF_UART2_UCR = 010;	//��λ������
        	MCF_UART2_UCR = 011;	//��λ������
        	MCF_UART2_UCR = 001;	//��λģʽ�Ĵ���ָ��
        	MCF_UART2_UCR = MCF_UART_UCR_RESET_MR | MCF_UART_UCR_RESET_RX | MCF_UART_UCR_RESET_TX;
        	MCF_UART2_UMR1 = MCF_UART_UMR_BC_8 |MCF_UART_UMR_PM_NONE;//�����ж�ʹ�ܣ�8λ����λ������żУ��
        	MCF_UART2_UMR2 = MCF_UART_UMR_CM_NORMAL | MCF_UART_UMR_SB_STOP_BITS_1;  //����ͨ��ģʽ��1λֹͣλ
        	MCF_UART2_UCSR = MCF_UART_UCSR_TCS_SYS_CLK | MCF_UART_UCSR_RCS_SYS_CLK; //TXD��RXD������ϵͳʱ��
        	MCF_UART2_UIMR = MCF_UART_UIMR_FFULL_RXRDY;//MCF_UART_UIMR_FFULL_RXRDY;		//�򿪽��ܾ����ж�
        	//MCF_UART2_UIMR = MCF_UART_UIMR_DB | MCF_UART_UIMR_COS;
        	//               ����Ƶ�� *1000000/��Ҫ�Ĳ�����*32
        	ubgs = (uint16)(sysOsciFre/(115200*32));
        	MCF_UART2_UBG1 = (uint8)((ubgs&0xFF00)>>8);
        	MCF_UART2_UBG2 = (uint8)(ubgs&0x00FF);

        	MCF_INTC0_IMRL &=~ MCF_INTC_IMRL_MASKALL;//�������������ж�
            MCF_INTC0_IMRL &=~ MCF_INTC_IMRL_INT_MASK15;//�����ж�Դ��Ϊ15��ʵ��λ��Ϊ��15+64
            //              �ж����ȼ������7�����жϼ������3��
            MCF_INTC0_ICR15 = MCF_INTC_ICR_IP(4) |MCF_INTC_ICR_IL(2);

        	MCF_UART2_UCR = MCF_UART_UCR_RX_ENABLED | MCF_UART_UCR_TX_ENABLED;//ʹ�ܴ��ڽ��շ���

        }break;
	}
}
void TUart0_Putchar(char data){

    while(!(MCF_UART0_USR & MCF_UART_USR_TXEMP)){};
//asm(move.b data,0x4000020C);
    MCF_UART0_UTB = data;
}
void TUart0_Puts(char *data){
    while(*data) TUart0_Putchar(*data++);
}
void TUart0_DMAInit(){

    MCF_DMA0_SAR = (vuint32)TXBuffer;
    MCF_DMA0_DAR = (vuint32)0x4000020C;
    MCF_DMA0_BCR = 0;

    //MCF_SCM_MPR |= 0x04;              //ʹDMA������Ϊ����ԱȨ��
    MCF_SCM_DMAREQC |= MCF_SCM_DMAREQC_DMAC0(0x0C);        //ʹ��DMA0ģ������ԴUART0������
    MCF_SCM_PACR2 = MCF_SCM_PACR_ACCESS_CTRL1(6);           //����DMA���û�/����Ա ����Ȩ�� --- �ɶ�/��д
    MCF_SCM_GPACR0 = MCF_SCM_GPACR_ACCESS_CTRL(6);
    //---2015.5.2---MCF_SCM_RAMBAR |= MCF_SCM_RAMBAR_BDE;//ʹ�ܷǺ���ģ��(����ָDMA)����SRAM
    /*               ������ȡ         Դ��ַ����Ϊ�ֽ���     Ŀ�ĵ�ַ����Ϊ�ֽ���         Դ��ַ�Զ�����          �Զ�����*/
    MCF_DMA0_DCR = MCF_DMA_DCR_CS | MCF_DMA_DCR_SSIZE(1) | MCF_DMA_DCR_DSIZE(1) | MCF_DMA_DCR_SINC | MCF_DMA_DCR_D_REQ;

    /*Disable interrupts using the UIMR register. The appropriate UIMR bits must be cleared so that
    interrupt requests are disabled for those conditions for which a DMA request is desired. For
    example, to generate transmit DMA requests from UART1, UIMR1[TXRDY] should be cleared.
    This prevents TXRDY from generating an interrupt request while a transmit DMA request is
    generated.
    */

    /*
    �ĵ�����һ�λ�����Ҫ����RAMBAR�Ĵ�����SCM RAMBAR�Ĵ�������Ȼ�������ƣ���RAMBAR�Ĵ�����
    �ں˼Ĵ�������ʹ�ں��ܹ�����SRAM�ڴ棬��SCM RAMBAR�Ĵ����������������ܹ�����SRAM�ڴ��
    */

    /*
    ������DMA�����ܲ�ǿ����ô���ã�����STM32���ֲ��ò����У�����~~~
    */
    MCF_INTC0_IMRL &=~ MCF_INTC_IMRL_MASKALL;                   //�������������ж�
    MCF_INTC0_IMRL &=~ MCF_INTC_IMRL_INT_MASK9;                 //�����ж�Դ��Ϊ9��ʵ��λ��Ϊ9+64
    MCF_INTC0_ICR09 = MCF_INTC_ICR_IP(5) |MCF_INTC_ICR_IL(2);   //�����ж����ȼ�
    /*              �����ⲿ����    */
    //MCF_DMA0_DCR |= MCF_DMA_DCR_EEXT;

}
void TUart0_DMAPuts(uint16 size){

    MCF_DMA0_DSR |= MCF_DMA_DSR_DONE;
    MCF_DMA0_DAR = (vuint32)0x4000020C;
    MCF_DMA0_SAR = (vuint32)TXBuffer;
    MCF_DMA0_BCR = size;
    MCF_DMA0_DCR |= MCF_DMA_DCR_EEXT;

}
void TUart0_DMAPutBuffer(vuint8 *buf,uint16 size){

    MCF_DMA0_DSR |= MCF_DMA_DSR_DONE;
    MCF_DMA0_DAR = (vuint32)0x4000020C;
    MCF_DMA0_SAR = (vuint32)buf;
    MCF_DMA0_BCR = size;
    MCF_DMA0_DCR |= MCF_DMA_DCR_EEXT;

}
void TUart1_Putchar(char data){

    while(!(MCF_UART1_USR & MCF_UART_USR_TXEMP)){};
    MCF_UART1_UTB = data;
}
void TUart1_Puts(char *data){
    while(*data) TUart1_Putchar(*data++);
}
void TUart2_Putchar(char data){

    while(!(MCF_UART2_USR & MCF_UART_USR_TXEMP)){};
    MCF_UART2_UTB = data;
}
void TUart2_Puts(char *data){
    while(*data) TUart2_Putchar(*data++);
}
__declspec(interrupt) void UART0_DMA_interrupt(void){
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
    MCF_DMA0_DSR |= MCF_DMA_DSR_DONE;
}
__declspec(interrupt) void UART0_interrupt(void){

    uint8 a;
    TUart0_Putchar('A');
    if(MCF_UART0_USR&MCF_UART_USR_RXRDY){
        a = MCF_UART0_URB;
        TUart0_Putchar(a);
        TUart0_Putchar('+');
    }

}
__declspec(interrupt) void UART1_interrupt(void){

    uint8 a;
    TUart1_Putchar('B');
    if(MCF_UART1_USR&MCF_UART_USR_RXRDY){
        a = MCF_UART1_URB;
        TUart1_Putchar(a);
        TUart1_Putchar('+');
    }

}
__declspec(interrupt) void UART2_interrupt(void){

    uint8 a;
    TUart2_Putchar('C');
    if(MCF_UART2_USR&MCF_UART_USR_RXRDY){
        a = MCF_UART2_URB;
        TUart2_Putchar(a);
        TUart2_Putchar('+');
    }

}
#endif
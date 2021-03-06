#include "DIY_ComDef.h"
#ifdef TZ_MCF52259_UART
/*
*****2015.4.21串口模块编写完毕
*****功能：UART0-2能够使用中断，UART0暂不能够使用DMA传输
*****编写中遇到的主要问题：（
    1.中断向量表不熟悉导致中断函数无法进入（与MCF_INTC0_IMRL寄存器和中断向量文件exceptions.c有关）
    2.UMR1寄存器使能了MCF_UART_UMR_RXIRQ导致不能进入中断
    3.MCF_UART2_UTB和MCF_UART2_URB混淆导致中断函数不能发送正确数据
    4.（最后问题）引脚接错导致其中一模块无法进入中断（⊙﹏⊙b汗-也是醉了）
）
*/

/*
*****2015.5.17串口DMA功能编写完成
*****功能：UART0使用串口DMA发送数据
*****编写中遇到的主要问题：（
    1.在经过几天的DMA深度研究后，终于DMA模块写完了，所以UART理所当然也要分一杯羹，
        所以DMA UART就这么完成了
）
*/
char TXBuffer[100];
char RXBuffer[100];
uint8 uartMode = 0;

void TUartx_INIT(uint8 x){
    uint16 ubgs;
    switch(x){
        case 0:default:{
        	MCF_GPIO_PUAPAR |= (MCF_GPIO_PUAPAR_UTXD0_UTXD0|MCF_GPIO_PUAPAR_URXD0_URXD0);//配置GPIO口为UART模式
        	//MCF_UART0_UCR = 010;	//复位接收器
        	//MCF_UART0_UCR = 011;	//复位发送器
        	//MCF_UART0_UCR = 001;	//复位模式寄存器指针
        	MCF_UART0_UCR = MCF_UART_UCR_RESET_MR | MCF_UART_UCR_RESET_RX | MCF_UART_UCR_RESET_TX;
        	MCF_UART0_UMR1 = MCF_UART_UMR_BC_8 |MCF_UART_UMR_PM_NONE;//接收中断使能，8位数据位，无奇偶校验
        	MCF_UART0_UMR2 = MCF_UART_UMR_CM_NORMAL | MCF_UART_UMR_SB_STOP_BITS_1;  //正常通道模式，1位停止位
        	MCF_UART0_UCSR = MCF_UART_UCSR_TCS_SYS_CLK | MCF_UART_UCSR_RCS_SYS_CLK; //TXD和RXD都是用系统时钟
        	MCF_UART0_UIMR = MCF_UART_UIMR_DB | MCF_UART_UIMR_COS;
        	//MCF_UART0_UIMR = MCF_UART_UIMR_FFULL_RXRDY;		//关闭所有中断
        	//               晶振频率 *1000000/需要的波特率*32
        	ubgs = (uint16)(sysOsciFre/(115200*32));
        	MCF_UART0_UBG1 = (uint8)((ubgs&0xFF00)>>8);
        	MCF_UART0_UBG2 = (uint8)(ubgs&0x00FF);

        	//MCF_INTC0_IMRL &=~ MCF_INTC_IMRL_MASKALL;       //清零屏蔽所有中断
            //MCF_INTC0_IMRL &=~ MCF_INTC_IMRL_INT_MASK13;    //设置中断源号为13，实际位置为，13+64
            /*              中断优先级，最高7级，中断级别，最高3级          */
            //MCF_INTC0_ICR13 = MCF_INTC_ICR_IP(4) |MCF_INTC_ICR_IL(2);

        	//MCF_UART0_UIMR = 0;		            //关闭所有UART的CPU中断
        	MCF_UART0_UCR = MCF_UART_UCR_RX_ENABLED | MCF_UART_UCR_TX_ENABLED;//使能串口接收发送

        }break;
        case 1:{
        	MCF_GPIO_PUBPAR |= (MCF_GPIO_PUBPAR_UTXD1_UTXD1|MCF_GPIO_PUBPAR_URXD1_URXD1);//配置GPIO口为UART模式
        	MCF_UART1_UCR = 010;	//复位接收器
        	MCF_UART1_UCR = 011;	//复位发送器
        	MCF_UART1_UCR = 001;	//复位模式寄存器指针
        	MCF_UART1_UCR = MCF_UART_UCR_RESET_MR | MCF_UART_UCR_RESET_RX | MCF_UART_UCR_RESET_TX;
        	MCF_UART1_UMR1 = MCF_UART_UMR_BC_8 |MCF_UART_UMR_PM_NONE;//接收中断使能，8位数据位，无奇偶校验
        	MCF_UART1_UMR2 = MCF_UART_UMR_CM_NORMAL | MCF_UART_UMR_SB_STOP_BITS_1;  //正常通道模式，1位停止位
        	MCF_UART1_UCSR = MCF_UART_UCSR_TCS_SYS_CLK | MCF_UART_UCSR_RCS_SYS_CLK; //TXD和RXD都是用系统时钟
        	MCF_UART1_UIMR = MCF_UART_UIMR_FFULL_RXRDY;		//打开接受就绪中断
        	//               晶振频率 *1000000/需要的波特率*32
        	ubgs = (uint16)(sysOsciFre/(115200*32));
        	MCF_UART1_UBG1 = (uint8)((ubgs&0xFF00)>>8);
        	MCF_UART1_UBG2 = (uint8)(ubgs&0x00FF);

        	MCF_INTC0_IMRL &=~ MCF_INTC_IMRL_MASKALL;//清零屏蔽所有中断
            MCF_INTC0_IMRL &=~ MCF_INTC_IMRL_INT_MASK14;//设置中断源号为14，实际位置为，14+64
            //              中断优先级，最高7级，中断级别，最高3级
            MCF_INTC0_ICR14 = MCF_INTC_ICR_IP(5) |MCF_INTC_ICR_IL(3);

        	MCF_UART1_UCR = MCF_UART_UCR_RX_ENABLED | MCF_UART_UCR_TX_ENABLED;//使能串口接收发送
        }break;
        case 2:{
        	MCF_GPIO_PUCPAR |= (MCF_GPIO_PUCPAR_UTXD2_UTXD2|MCF_GPIO_PUCPAR_URXD2_URXD2);//配置GPIO口为UART模式
        	MCF_UART2_UCR = 010;	//复位接收器
        	MCF_UART2_UCR = 011;	//复位发送器
        	MCF_UART2_UCR = 001;	//复位模式寄存器指针
        	MCF_UART2_UCR = MCF_UART_UCR_RESET_MR | MCF_UART_UCR_RESET_RX | MCF_UART_UCR_RESET_TX;
        	MCF_UART2_UMR1 = MCF_UART_UMR_BC_8 |MCF_UART_UMR_PM_NONE;//接收中断使能，8位数据位，无奇偶校验
        	MCF_UART2_UMR2 = MCF_UART_UMR_CM_NORMAL | MCF_UART_UMR_SB_STOP_BITS_1;  //正常通道模式，1位停止位
        	MCF_UART2_UCSR = MCF_UART_UCSR_TCS_SYS_CLK | MCF_UART_UCSR_RCS_SYS_CLK; //TXD和RXD都是用系统时钟
        	MCF_UART2_UIMR = MCF_UART_UIMR_FFULL_RXRDY;//MCF_UART_UIMR_FFULL_RXRDY;		//打开接受就绪中断
        	//MCF_UART2_UIMR = MCF_UART_UIMR_DB | MCF_UART_UIMR_COS;
        	//               晶振频率 *1000000/需要的波特率*32
        	ubgs = (uint16)(sysOsciFre/(115200*32));
        	MCF_UART2_UBG1 = (uint8)((ubgs&0xFF00)>>8);
        	MCF_UART2_UBG2 = (uint8)(ubgs&0x00FF);

        	MCF_INTC0_IMRL &=~ MCF_INTC_IMRL_MASKALL;//清零屏蔽所有中断
            MCF_INTC0_IMRL &=~ MCF_INTC_IMRL_INT_MASK15;//设置中断源号为15，实际位置为，15+64
            //              中断优先级，最高7级，中断级别，最高3级
            MCF_INTC0_ICR15 = MCF_INTC_ICR_IP(5) |MCF_INTC_ICR_IL(3);

        	MCF_UART2_UCR = MCF_UART_UCR_RX_ENABLED | MCF_UART_UCR_TX_ENABLED;//使能串口接收发送

        }break;
	}
}
void TUart0_Putchar(char data){
    while(!(MCF_UART0_USR & MCF_UART_USR_TXEMP)){};
    MCF_UART0_UTB = data;
}

void TUart0_Puts(char *data){
    while(*data) TUart0_Putchar(*data++);
}

void TUart0_DMAInit(){

    MCF_DMA0_SAR = (vuint32)TXBuffer;
    MCF_DMA0_DAR = (vuint32)0x4000020C;
    MCF_DMA0_BCR = 0;

    //MCF_SCM_MPR |= 0x04;              //使DMA控制器为管理员权限
    MCF_SCM_DMAREQC |= MCF_SCM_DMAREQC_DMAC0(0x0C);        //使用DMA0模块请求源UART0发送器
    MCF_SCM_PACR2 = MCF_SCM_PACR_ACCESS_CTRL1(6);           //设置DMA的用户/管理员 访问权限 --- 可读/可写
    MCF_SCM_GPACR0 = MCF_SCM_GPACR_ACCESS_CTRL(6);
    //---2015.5.2---MCF_SCM_RAMBAR |= MCF_SCM_RAMBAR_BDE;//使能非核心模块(这里指DMA)访问SRAM
    /*               周期窃取         源地址数据为字节型     目的地址数据为字节型         源地址自动增加          自动清零*/
    MCF_DMA0_DCR = MCF_DMA_DCR_CS | MCF_DMA_DCR_SSIZE(1) | MCF_DMA_DCR_DSIZE(1) | MCF_DMA_DCR_SINC | MCF_DMA_DCR_D_REQ;

    /*Disable interrupts using the UIMR register. The appropriate UIMR bits must be cleared so that
    interrupt requests are disabled for those conditions for which a DMA request is desired. For
    example, to generate transmit DMA requests from UART1, UIMR1[TXRDY] should be cleared.
    This prevents TXRDY from generating an interrupt request while a transmit DMA request is
    generated.
    */

    /*
    文档里有一段话：不要混淆RAMBAR寄存器和SCM RAMBAR寄存器，虽然名字相似，但RAMBAR寄存器是
    内核寄存器用来使内核能够访问SRAM内存，而SCM RAMBAR寄存器是用来让外设能够访问SRAM内存的
    */

    /*
    烦冷火的DMA，功能不强还这么难用，不如STM32，怪不得不流行，渣渣~~~
    */
    MCF_INTC0_IMRL &=~ MCF_INTC_IMRL_MASKALL;                   //清零屏蔽所有中断
    MCF_INTC0_IMRL &=~ MCF_INTC_IMRL_INT_MASK9;                 //设置中断源号为9，实际位置为9+64
    MCF_INTC0_ICR09 = MCF_INTC_ICR_IP(5) |MCF_INTC_ICR_IL(2);   //设置中断优先级
    /*              允许外部请求    */
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
    if(MCF_UART0_USR&MCF_UART_USR_RXRDY){
        a = MCF_UART0_URB;
        sprintf(TXBuffer,"data:%u\n",a);
        TUart0_Puts(TXBuffer);
    }

}
__declspec(interrupt) void UART1_interrupt(void){

    uint8 a;
    if(MCF_UART1_USR&MCF_UART_USR_RXRDY){
        a = MCF_UART1_URB;
        switch(a){
            case 200:{
                startend = 1;
                TUart1_Puts("停车\n");
             }break;
            case 201:{
                startend = 0;
                TUart1_Puts("启动\n");
             }break;
            case 202:{
                uartMode = 1;
                TUart1_Puts("Change P\n");
            }break;
            case 203:{
                uartMode = 2;
                TUart1_Puts("Change I\n");
            }break;
            case 204:{
                uartMode = 3;
                TUart1_Puts("Change D\n");
            }break;
            case 210:{
                baseSpeed++;
                sprintf(TXBuffer,"S:%u\n",baseSpeed);
                TUart1_Puts(TXBuffer);
            }break;
            case 211:{
                baseSpeed--;
                sprintf(TXBuffer,"S:%u\n",baseSpeed);
                TUart1_Puts(TXBuffer);
            }break;
            case 212:{
                baseSpeed+=5;
                sprintf(TXBuffer,"S:%u\n",baseSpeed);
                TUart1_Puts(TXBuffer);
            }break;
            case 213:{
                baseSpeed-=5;
                sprintf(TXBuffer,"S:%u\n",baseSpeed);
                TUart1_Puts(TXBuffer);
            }break;
            default:{
                switch(uartMode){
                    case 1:{
                        Kp_a = a/100.0;
                        sprintf(TXBuffer,"P:0.%u\n",a);
                        TUart1_Puts(TXBuffer);
                    }break;
                    case 2:{
                        Ki_a = a/100.0;
                        sprintf(TXBuffer,"I:0.%u\n",a);
                        TUart1_Puts(TXBuffer);
                    }break;
                    case 3:{
                        Kd_a = a/100.0;
                        sprintf(TXBuffer,"D:0.%u\n",a);
                        TUart1_Puts(TXBuffer);
                    }break;
                }
            }
        }
        //sprintf(TXBuffer,"data:%u\n",a);
        //TUart1_Puts(TXBuffer);
    }
}
__declspec(interrupt) void UART2_interrupt(void){

    uint8 a;
    //TUart2_Putchar('C');
    if(MCF_UART2_USR&MCF_UART_USR_RXRDY){
        a = MCF_UART2_URB;
        //TUart2_Putchar(a);
        //TUart2_Putchar('+');
    }

}
#endif
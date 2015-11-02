#include "DIY_ComDef.h"
#ifdef TZ_MCF52259_I2C

void TI2Cx_Init(uint8 mode){

    switch(mode){
        case 0:{
            MCF_GPIO_PASPAR |= MCF_GPIO_PASPAR_SCL0_SCL0 | MCF_GPIO_PASPAR_SDA0_SDA0;
            MCF_I2C0_I2FDR = MCF_I2C_I2FDR_IC(0x3a);        //80M / 896 < 100k 只要别高于100k就行，不知道为啥的I2C原理肯定不懂
            /*                  使能主机模式     使能I2C0*/
            MCF_I2C0_I2CR = MCF_I2C_I2CR_MSTA | MCF_I2C_I2CR_IEN;
            MCF_I2C0_I2SR = 0;

        }break;
        case 1:{
            MCF_GPIO_PQSPAR |= MCF_GPIO_PQSPAR_QSPI_DOUT_SCL1 | MCF_GPIO_PQSPAR_QSPI_DIN_SDA1;
            MCF_I2C1_I2FDR = MCF_I2C_I2FDR_IC(0x3a);        //80M / 896 < 100k 只要别高于100k就行，不知道为啥的I2C原理肯定不懂
            /*                  使能主机模式     使能I2C1*/
            MCF_I2C1_I2CR = MCF_I2C_I2CR_MSTA | MCF_I2C_I2CR_IEN;

        }break;
    }
}
/**********************************************************************

    2015.4.27由于I2C模块对于本程序运行速度影响不大，只在开始时用一下，所以直接拷贝用之，
    如果在运行中会多次用到I2C，像如直立车，则需要改进，拷贝的代码存在大量
    代码冗余和时间占用(轮训的结果)，所以需要改用中断实现
    还有就是摄像头用的SCCB协议不能用I2C替换，需要自己写

***********************************************************************/
void iic_delay()
{
	asm{};asm{};asm{};asm{};asm{};
	asm{};asm{};asm{};asm{};asm{};
	asm{};asm{};asm{};asm{};asm{};
	asm{};asm{};asm{};asm{};asm{};
	asm{};asm{};asm{};asm{};asm{};
	asm{};asm{};asm{};asm{};asm{};
	asm{};asm{};asm{};asm{};asm{};
	asm{};asm{};asm{};asm{};asm{};
	asm{};asm{};asm{};asm{};asm{};
	asm{};asm{};asm{};asm{};asm{};
	asm{};asm{};asm{};asm{};asm{};
	asm{};asm{};asm{};asm{};asm{};
}
void I2CsendByte(uint8 data, uint8 address, uint8 id)
{
	//mcf5xxx_irq_disable();

	/* setting in Tx mode */
	MCF_I2C0_I2CR |= MCF_I2C_I2CR_MTX;

	/* generates start condition */
	MCF_I2C0_I2CR |= MCF_I2C_I2CR_MSTA;

	/* set devide ID to write */
	MCF_I2C0_I2DR = id;

	/* wait until one byte transfer completion */
	while( !(MCF_I2C0_I2SR & MCF_I2C_I2SR_IIF ))
	;
	TUart1_Puts("1");
	/* clear the completion transfer flag */
	MCF_I2C0_I2SR &= ~MCF_I2C_I2SR_IIF;

	/* memory address */
	MCF_I2C0_I2DR = address;
	iic_delay();
	/* wait until one byte transfer completion */
	while( !(MCF_I2C0_I2SR & MCF_I2C_I2SR_IIF ))
	;
	TUart1_Puts("2");
	/* clear the completion transfer flag */
	MCF_I2C0_I2SR &= ~MCF_I2C_I2SR_IIF;

	/* memory data */
	MCF_I2C0_I2DR = data;
	/* wait until one byte transfer completion */
	iic_delay();
	while( !(MCF_I2C0_I2SR & MCF_I2C_I2SR_IIF ))
	;
	TUart1_Puts("3");
	/* clear the completion transfer flag */
	MCF_I2C0_I2SR &= ~MCF_I2C_I2SR_IIF;
	/* generates stop condition */
	MCF_I2C0_I2CR &= ~MCF_I2C_I2CR_MSTA;
	//mcf5xxx_irq_enable();
	return;
}
uint8 I2CreceiveByte(uint8 address, uint8 id)
{
	uint8 data;

	/* setting in Tx mode */
	MCF_I2C0_I2CR |= MCF_I2C_I2CR_MTX;

	/* send start condition */
	MCF_I2C0_I2CR |= MCF_I2C_I2CR_MSTA;

	/* devide ID to write */
	MCF_I2C0_I2DR = id;

	/* wait until one byte transfer completion */
	while( !(MCF_I2C0_I2SR & MCF_I2C_I2SR_IIF ))
	;
	/* clear the completion transfer flag */
	MCF_I2C0_I2SR &= ~MCF_I2C_I2SR_IIF;

	/* memory address */
	MCF_I2C0_I2DR = address;

	/* wait until one byte transfer completion */
	while( !(MCF_I2C0_I2SR & MCF_I2C_I2SR_IIF ))
	;
	/* clear the completion transfer flag */
	MCF_I2C0_I2SR &= ~MCF_I2C_I2SR_IIF;

	/* resend start */
	MCF_I2C0_I2CR |= MCF_I2C_I2CR_RSTA;

	/* device id to read */
	MCF_I2C0_I2DR = (uint8)(id | 0x01);

	/* wait until one byte transfer completion */
	while( !(MCF_I2C0_I2SR & MCF_I2C_I2SR_IIF ))
	;
	/* clear the completion transfer flag */
	MCF_I2C0_I2SR &= ~MCF_I2C_I2SR_IIF;

	/* setting in Rx mode */
	MCF_I2C0_I2CR &= ~MCF_I2C_I2CR_MTX;

	/* send NO ACK */
	MCF_I2C0_I2CR |= MCF_I2C_I2CR_TXAK;

	/* dummy read */
	data = MCF_I2C0_I2DR;

	/* wait until one byte transfer completion */
	while( !(MCF_I2C0_I2SR & MCF_I2C_I2SR_IIF ))
	;
	/* clear the completion transfer flag */
	MCF_I2C0_I2SR &= ~MCF_I2C_I2SR_IIF;

	/* read data received */
	data = MCF_I2C0_I2DR;

	/* wait until one byte transfer completion */
	while( !(MCF_I2C0_I2SR & MCF_I2C_I2SR_IIF ))
	;
	/* clear the completion transfer flag */
	MCF_I2C0_I2SR &= ~MCF_I2C_I2SR_IIF;

	/* generates stop condition */
	MCF_I2C0_I2CR &= ~MCF_I2C_I2CR_MSTA;

	/* send the received data */
	return data;
}

#endif

#include "DIY_ComDef.h"
#ifdef TZ_CAMERA_SCCB

/*
uint8 SCCB_Start(void)
{
    W_SDA = 1;
    W_SCL = 1;
    Delay1us(1);

    DDR_SDA = 0;
    if(!R_SDA){
        DDR_SDA = 1;
        TUart0_Puts("Bus Busy!\r\n");
        return 0;
    }
    DDR_SDA = 1;
    W_SDA = 0;
    Delay1us(1);

    W_SCL = 0;

    DDR_SDA = 0;
    if(R_SDA){
        DDR_SDA = 1;
        TUart0_Puts("Bus Error!\r\n");
        return 0;
    }

}
void SCCB_Stop(void)
{
    W_SCL = 0;
    //Delay1us(1);
    W_SDA = 0;
    Delay1us(1);
    W_SCL = 1;
    Delay1us(1);
    W_SDA = 1;
    Delay1us(1);
}
void SCCB_Ack(void)
{
    W_SCL = 0;
    Delay1us(1);
    W_SDA = 0;
    Delay1us(1);
    W_SCL = 1;
    Delay1us(1);
    W_SCL = 0;
    Delay1us(1);
}
void SCCB_NoAck(void)
{
    W_SCL = 0;
    Delay1us(1);
    W_SDA = 1;
    Delay1us(1);
    W_SCL = 1;
    Delay1us(1);
    W_SCL = 0;
    Delay1us(1);
}
uint8 SCCB_WaitAck(void)
{
    W_SCL = 0;
    //SDA_H();
    DDR_SDA = 0;

    Delay1us(1);      // 延时1us
    W_SCL = 1;

    Delay1us(1);      // 延时1us

    if(R_SDA)           //应答为高电平，异常，通信失败
    {
        DDR_SDA = 1;
        W_SCL = 0;
        TUart0_Puts("ACK Error!\r\n");
        return 0;
    }
    DDR_SDA = 1;
    W_SCL = 0;
    return 1;
}

void SCCB_SendByte(uint8 ch){

    uint8 n = 8;     // 向BFSDA上发送一位数据字节，共八位
    while(n--)
    {

        if(ch & 0x80)    // 若要发送的数据最高位为1则发送位1
        {
            W_SDA = 1;    // 传送位1
        }
        else
        {
            W_SDA = 0;    // 否则传送位0
        }
        ch = ch<<1;    // 数据左移一位
        Delay1us(1);      // 延时1us
        W_SCL = 1;                //SCL 拉高，采集信号
        Delay1us(1);      // 延时1us
        W_SCL = 0;                  //SCL 时钟线拉低
    }
}
uint8 SCCB_ReceiveByte(void){

    uint8 n = 8;    // 从BFSDA线上读取一上数据字节，共八位
    uint8 tdata = 0;
    DDR_SDA = 0;

    while(n--)
    {
        tdata = tdata<<1;    // 左移一位，或_crol_(temp,1)
        W_SCL = 0;
        Delay1us(1);
        W_SCL = 1;
        Delay1us(1);

        if(R_SDA)
            tdata = tdata|0x01;    // 若接收到的位为1，则数据的最后一位置1

    }
    DDR_SDA = 1;
    W_SCL = 0;
    return(tdata);
}

int SCCB_WriteByte( uint16 WriteAddress , uint8 SendByte )            //考虑到用sccb的管脚模拟，比较容易失败，因此多试几次
{
    uint8 i = 0;
    while( 0 == SCCB_WriteByte_one ( WriteAddress, SendByte ) )
    {
        i++;
        if(i == 20)
        {
            TUart0_Puts("Configure Error!\r\n");
            return 0 ;
        }
    }
    return 1;
}

int SCCB_WriteByte_one( uint16 WriteAddress , uint8 SendByte )
{
    if(!SCCB_Start())
    {
        return 0;
    }
    SCCB_SendByte( DEV_ADR );                    // 器件地址
    if( !SCCB_WaitAck() )
    {
        SCCB_Stop();
        return 0;
    }
    TUart0_Puts("Sending Command!\r\n");
    SCCB_SendByte((uint8)(WriteAddress & 0x00FF));   // 设置低起始地址
    SCCB_WaitAck();
    SCCB_SendByte(SendByte);
    SCCB_WaitAck();
    SCCB_Stop();
    return 1;
}

int SCCB_ReadByte(uint8 *pBuffer,   uint16 length,   uint8 ReadAddress)
{
    uint8 i = 0;
    while( 0 == SCCB_ReadByte_one(pBuffer, length, ReadAddress) )
    {
        i++;
        if(i == 30)
        {
            return 0 ;
        }
    }
    return 1;
}

int SCCB_ReadByte_one(uint8 *pBuffer,   uint16 length,   uint8 ReadAddress)
{
    if(!SCCB_Start())
    {
        return 0;
    }
    SCCB_SendByte( DEV_ADR );         // 器件地址
    if( !SCCB_WaitAck() )
    {
        SCCB_Stop();
        return 0;
    }
    SCCB_SendByte( ReadAddress );           // 设置低起始地址
    SCCB_WaitAck();
    SCCB_Stop();

    if(!SCCB_Start())
    {
        return 0;
    }
    SCCB_SendByte( DEV_ADR + 1 );               // 器件地址

    if(!SCCB_WaitAck())
    {
        SCCB_Stop();
        return 0;
    }
    while(length)
    {
        *pBuffer = SCCB_ReceiveByte();
        if(length == 1)
        {
            SCCB_NoAck();
        }
        else
        {
            SCCB_Ack();
        }
        pBuffer++;
        length--;
    }
    SCCB_Stop();
    return 1;
}
*/

static uint8 SCCB_Start(void)
{
    SDA_H();
    SCL_H();
    SCCB_DELAY();

    SDA_DDR_IN();
    if(!SDA_IN())
    {
        SDA_DDR_OUT();
        return 0;   /* SDA线为低电平则总线忙,退出 */
    }
    SDA_DDR_OUT();
    SDA_L();
    SCCB_DELAY();

    SCL_L();

    if(SDA_IN())
    {
        SDA_DDR_OUT();
        return 0;   /* SDA线为高电平则总线出错,退出 */
    }
    //SDA_DDR_OUT();
    //SDA_L();
    //SCCB_delay();
    return 1;
}

/*!
 *  @brief      SCCB停止信号
 *  @since      v5.0
 */
static void SCCB_Stop(void)
{
    SCL_L();
    //SCCB_DELAY();
    SDA_L();
    SCCB_DELAY();
    SCL_H();
    SCCB_DELAY();
    SDA_H();
    SCCB_DELAY();
}

/*!
 *  @brief      SCCB应答信号
 *  @since      v5.0
 */
static void SCCB_Ack(void)
{
    SCL_L();
    SCCB_DELAY();
    SDA_L();
    SCCB_DELAY();
    SCL_H();
    SCCB_DELAY();
    SCL_L();
    SCCB_DELAY();
}

/*!
 *  @brief      SCCB无应答信号
 *  @since      v5.0
 */
static void SCCB_NoAck(void)
{
    SCL_L();
    SCCB_DELAY();
    SDA_H();
    SCCB_DELAY();
    SCL_H();
    SCCB_DELAY();
    SCL_L();
    SCCB_DELAY();
}

/*!
 *  @brief      SCCB 等待应答
 *  @return     应答结果（0表示无应答，1表示有应答）
 *  @since      v5.0
 */
static int SCCB_WaitAck(void)
{
    SCL_L();
    //SDA_H();
    SDA_DDR_IN();

    SCCB_DELAY();
    SCL_H();

    SCCB_DELAY();

    if(SDA_IN())           //应答为高电平，异常，通信失败
    {
        SDA_DDR_OUT();
        SCL_L();
        return 0;
    }
    SDA_DDR_OUT();
    SCL_L();
    return 1;
}

/*!
 *  @brief      SCCB 发送的数据
 *  @param      SendByte    需要发送的数据
 *  @since      v5.0
 */
static void SCCB_SendByte(uint8 SendByte)
{
    uint8 i = 8;
    while(i--)
    {

        if(SendByte & 0x80)     //SDA 输出数据
        {
            SDA_H();
        }
        else
        {
            SDA_L();
        }
        SendByte <<= 1;
        SCCB_DELAY();
        SCL_H();                //SCL 拉高，采集信号
        SCCB_DELAY();
        SCL_L();                //SCL 时钟线拉低
        //SCCB_DELAY();
    }
    //SCL_L();
}

/*!
 *  @brief      接收SCCB总线的数据
 *  @return     接收到的数据
 *  @since      v5.0
 */
static int SCCB_ReceiveByte(void)
{
    uint8 i = 8;
    uint8 ReceiveByte = 0;

    //SDA_H();
    //SCCB_DELAY();
    SDA_DDR_IN();

    while(i--)
    {
        ReceiveByte <<= 1;
        SCL_L();
        SCCB_DELAY();
        SCL_H();
        SCCB_DELAY();

        if(SDA_IN())
        {
            ReceiveByte |= 0x01;
        }


    }
    SDA_DDR_OUT();
    SCL_L();
    return ReceiveByte;
}

/*****************************************************************************************
* 函数名：SCCB_WriteByte
* 描述  ：写一字节数据
* 输入  ：- WriteAddress: 待写入地址    - SendByte: 待写入数据  - DeviceAddress: 器件类型
* 输出  ：返回为:=1成功写入,=0失败
* 注意  ：无
*****************************************************************************************/
static int SCCB_WriteByte_one( uint16 WriteAddress , uint8 SendByte );


int SCCB_WriteByte( uint16 WriteAddress , uint8 SendByte )            //考虑到用sccb的管脚模拟，比较容易失败，因此多试几次
{
    uint8 i = 0;
    while( 0 == SCCB_WriteByte_one ( WriteAddress, SendByte ) )
    {
        i++;
        if(i == 20)
        {
            return 0 ;
        }
    }
    return 1;
}

int SCCB_WriteByte_one( uint16 WriteAddress , uint8 SendByte )
{
    if(!SCCB_Start())
    {
        return 0;
    }
    SCCB_SendByte( DEV_ADR );                    /* 器件地址 */
    if( !SCCB_WaitAck() )
    {
        SCCB_Stop();
        return 0;
    }
    SCCB_SendByte((uint8)(WriteAddress & 0x00FF));   /* 设置低起始地址 */
    SCCB_WaitAck();
    SCCB_SendByte(SendByte);
    SCCB_WaitAck();
    SCCB_Stop();
    return 1;
}




/******************************************************************************************************************
 * 函数名：SCCB_ReadByte
 * 描述  ：读取一串数据
 * 输入  ：- pBuffer: 存放读出数据  - length: 待读出长度    - ReadAddress: 待读出地址        - DeviceAddress: 器件类型
 * 输出  ：返回为:=1成功读入,=0失败
 * 注意  ：无
 **********************************************************************************************************************/
static int SCCB_ReadByte_one(uint8 *pBuffer,   uint16 length,   uint8 ReadAddress);

int SCCB_ReadByte(uint8 *pBuffer,   uint16 length,   uint8 ReadAddress)
{
    uint8 i = 0;
    while( 0 == SCCB_ReadByte_one(pBuffer, length, ReadAddress) )
    {
        i++;
        if(i == 30)
        {
            return 0 ;
        }
    }
    return 1;
}

int SCCB_ReadByte_one(uint8 *pBuffer,   uint16 length,   uint8 ReadAddress)
{
    if(!SCCB_Start())
    {
        return 0;
    }
    SCCB_SendByte( DEV_ADR );         /* 器件地址 */
    if( !SCCB_WaitAck() )
    {
        SCCB_Stop();
        return 0;
    }
    SCCB_SendByte( ReadAddress );           /* 设置低起始地址 */
    SCCB_WaitAck();
    SCCB_Stop();

    if(!SCCB_Start())
    {
        return 0;
    }
    SCCB_SendByte( DEV_ADR + 1 );               /* 器件地址 */

    if(!SCCB_WaitAck())
    {
        SCCB_Stop();
        return 0;
    }
    while(length)
    {
        *pBuffer = SCCB_ReceiveByte();
        if(length == 1)
        {
            SCCB_NoAck();
        }
        else
        {
            SCCB_Ack();
        }
        pBuffer++;
        length--;
    }
    SCCB_Stop();
    return 1;
}

#endif
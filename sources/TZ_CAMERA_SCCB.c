
#include "DIY_ComDef.h"
#ifdef TZ_CAMERA_SCCB
/*
*****2015.5.15 ����ͷSCCBģ��
*****���ܣ�SCCBЭ������
*****��д����������Ҫ���⣺��
    1.�����Լ����������������ģ���޹ؽ�Ҫ�����Ծ�ֱ����ֲ�˹ٷ�K60�ĳ���
    ��Ҳ��Ϊ������ļ��������Լ�д�ĳ���ϣ��������˿����Լ�дһ��
��
*/

/*
//�Լ�д��SCCBЭ��
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

    Delay1us(1);      // ��ʱ1us
    W_SCL = 1;

    Delay1us(1);      // ��ʱ1us

    if(R_SDA)           //Ӧ��Ϊ�ߵ�ƽ���쳣��ͨ��ʧ��
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

    uint8 n = 8;     // ��BFSDA�Ϸ���һλ�����ֽڣ�����λ
    while(n--)
    {

        if(ch & 0x80)    // ��Ҫ���͵��������λΪ1����λ1
        {
            W_SDA = 1;    // ����λ1
        }
        else
        {
            W_SDA = 0;    // ������λ0
        }
        ch = ch<<1;    // ��������һλ
        Delay1us(1);      // ��ʱ1us
        W_SCL = 1;                //SCL ���ߣ��ɼ��ź�
        Delay1us(1);      // ��ʱ1us
        W_SCL = 0;                  //SCL ʱ��������
    }
}
uint8 SCCB_ReceiveByte(void){

    uint8 n = 8;    // ��BFSDA���϶�ȡһ�������ֽڣ�����λ
    uint8 tdata = 0;
    DDR_SDA = 0;

    while(n--)
    {
        tdata = tdata<<1;    // ����һλ����_crol_(temp,1)
        W_SCL = 0;
        Delay1us(1);
        W_SCL = 1;
        Delay1us(1);

        if(R_SDA)
            tdata = tdata|0x01;    // �����յ���λΪ1�������ݵ����һλ��1

    }
    DDR_SDA = 1;
    W_SCL = 0;
    return(tdata);
}

int SCCB_WriteByte( uint16 WriteAddress , uint8 SendByte )            //���ǵ���sccb�Ĺܽ�ģ�⣬�Ƚ�����ʧ�ܣ���˶��Լ���
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
    SCCB_SendByte( DEV_ADR );                    // ������ַ
    if( !SCCB_WaitAck() )
    {
        SCCB_Stop();
        return 0;
    }
    TUart0_Puts("Sending Command!\r\n");
    SCCB_SendByte((uint8)(WriteAddress & 0x00FF));   // ���õ���ʼ��ַ
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
    SCCB_SendByte( DEV_ADR );         // ������ַ
    if( !SCCB_WaitAck() )
    {
        SCCB_Stop();
        return 0;
    }
    SCCB_SendByte( ReadAddress );           // ���õ���ʼ��ַ
    SCCB_WaitAck();
    SCCB_Stop();

    if(!SCCB_Start())
    {
        return 0;
    }
    SCCB_SendByte( DEV_ADR + 1 );               // ������ַ

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
        return 0;   /* SDA��Ϊ�͵�ƽ������æ,�˳� */
    }
    SDA_DDR_OUT();
    SDA_L();
    SCCB_DELAY();

    SCL_L();

    if(SDA_IN())
    {
        SDA_DDR_OUT();
        return 0;   /* SDA��Ϊ�ߵ�ƽ�����߳���,�˳� */
    }
    //SDA_DDR_OUT();
    //SDA_L();
    //SCCB_delay();
    return 1;
}

/*!
 *  @brief      SCCBֹͣ�ź�
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
 *  @brief      SCCBӦ���ź�
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
 *  @brief      SCCB��Ӧ���ź�
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
 *  @brief      SCCB �ȴ�Ӧ��
 *  @return     Ӧ������0��ʾ��Ӧ��1��ʾ��Ӧ��
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

    if(SDA_IN())           //Ӧ��Ϊ�ߵ�ƽ���쳣��ͨ��ʧ��
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
 *  @brief      SCCB ���͵�����
 *  @param      SendByte    ��Ҫ���͵�����
 *  @since      v5.0
 */
static void SCCB_SendByte(uint8 SendByte)
{
    uint8 i = 8;
    while(i--)
    {

        if(SendByte & 0x80)     //SDA �������
        {
            SDA_H();
        }
        else
        {
            SDA_L();
        }
        SendByte <<= 1;
        SCCB_DELAY();
        SCL_H();                //SCL ���ߣ��ɼ��ź�
        SCCB_DELAY();
        SCL_L();                //SCL ʱ��������
        //SCCB_DELAY();
    }
    //SCL_L();
}

/*!
 *  @brief      ����SCCB���ߵ�����
 *  @return     ���յ�������
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
* ��������SCCB_WriteByte
* ����  ��дһ�ֽ�����
* ����  ��- WriteAddress: ��д���ַ    - SendByte: ��д������  - DeviceAddress: ��������
* ���  ������Ϊ:=1�ɹ�д��,=0ʧ��
* ע��  ����
*****************************************************************************************/
static int SCCB_WriteByte_one( uint16 WriteAddress , uint8 SendByte );


int SCCB_WriteByte( uint16 WriteAddress , uint8 SendByte )            //���ǵ���sccb�Ĺܽ�ģ�⣬�Ƚ�����ʧ�ܣ���˶��Լ���
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
    SCCB_SendByte( DEV_ADR );                    /* ������ַ */
    if( !SCCB_WaitAck() )
    {
        SCCB_Stop();
        return 0;
    }
    SCCB_SendByte((uint8)(WriteAddress & 0x00FF));   /* ���õ���ʼ��ַ */
    SCCB_WaitAck();
    SCCB_SendByte(SendByte);
    SCCB_WaitAck();
    SCCB_Stop();
    return 1;
}




/******************************************************************************************************************
 * ��������SCCB_ReadByte
 * ����  ����ȡһ������
 * ����  ��- pBuffer: ��Ŷ�������  - length: ����������    - ReadAddress: ��������ַ        - DeviceAddress: ��������
 * ���  ������Ϊ:=1�ɹ�����,=0ʧ��
 * ע��  ����
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
    SCCB_SendByte( DEV_ADR );         /* ������ַ */
    if( !SCCB_WaitAck() )
    {
        SCCB_Stop();
        return 0;
    }
    SCCB_SendByte( ReadAddress );           /* ���õ���ʼ��ַ */
    SCCB_WaitAck();
    SCCB_Stop();

    if(!SCCB_Start())
    {
        return 0;
    }
    SCCB_SendByte( DEV_ADR + 1 );               /* ������ַ */

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
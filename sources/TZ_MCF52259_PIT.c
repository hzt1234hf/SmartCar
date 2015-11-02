#include "DIY_ComDef.h"
#ifdef TZ_MCF52259_PIT
/*
*****2015.4.21 PIT模块编写完毕
*****功能：PIT可编程定时器模块进行定时
*****编写中遇到的主要问题：（
    1.预分频器和MCF_PIT0_PMR技术值需要注意计算
）
*/

void TPITx_Init(uint8 mode){
    switch(mode){
        default:case 0:{
            MCF_PIT0_PCSR = 0;
            //                 自动重载         计数为0中断         使能中断            立即载入PMR值       80000000 / 2^10 = 78125 2^11 = 39062 2^12=19531
            MCF_PIT0_PCSR = MCF_PIT_PCSR_RLD | MCF_PIT_PCSR_PIF | MCF_PIT_PCSR_PIE | MCF_PIT_PCSR_OVW | MCF_PIT_PCSR_PRE(11);
            MCF_PIT0_PMR = 19531 * 1;   //设置计数值
        	MCF_INTC0_IMRH &=~ MCF_INTC_IMRH_INT_MASK55;                //设置中断源号为55，实际位置为55+64
        	MCF_INTC0_ICR55 = MCF_INTC_ICR_IP(4) |MCF_INTC_ICR_IL(2);   //设置中断优先级
            //MCF_PIT0_PCSR |= MCF_PIT_PCSR_EN;
        }break;
        case 1:{
            MCF_PIT1_PCSR = 0;
            //                 自动重载         计数为0中断         使能中断            立即载入PMR值       80000000 / 2^10 = 78125 2^11 = 39062 2^12=19531
            MCF_PIT1_PCSR = MCF_PIT_PCSR_RLD | MCF_PIT_PCSR_PIF | MCF_PIT_PCSR_PIE | MCF_PIT_PCSR_OVW | MCF_PIT_PCSR_PRE(11);
            MCF_PIT1_PMR = 19531 * 1;   //设置计数值
        	MCF_INTC0_IMRH &=~ MCF_INTC_IMRH_INT_MASK56;                //设置中断源号为55，实际位置为55+64
        	MCF_INTC0_ICR56 = MCF_INTC_ICR_IP(4) |MCF_INTC_ICR_IL(2);   //设置中断优先级
            //MCF_PIT1_PCSR |= MCF_PIT_PCSR_EN;
        }break;
    }
}
__declspec(interrupt:0) void TPIT0_interrupt(void){

#if 1
    //DMA自动输出
    TUart0_Putchar(0x01);
    TUart0_Putchar(0xFE);

    TUart0_DMAPutBuffer(Image[0],IMG_SIZE);

    TUart0_Putchar(0xFE);
    TUart0_Putchar(0x01);
#elif 0
    //DMA自动输出
    Image_ToPC[0] = 0x01;
    Image_ToPC[1] = 0xFE;
    Image_ToPC[IMG_SIZE+2] = 0xFE;
    Image_ToPC[IMG_SIZE+3] = 0x01;
    TUart0_DMAPutBuffer(Image_ToPC,IMG_SIZE+4);

#elif 0

    TUart0_Putchar(0);
    TUart0_Putchar(255);
    TUart0_Putchar(1);
    TUart0_Putchar(0);
    TUart0_DMAPutBuffer(Image[0],IMG_SIZE);

#elif 0       //循环串口输出
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
    MCF_PIT0_PCSR |= MCF_PIT_PCSR_PIF;  //清中断标志

}
__declspec(interrupt:0) void TPIT1_interrupt(void){

#if 0
/***********************************/
/*      8位检测全图实现提取边线     */
/***********************************/
    uint8 row,col,col2,data;
    for(row = 0; row<120; row++){
        for(col = 0,col2 = 1;col < 20; col++,col2++){
            if(col2>=20)
                col2 = 0;
            data = Image[row][col2];
            if(data==255){
                if(Image[row][col2+1] !=0)
                    Image_Ptr[row * 20 + col] = 0;
                else
                    Image_Ptr[row * 20 + col] = 0x01;
            }else if(data==0){
                if(Image[row][col2+1] !=255)
                    Image_Ptr[row * 20 + col] = 0;
                else
                    Image[row][col2+1] = 0x80;
            }else{
                switch(data){
                    case 0x01:Image_Ptr[row * 20 + col] = 0x01;break;
                    case 0xFE:case 0x03:Image_Ptr[row * 20 + col] = 0x02;break;
                    case 0xFC:case 0x07:Image_Ptr[row * 20 + col] = 0x04;break;
                    case 0xF8:case 0x0F:Image_Ptr[row * 20 + col] = 0x08;break;
                    case 0xF0:case 0x1F:Image_Ptr[row * 20 + col] = 0x10;break;
                    case 0xE0:case 0x3F:Image_Ptr[row * 20 + col] = 0x20;break;
                    case 0xC0:case 0x7F:Image_Ptr[row * 20 + col] = 0x40;break;
                    case 0x80:Image_Ptr[row * 20 + col] = 0x80;break;
                    default:Image_Ptr[row * 20 + col] = 0xff;
                }
            }
        }
    }
    for(row = 0;row<40;row++){
        Image_Ptr[row * 60 + 9] |= 0x01;
        Image_Ptr[row * 60 + 10] |= 0x80;
    }
#elif 1
/***********************************/
/*      8位扫描实现提取边线         */
/***********************************/
    vuint8 data;
    vuint8 row,col;
    vuint8 leftJP,rightJP;
    for(row = 0; row<120; row++){
        data = Image[row][0];
        for(col = 0;col < 19; col++){
            Image[row][col] = Image[row][col + 1];
        }
        Image[row][col] = data;
    }
    leftJP = 9;
    rightJP = 10;
    for(row = 119;;row--){
        for(col = leftJP;col>=0;){
            if(0&&Image[row][col] == 255){
                leftJP = rightJP;           //如果出现这种情况，那么左边其实点与右边起始点对齐
                continue;                   //略过此次循环，导致col并没有改变，从而实现重新检测这一行
            }else if(Image[row][col] == 0){
                if(col > 0 && Image[row][col-1] == 255){
                    Image_Ptr[row * 20 + col-1] = 0x01;
                    leftJP = col + 4;
                    break;
                }else if(col == 0){
                    Image_Ptr[row * 20 + col] = 0x01;
                    break;
                }
            }else{
                switch(Image[row][col]){
                    case 0xFE:Image_Ptr[row * 20 + col] = 0x02;break;
                    case 0xFC:Image_Ptr[row * 20 + col] = 0x04;break;
                    case 0xF8:Image_Ptr[row * 20 + col] = 0x08;break;
                    case 0xF0:Image_Ptr[row * 20 + col] = 0x10;break;
                    case 0xE0:Image_Ptr[row * 20 + col] = 0x20;break;
                    case 0xC0:Image_Ptr[row * 20 + col] = 0x40;break;
                    case 0x80:Image_Ptr[row * 20 + col] = 0x80;break;
                }
                break;
                leftJP = col + 4;
            }
            col--;
        }
        for(col = rightJP;col<=19;){
            if(0&&Image[row][col] == 255){
                rightJP = leftJP;           //如果出现这种情况，那么右边起始点与左边起始点对齐
                continue;                   //略过此次循环，导致col并没有改变，从而实现重新检测这一行
            }else if(Image[row][col] == 0){
                if(col < 19 && Image[row][col+1] == 255){
                    Image_Ptr[row * 20 + col+1] = 0x80;
                    rightJP = col - 1;
                    break;
                }else if(col == 19){
                    break;
                }
            }else{
                switch(Image[row][col]){
                    case 0x01:Image_Ptr[row * 20 + col] = 0x01;break;
                    case 0x03:Image_Ptr[row * 20 + col] = 0x02;break;
                    case 0x07:Image_Ptr[row * 20 + col] = 0x04;break;
                    case 0x0F:Image_Ptr[row * 20 + col] = 0x08;break;
                    case 0x1F:Image_Ptr[row * 20 + col] = 0x10;break;
                    case 0x3F:Image_Ptr[row * 20 + col] = 0x20;break;
                    case 0x7F:Image_Ptr[row * 20 + col] = 0x40;break;
                }
                break;
                rightJP = col - 1;
            }
            col++;
        }
        if(row == 0)
            break;
        if( rightJP < leftJP ){//保证左右开始不会有交叉
            rightJP = (rightJP + leftJP)/2;
            leftJP = rightJP;
        }

    }
#elif 0
/***********************************/
/*     上位机显示偏移校正后的图像   */
/***********************************/
    uint8 row,col,col2,data;
    for(row = 0; row<120; row++){
        for(col = 0,col2 = 1;col < 20; col++,col2++){
            if(col2>=20)
                col2 = 0;
            Image_Ptr[row * 20 + col] = Image[row][col2];
        }
    }
#elif 0
/***********************************/
/*     上位机显示原始采集的图像     */
/***********************************/
    uint8 row,col,col2,data;
    for(row = 0; row<120; row++){
        for(col = 0,col2 = 1;col < 20; col++,col2++){
            Image_Ptr[row * 20 + col] = Image[row][col2];
        }
    }
#endif
#if 0
{
    TUart0_Puts("PIT1!\r\n");
    sprintf(TXBuffer,"%u,%u,%u,%u--",chang,hang,hang2,chang2);
    TUart0_Puts(TXBuffer);
    sprintf(TXBuffer,"%u,%u,%u,%u,%u,%u---",MCF_DTIM3_DTCR,MCF_DMA3_SAR,MCF_DMA3_DAR,Image,&Image[1],MCF_DMA3_BCR&0xffffff);
    TUart0_Puts(TXBuffer);
    sprintf(TXBuffer,"%u\r\n",&(Image[Cnt_HREF]));
    TUart0_Puts(TXBuffer);
    hang = 0;
    hang2 = 0;
    chang = 0;
    chang2 = 0;
    leftMSCnt = 0;
    rightMSCnt = 0;
}
#endif

    //TUart0_Puts("PIT1!\r\n");
    MCF_PIT1_PCSR &= ~MCF_PIT_PCSR_EN;
    MCF_PIT1_PCSR |= MCF_PIT_PCSR_PIF;  //清中断标志
}
#endif
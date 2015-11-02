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
            MCF_PIT0_PCSR = MCF_PIT_PCSR_RLD | MCF_PIT_PCSR_PIF | MCF_PIT_PCSR_PIE | MCF_PIT_PCSR_OVW | MCF_PIT_PCSR_PRE(TPIT0_PRECLK);
            MCF_PIT0_PMR = TPIT0_ZQ * 1000;   //设置计数值
        	MCF_INTC0_IMRH &=~ MCF_INTC_IMRH_INT_MASK55;                //设置中断源号为55，实际位置为55+64
        	MCF_INTC0_ICR55 = MCF_INTC_ICR_IP(4) |MCF_INTC_ICR_IL(2);   //设置中断优先级
            //MCF_PIT0_PCSR |= MCF_PIT_PCSR_EN;
        }break;
        case 1:{
            MCF_PIT1_PCSR = 0;
            //                 自动重载         计数为0中断         使能中断            立即载入PMR值       80000000 / 2^10 = 78125 2^11 = 39062 2^12=19531
            MCF_PIT1_PCSR = MCF_PIT_PCSR_RLD | MCF_PIT_PCSR_PIF | MCF_PIT_PCSR_PIE | MCF_PIT_PCSR_OVW | MCF_PIT_PCSR_PRE(TPIT1_PRECLK);
            MCF_PIT1_PMR = TPIT1_ZQ * 1000;   //设置计数值
        	MCF_INTC0_IMRH &=~ MCF_INTC_IMRH_INT_MASK56;                //设置中断源号为55，实际位置为55+64
        	MCF_INTC0_ICR56 = MCF_INTC_ICR_IP(4) |MCF_INTC_ICR_IL(2);   //设置中断优先级
            //MCF_PIT1_PCSR |= MCF_PIT_PCSR_EN;
        }break;
    }
}
__declspec(interrupt:0) void TPIT0_interrupt(void){
#if 0
    vuint8 row;
    vuint16 left_Offset_Cnt = 0,right_Offset_Cnt = 0;
    vuint8 left_Offset_Max = 0,right_Offset_Max = 0;
    for(row = 119;;row--){
        if(img_EdgeInfo[row][2] != 0){
            if(img_EdgeInfo[row][2] < 80){
                left_Offset_Cnt += (80 - img_EdgeInfo[row][2]);
                if((80 - img_EdgeInfo[row][2]) > left_Offset_Max)
                    left_Offset_Max = (uint8)(80 - img_EdgeInfo[row][2]);
            }else{
                right_Offset_Cnt += (img_EdgeInfo[row][2] - 80);
                if((img_EdgeInfo[row][2] - 80) > right_Offset_Max)
                    right_Offset_Max = (uint8)(img_EdgeInfo[row][2] - 80);
            }
        }
        if(row == 0)
            break;
    }

    if(left_Offset_Cnt>right_Offset_Cnt){
        pwmCnt = 3750 - (left_Offset_Cnt+right_Offset_Cnt)/3*(left_Offset_Cnt/(float)(left_Offset_Cnt+right_Offset_Cnt));
        TPWM45_SetDTY(pwmCnt);
    }else{
        pwmCnt = 3750 + (left_Offset_Cnt+right_Offset_Cnt)/3*(right_Offset_Cnt/(float)(left_Offset_Cnt+right_Offset_Cnt));
        TPWM45_SetDTY(pwmCnt);
    }
    //sprintf(TXBuffer,"%u,%u,%u\r\n",pwmCnt,left_Offset_Cnt,right_Offset_Cnt);
    //TUart0_Puts(TXBuffer);
#elif 1
    vuint8 row;
    uint8 pitch;
    for(row = 118,pitch = img_EdgeInfo[row+1][2];row >=90;row--){
        pitch = (pitch + img_EdgeInfo[row][2]) /2;
    }
    if(pitch>CAMERA_W/2&&pwmCnt<4250){
        pwmCnt+=15;
    }else if(pitch<CAMERA_W/2&&pwmCnt>3150){
        pwmCnt-=15;
    }else{
        pwmCnt = 3700;
    }
    TPWM45_SetDTY(pwmCnt);
#endif
    //TUart0_Puts("PIT0!\r\n");
    MCF_PIT0_PCSR &= ~MCF_PIT_PCSR_EN;
    MCF_PIT0_PCSR |= MCF_PIT_PCSR_PIF;  //清中断标志

}
uint16 TZ_sqrt(uint16 x){
    uint16 ans = 0,p = 0x80;
    while(p!=0)
    {
        ans+=p;
        if(ans*ans>x)
        {
            ans-=p;
        }
        p=(uint16)(p/2);
    }
    return(ans);
}
uint16 TZ_length(uint8 x1,uint8 y1,uint8 x2,uint8 y2){
}
__declspec(interrupt:0) void TPIT1_interrupt(void){

#define EDGE_OFFSET 2
/*图像边线提取后下一行起始位置偏移值*/
#define EDGE_INVALID 255
/*边线无效值定义，必须不为图像列的取值*/
#define IMG_ANALYSE_MODE    5
/*
5:uint8型线提取边线
11:上位机显示偏移校正后的图像
12:上位机显示原始采集的图像
13:上位机显示偏移校正后隔行采集的图像
0:调试模式，串口输出变量值，不进行图像处理
*/

#if(IMG_ANALYSE_MODE==5)
/***********************************/
/*     uint8型线扫描法提取边线   */
/***********************************/

    vuint8 *ptr;

    for(u = 0; u<CAMERA_H;u++){
        for(v = 0;v<CAMERA_W_8;v++){
            Image_Ptr[u * 20 + v] = 0;
        }
    }

    //速度最快
    for(u = 0; u<IMG_H; u++){
        ptr = &ImageC[u][1];
        for(v = 1;v < IMG_W_8; v++){
            data = Image[Image_Row[u]][v];
            Image_Ptr[Image_Row[u] * 20 + v - 1] = data;
            *ptr = data&0x80;
            data=data<<1;
            ptr++;
            *ptr = data&0x80;
            data=data<<1;
            ptr++;
            *ptr = data&0x80;
            data=data<<1;
            ptr++;
            *ptr = data&0x80;
            data=data<<1;
            ptr++;
            *ptr = data&0x80;
            data=data<<1;
            ptr++;
            *ptr = data&0x80;
            data=data<<1;
            ptr++;
            *ptr = data&0x80;
            data=data<<1;
            ptr++;
            *ptr = data&0x80;
            ptr++;
        }
        data = Image[Image_Row[u]][0];
        Image_Ptr[Image_Row[u] * 20 + v - 1] = data;
        *ptr = data&0x80;
        data=data<<1;
        ptr++;
        *ptr = data&0x80;
        data=data<<1;
        ptr++;
        *ptr = data&0x80;
        data=data<<1;
        ptr++;
        *ptr = data&0x80;
        data=data<<1;
        ptr++;
        *ptr = data&0x80;
        data=data<<1;
        ptr++;
        *ptr = data&0x80;
        data=data<<1;
        ptr++;
        *ptr = data&0x80;
        data=data<<1;
        ptr++;
        *ptr = data&0x80;
    }

#if 1
    leftEdgeStart = IMG_W/2;  	//左边线检测的起始位置
    leftEdgeEnd = 0;      		//左边线检测的结束位置

    rightEdgeStart = IMG_W/2+1; //右边线检测的起始位置
    rightEdgeEnd = IMG_W - 1;	//右边线检测的结束位置

    //img_EdgeInfo[CAMERA_H][5] = {0};//图像边界信息
    //[0]：左边界 [1]：右边界 [2]：中线 [3]：获取到的边界信息 [4]：斜率\曲率

    leftEdgeFind = 0;rightEdgeFind = 0;			//左右边界是否找到标志量
    leftEdgeMissCnt = 0;rightEdgeMissCnt = 0;	//左右边界miss个数计数
    leftEdgeBool = 1;rightEdgeBool = 1;			//左右边界

    leftJPMissLine = 0;rightJPMissLine = 0;
    //leftLastLine[2] = {0};rightLastLine[2] = {0};

    centerLine = CAMERA_W/2+1;

    colEdgeEnable = 0;

    leftEdgeOffset = 20;
    rightEdgeOffset = 20;
    edgeOffset = 0;

    leftEdgeFind = 0;
    rightEdgeFind = 0;

    for(row = IMG_H-1;row>=0;row --){

        img_EdgeInfo[row][0] = 0;
        img_EdgeInfo[row][1] = 0;
        img_EdgeInfo[row][2] = 0;
        img_EdgeInfo[row][3] = 0;
        img_EdgeInfo[row][4] = 0;
        world_EdgeInfo[row][0] = 0;
        world_EdgeInfo[row][1] = 0;
        world_EdgeInfo[row][2] = 0;
        world_EdgeInfo[row][3] = 0;
        world_EdgeInfo[row][4] = 0;        //Image_Ptr[Image_Row[row] * 20 + leftEdgeStart/8] |= 1<<(7-leftEdgeStart%8);
        //Image_Ptr[Image_Row[row] * 20 + rightEdgeStart/8] |= 1<<(7-rightEdgeStart%8);
        if(ImageC[row][centerLine] == 0x80){
            img_EdgeInfo[row][3] = 5;
            break;
        }
        for(col = leftEdgeStart;col>leftEdgeEnd;col--){
            if((ImageC[row][col]&0x80) && ImageC[row][col+1] == 0 ){
                img_EdgeInfo[row][0] = col;         //获取图像边界信息
                world_EdgeInfo[row][0] = correctTable[row][col];//获取世界边界信息

                //更新最后的边线的行列
                leftLastLine[0] = row;
                leftLastLine[1] = col;

                leftEdgeFind = 1;//左边线标志量置位
                break;
            }else if(ImageC[row][col+1]&0x80){
                leftEdgeStart = centerLine;
            }
        }
        for(col = rightEdgeStart;col<rightEdgeEnd;col++){
            if((ImageC[row][col]&0x80) && ImageC[row][col-1] == 0 ){
                img_EdgeInfo[row][1] = col;         //获取图像边界信息
                world_EdgeInfo[row][1] = correctTable[row][col];//获取世界边界信息

                //更新最后的边线的行列
                rightLastLine[0] = row;
                rightLastLine[1] = col;

                rightEdgeFind = 1;//右边线标志量置位
                break;
            }else if(ImageC[row][col-1]&0x80){
                rightEdgeStart = centerLine;
            }
        }
#if 0
        if(row <= IMG_H-2){
            if(leftEdgeFind&&img_EdgeInfo[row+1][0]!=0){
                edgeOffset = img_EdgeInfo[row][0]-img_EdgeInfo[row+1][0];
                if(edgeOffset<0)
                    edgeOffset = -edgeOffset;
                if(edgeOffset>leftEdgeOffset+5)
                    img_EdgeInfo[row][0] = img_EdgeInfo[row+1][0];
                else
                    leftEdgeOffset = edgeOffset;
            }
            if(rightEdgeFind&&img_EdgeInfo[row+1][0]!=0){
                edgeOffset = img_EdgeInfo[row][1]-img_EdgeInfo[row+1][1];
                if(edgeOffset<0)
                    edgeOffset = -edgeOffset;
                if(edgeOffset>rightEdgeOffset+5)
                    img_EdgeInfo[row][1] = img_EdgeInfo[row+1][1];
                else
                    rightEdgeOffset = edgeOffset;
            }
        }
#endif
        if(leftEdgeFind&&rightEdgeFind){

        	img_EdgeInfo[row][2] = (img_EdgeInfo[row][0] + img_EdgeInfo[row][1])*0.5;
        	world_EdgeInfo[row][2] = (world_EdgeInfo[row][0] + world_EdgeInfo[row][1])*0.5;

        	centerLine = img_EdgeInfo[row][2];
        	//Image_Ptr[Image_Row[row] * 20 + img_EdgeInfo[row][2]/8] |= 1<<(7-img_EdgeInfo[row][2]%8);
        	//Image_Ptr[Image_Row[row] * 20 + world_EdgeInfo[row][2]/8] |= 1<<(7-world_EdgeInfo[row][2]%8);

            if(0&&row < IMG_H-1){
                if(img_EdgeInfo[row][2]>img_EdgeInfo[row-1][2])
                    img_EdgeInfo[row][4] = 1;
                else if(img_EdgeInfo[row][2]<img_EdgeInfo[row-1][2])
                    img_EdgeInfo[row][4] = 2;
            }

            leftEdgeStart =   img_EdgeInfo[row][0] + 15;
            leftEdgeEnd   =   img_EdgeInfo[row][0] - 5;
            rightEdgeStart =  img_EdgeInfo[row][1] - 15;
            rightEdgeEnd  =   img_EdgeInfo[row][1] + 5;

            img_EdgeInfo[row][3] = 1;

        }else if(leftEdgeFind){
            temp = rightLastLine[1];

        	//img_EdgeInfo[row][2] = (img_EdgeInfo[row][0] + temp)/2;
        	//world_EdgeInfo[row][2] = (world_EdgeInfo[row][0] + world_EdgeInfo[temp][1])/2;

            leftEdgeStart = img_EdgeInfo[row][0] + 15;
            leftEdgeEnd = img_EdgeInfo[row][0] - 5;
            img_EdgeInfo[row][3] = 2;

        }else if(rightEdgeFind){
            temp = leftLastLine[1];

        	//img_EdgeInfo[row][2] = (img_EdgeInfo[row][1] + leftLastLine[1])/2;
        	//world_EdgeInfo[row][2] = (world_EdgeInfo[temp][0] + world_EdgeInfo[row][1])/2;

            rightEdgeStart = img_EdgeInfo[row][1] - 15;
            rightEdgeEnd = img_EdgeInfo[row][1] + 5;
            img_EdgeInfo[row][3] = 3;

        }else{
            leftEdgeStart = centerLine;
            rightEdgeStart = centerLine+1;
            img_EdgeInfo[row][3] = 4;
        }

        if(leftEdgeStart>=CAMERA_W){
            leftEdgeStart = CAMERA_W - 1;
        }
        if(rightEdgeStart < 0){
            rightEdgeStart = 0;
        }
        if(leftEdgeEnd < 0){
            leftEdgeEnd = 0;
            leftEdgeStart = leftEdgeEnd + 20;
        }
        if(rightEdgeEnd >= CAMERA_W){
            rightEdgeEnd = CAMERA_W - 1;
            rightEdgeStart = rightEdgeEnd - 20;
        }

        leftEdgeFind = 0;
        rightEdgeFind = 0;

    }
    for(row = IMG_H-2;row>0;row--){
        if(img_EdgeInfo[row-1][2]&&img_EdgeInfo[row+1][2]){
            img_EdgeInfo[row][2] = (img_EdgeInfo[row-1][2] + img_EdgeInfo[row+1][2])/2;
            world_EdgeInfo[row][2] = correctTable[row][img_EdgeInfo[row][2]];//获取世界边界信息

        }else if(img_EdgeInfo[row][3] == 4){
        }else if(img_EdgeInfo[row][3] == 5&&img_EdgeInfo[row+2][3] != 1&&row>5){
            if(leftLastLine[0] > rightLastLine[0]){
                v = leftLastLine[1];
                u = (leftLastLine[0]+row)/2;
                topEdgeStart = u;
                bottomEdgeStart = u;
                topEdgeEnd = row - 5;
                bottomEdgeEnd = leftLastLine[0] + 5;
                direct = -1;
                colEnd = 0;
                colEdgeEnable = 1;

            }else if(leftLastLine[0] < rightLastLine[0]){
                v = rightLastLine[1];
                u = (rightLastLine[0]+row)/2;
                topEdgeStart = u;
                bottomEdgeStart = u;
                topEdgeEnd = row - 5;
                bottomEdgeEnd = rightLastLine[0] + 5;
                direct = 1;
                colEnd = IMG_W-1;
                colEdgeEnable = 1;
            }
        }
        if(img_EdgeInfo[row][2])
            Image_Ptr[Image_Row[row] * 20 + img_EdgeInfo[row][2]/8] |= 1<<(7-img_EdgeInfo[row][2]%8);
    }
            Image_Ptr[Image_Row[row] * 20 + img_EdgeInfo[row][2]/8] |= 1<<(7-img_EdgeInfo[row][2]%8);
    if(colEdgeEnable){

        colEdgeCnt = 0;
        colCenterLine = u;
        colBool = 0;

        for(col = v;col!=colEnd;col+=direct){

            img_ColEdgeInfo[colEdgeCnt][0] = 0;
            img_ColEdgeInfo[colEdgeCnt][1] = 0;
            img_ColEdgeInfo[colEdgeCnt][2] = 0;
            img_ColEdgeInfo[colEdgeCnt][3] = 0;
            img_ColEdgeInfo[colEdgeCnt][4] = 0;
            img_ColEdgeInfo[colEdgeCnt][5] = col;
            world_ColEdgeInfo[colEdgeCnt][0] = 0;
            world_ColEdgeInfo[colEdgeCnt][1] = 0;
            world_ColEdgeInfo[colEdgeCnt][2] = 0;
            world_ColEdgeInfo[colEdgeCnt][3] = 0;
            world_ColEdgeInfo[colEdgeCnt][4] = 0;
            world_ColEdgeInfo[colEdgeCnt][5] = col;

            if(ImageC[colCenterLine][col] == 0x80){
                img_ColEdgeInfo[colEdgeCnt][3] = 5;
                break;
            }

        	//Image_Ptr[Image_Row[topEdgeStart] * 20 + col/8] |= 1<<(7-col%8);
        	//Image_Ptr[Image_Row[bottomEdgeStart] * 20 + col/8] |= 1<<(7-col%8);
            for(row = topEdgeStart;row>topEdgeEnd;row--){
                if((ImageC[row][col]&0x80) && ImageC[row+1][col] == 0 ){
                    img_ColEdgeInfo[colEdgeCnt][0] = row;         //获取图像边界信息
                    world_ColEdgeInfo[colEdgeCnt][0] = correctTable[row][col];//获取世界边界信息

                    topEdgeFind = 1;
                    break;

                }
            }
            for(row = bottomEdgeStart;row<bottomEdgeEnd;row++){
                if((ImageC[row][col]&0x80) && ImageC[row-1][col] == 0 ){
                    img_ColEdgeInfo[colEdgeCnt][1] = row;         //获取图像边界信息
                    world_ColEdgeInfo[colEdgeCnt][1] = correctTable[row][col];//获取世界边界信息

                    bottomEdgeFind = 1;
                    break;

                }
            }
            if(topEdgeFind&&bottomEdgeFind){

                img_ColEdgeInfo[colEdgeCnt][2] = (uint8)((img_ColEdgeInfo[colEdgeCnt][0]+img_ColEdgeInfo[colEdgeCnt][1])/2);
                world_ColEdgeInfo[colEdgeCnt][2] = correctTable[img_ColEdgeInfo[colEdgeCnt][2]][col];//获取世界边界信息

        	    Image_Ptr[Image_Row[img_ColEdgeInfo[colEdgeCnt][2]] * 20 + img_ColEdgeInfo[colEdgeCnt][5]/8] |= 1<<(7-img_ColEdgeInfo[colEdgeCnt][5]%8);
                colCenterLine = img_ColEdgeInfo[colEdgeCnt][2];

                if(colBool == 0){
                    firstRow = img_ColEdgeInfo[colEdgeCnt][2];
                    firstCol = col;
                    colBool = 1;
                }

                topEdgeStart =   img_ColEdgeInfo[colEdgeCnt][0] + 5;
                topEdgeEnd   =   img_ColEdgeInfo[colEdgeCnt][0] - 5;
                bottomEdgeStart =  img_ColEdgeInfo[colEdgeCnt][1] - 5;
                bottomEdgeEnd  =   img_ColEdgeInfo[colEdgeCnt][1] + 5;

                img_ColEdgeInfo[colEdgeCnt][3] = 1;

                colEdgeCnt++;
            }else if(topEdgeFind){
                topEdgeStart =   img_ColEdgeInfo[colEdgeCnt][0] + 5;
                topEdgeEnd   =   img_ColEdgeInfo[colEdgeCnt][0] - 5;
                bottomEdgeStart = topEdgeStart;

                img_ColEdgeInfo[colEdgeCnt][3] = 2;

                colEdgeCnt++;
            }else if(bottomEdgeFind){
                bottomEdgeStart =  img_ColEdgeInfo[colEdgeCnt][1] - 5;
                bottomEdgeEnd  =   img_ColEdgeInfo[colEdgeCnt][1] + 5;
                topEdgeStart = bottomEdgeStart;

                img_ColEdgeInfo[colEdgeCnt][3] = 3;

                colEdgeCnt++;
            }else{
                img_ColEdgeInfo[colEdgeCnt][3] = 4;
                colEdgeCnt++;
            }

            if(topEdgeStart>=40){
                topEdgeStart = 40;
            }
            if(bottomEdgeStart < 0){
                bottomEdgeStart = 0;
            }
            if(topEdgeEnd < 0){
                topEdgeEnd = 0;
                topEdgeStart = topEdgeEnd + 10;
            }
            if(bottomEdgeEnd >= 40){
                bottomEdgeEnd = 40;
                bottomEdgeStart = bottomEdgeEnd - 10;
            }

            topEdgeFind = 0;
            bottomEdgeFind = 0;

        }
        if(colBool){
            if(direct == 1){
                temp = rightLastLine[0] - firstRow;
                data = firstCol - img_EdgeInfo[rightLastLine[0]][2];
                //sprintf(TXBuffer,"[%d+%d]",temp,data);
                //TUart0_Puts(TXBuffer);
                for(row = rightLastLine[0];row >= rightLastLine[0]-temp;row--){
                	img_EdgeInfo[row][2] = (uint16)(img_EdgeInfo[rightLastLine[0]][2] + ((float)(rightLastLine[0] - row))/(temp+1) * data);
                    world_EdgeInfo[row][2] = correctTable[row][img_EdgeInfo[row][2]];//获取世界边界信息

                    Image_Ptr[Image_Row[row] * 20 + img_EdgeInfo[row][2]/8] |= 1<<(7-img_EdgeInfo[row][2]%8);
                    //sprintf(TXBuffer,"[%d]\n",img_EdgeInfo[row][2]);
                    //TUart0_Puts(TXBuffer);
                }
            }else{
                temp = leftLastLine[0] - firstRow;
                data = img_EdgeInfo[rightLastLine[0]][2] - firstCol;
                for(row = leftLastLine[0];row>topEdgeEnd;row--){
                	img_EdgeInfo[row][2] = (uint16)(img_EdgeInfo[leftLastLine[0]][2] + ((float)(leftLastLine[0] - row))/(temp+1) * data);
                    world_EdgeInfo[row][2] = correctTable[row][img_EdgeInfo[row][2]];//获取世界边界信息

                    Image_Ptr[Image_Row[row] * 20 + img_EdgeInfo[row][2]/8] |= 1<<(7-img_EdgeInfo[row][2]%8);
                }
            }
        }
    }
#if 1
//以下出现的控制变量大多出现在Control文件里
    controlMode = 0;
    rowBreak = 0;
    controlError = 0;
    controlFinsh = 0;

    lineOffsetRow = 50;
    lineOffsetCnt = 0;
    lineOffsetValueCnt = 0;

    tpStartDist = 0;
    tpEndDist = 0;

    tp2RowCnt = 0;
    tp2ColCnt = 0;

    for(row = IMG_H-1;controlError == 0&&controlFinsh == 0&&row>0;row --){
        if(img_EdgeInfo[row][3] == 5 )
            rowBreak = 1;
        switch(controlMode){
            case 0:{
                if(rowBreak == 0 && img_EdgeInfo[row][2] != 0){
                    temp = img_EdgeInfo[row][2] - IMG_W_HALF + centLineOffset;
                    if(temp < (centLineOffset<<1)){
                        lineOffsetRow = row;
                        lineOffsetCnt ++;
                        lineOffsetValueCnt += (temp - centLineOffset);
                    }else{
                        controlMode++;    //改变控制方式

                        threePoint[1][0] = 0;
                        threePoint[2][0] = 0;

                        threePoint[0][0] = row;//赋值三点数组
                        threePoint[0][1] = img_EdgeInfo[row][2];
                        tpStartDist = world_ActualRange[row];


                    }
                }else if(rowBreak){
                    controlError = 1;
                    break;
                }
            }break;
            case 1:{
                if(lineOffsetRow < 20)
                    break;
                if(img_EdgeInfo[row][2] != 0){
                    threePoint[2][0] = row;
                    threePoint[2][1] = img_EdgeInfo[row][2];
                    if( (world_ActualRange[row] - tpStartDist) > 80){
                        midThreePointCol1 = (threePoint[2][0] + threePoint[0][0])/2;
                        midThreePointCol2 = midThreePointCol1+1;
                        do{
                            if(img_EdgeInfo[midThreePointCol1][2] != 0){
                                threePoint[1][0] = midThreePointCol1;
                                threePoint[1][1] = img_EdgeInfo[midThreePointCol1][2];
                                break;
                            }else{
                                midThreePointCol1--;
                            }
                            if(img_EdgeInfo[midThreePointCol2][2] != 0){
                                threePoint[1][0] = midThreePointCol2;
                                threePoint[1][1] = img_EdgeInfo[midThreePointCol2][2];
                                break;
                            }else{
                                midThreePointCol2++;
                            }
                        }while(midThreePointCol1 != threePoint[2][0] && midThreePointCol2 != threePoint[0][0]);
                        if(midThreePointCol1 == threePoint[2][0] || midThreePointCol2 == threePoint[0][0]){
                            controlError = 2;
                            break;
                        }else if(world_ActualRange[row]<150){
                            threePoint2[1][0] = 0;
                            threePoint2[2][0] = 0;

                            threePoint2[0][0] = row;//赋值三点数组
                            threePoint2[0][1] = img_EdgeInfo[row][2];
                            controlMode ++;
                        }else{
                            controlFinsh = 1;
                        }
                    }
                }else if(rowBreak){
                    if(threePoint[2][0] != 0){
                        midThreePointCol1 = (threePoint[2][0] + threePoint[0][0])/2;
                        midThreePointCol2 = midThreePointCol1+1;
                        do{
                            if(img_EdgeInfo[midThreePointCol1][2] != 0){
                                threePoint[1][0] = midThreePointCol1;
                                threePoint[1][1] = img_EdgeInfo[midThreePointCol1][2];
                                break;
                            }else{
                                midThreePointCol1--;
                            }
                            if(img_EdgeInfo[midThreePointCol2][2] != 0){
                                threePoint[1][0] = midThreePointCol2;
                                threePoint[1][1] = img_EdgeInfo[midThreePointCol2][2];
                                break;
                            }else{
                                midThreePointCol2++;
                            }
                        }while(midThreePointCol1 != threePoint[2][0] && midThreePointCol2 != threePoint[0][0]);
                        if(midThreePointCol1 == threePoint[2][0] || midThreePointCol2 == threePoint[0][0]){
                            controlError = 3;
                            break;
                        }
                    }
                    break;
                }
            }break;
            case 2:{
                tp2RowCnt++;
                if(img_EdgeInfo[row][2] != 0){
                    threePoint2[2][0] = row;
                    threePoint2[2][1] = img_EdgeInfo[row][2];
                }else if(rowBreak){
                    temp = 0;
                    if(img_ColEdgeInfo[temp][2] != 0){
                        while(img_ColEdgeInfo[temp][2] !=0 && img_ColEdgeInfo[temp][2] !=5){
                            tp2ColCnt++;
                            temp++;
                        }
                        if(tp2ColCnt>tp2RowCnt){
                            threePoint2[2][0] = img_ColEdgeInfo[temp-1][2];
                            threePoint2[2][1] = img_ColEdgeInfo[temp-1][5];
                            threePoint2[1][0] = img_ColEdgeInfo[0][2];
                            threePoint2[1][1] = img_ColEdgeInfo[0][5];
                            controlFinsh = 1;

                        }else if(tp2ColCnt<=tp2RowCnt){
                            threePoint2[1][0] = threePoint2[2][0];
                            threePoint2[1][1] = threePoint2[2][1];
                            threePoint2[2][0] = img_ColEdgeInfo[temp-1][2];
                            threePoint2[2][1] = img_ColEdgeInfo[temp-1][5];
                            controlFinsh = 1;
                        }
                    }else{
                        midThreePoint2Col1 = (threePoint2[2][0] + threePoint2[0][0])/2;
                        midThreePoint2Col2 = midThreePoint2Col1+1;
                        do{
                            if(img_EdgeInfo[midThreePoint2Col1][2] != 0){
                                threePoint2[1][0] = midThreePoint2Col1;
                                threePoint2[1][1] = img_EdgeInfo[midThreePoint2Col1][2];
                                break;
                            }else{
                                midThreePoint2Col1--;
                            }
                            if(img_EdgeInfo[midThreePoint2Col2][2] != 0){
                                threePoint2[1][0] = midThreePoint2Col2;
                                threePoint2[1][1] = img_EdgeInfo[midThreePoint2Col2][2];
                                break;
                            }else{
                                midThreePoint2Col2++;
                            }
                        }while(midThreePoint2Col1 != threePoint2[2][0] && midThreePoint2Col2 != threePoint2[0][0]);
                        if(midThreePoint2Col1 == threePoint2[2][0] || midThreePoint2Col2 == threePoint2[0][0]){
                            controlError = 4;
                            break;
                        }else{
                            controlFinsh = 1;
                        }

                    }
                }
            }break;
        }
    }

#endif
#endif
#elif (IMG_ANALYSE_MODE==11)
/***********************************/
/*     上位机显示偏移校正后的图像   */
/***********************************/

    for(row = 0; row<120; row++){   //图像偏移还原
        data = Image[row][0];
        for(col = 0;col < 19; col++){
            Image[row][col] = Image[row][col + 1];
        }
        Image[row][col] = data;
    }
#elif (IMG_ANALYSE_MODE==12)
/***********************************/
/*     上位机显示原始采集的图像     */
/***********************************/
    uint8 col2;
    for(row = 0; row<120; row++){
        for(col = 0,col2 = 1;col < 20; col++,col2++){
            Image_Ptr[row * 20 + col] = Image[row][col2];
        }
    }
#elif (IMG_ANALYSE_MODE==13)
/*********************************************/
/*     上位机显示偏移校正后隔行采集的图像     */
/********************************************/
    for(row = 0; row<50; row++){   //图像偏移还原
        data = Image[Image_Row[row]][0];
        for(col = 0;col < 19; col++){
            Image_Ptr[Image_Row[row] * 20 + col] = Image[Image_Row[row]][col + 1];
        }
        Image_Ptr[Image_Row[row] * 20 + col] = data;
    }
#elif (IMG_ANALYSE_MODE==14)

vuint8 *ptr;

for(u = 0; u<IMG_H;u++){
    for(v = 0;v<IMG_W_8;v++){
        Image_Ptr[Image_Row[u] * 20 + v] = 0;
    }
}

#if 0
    //bit还原为char型,速度最慢
    for(u = 0; u<IMG_H; u++){
        for(v = 1;v < IMG_W_8; v++){
            data = Image[Image_Row[u]][v];
            Image_Ptr[Image_Row[u] * 20 + v - 1] = data;
            ImageC[u][v*8-1] = data&0x01;
            data>>1;
            ImageC[u][v*8-2] = data&0x01;
            data>>1;
            ImageC[u][v*8-3] = data&0x01;
            data>>1;
            ImageC[u][v*8-4] = data&0x01;
            data>>1;
            ImageC[u][v*8-5] = data&0x01;
            data>>1;
            ImageC[u][v*8-6] = data&0x01;
            data>>1;
            ImageC[u][v*8-7] = data&0x01;
            data>>1;
            ImageC[u][v*8-8] = data&0x01;
        }
        data = Image[Image_Row[u]][0];
        Image_Ptr[Image_Row[u] * 20 + v - 1] = data;
        ImageC[u][119] = data&0x01;
        data>>1;
        ImageC[u][118] = data&0x01;
        data>>1;
        ImageC[u][117] = data&0x01;
        data>>1;
        ImageC[u][116] = data&0x01;
        data>>1;
        ImageC[u][115] = data&0x01;
        data>>1;
        ImageC[u][114] = data&0x01;
        data>>1;
        ImageC[u][113] = data&0x01;
        data>>1;
        ImageC[u][112] = data&0x01;
    }

#elif 0
    //略微优化速度
    for(u = 0; u<IMG_H; u++){
        for(v = 1;v < IMG_W_8; v++){
            a = (abc*)Image[Image_Row[u]][v];
            Image_Ptr[Image_Row[u] * 20 + v - 1] = data;
            ImageC[u][v*8-1] = a -> d0;
            ImageC[u][v*8-2] = a -> d1;
            ImageC[u][v*8-3] = a -> d2;
            ImageC[u][v*8-4] = a -> d3;
            ImageC[u][v*8-5] = a -> d4;
            ImageC[u][v*8-6] = a -> d5;
            ImageC[u][v*8-7] = a -> d6;
            ImageC[u][v*8-8] = a -> d7;
        }
    }
#elif 1

    for(u = 0; u<IMG_H; u++){
        ptr = &ImageC[u][1];
        for(v = 1;v < IMG_W_8; v++){
            data = Image[Image_Row[u]][v];
            Image_Ptr[Image_Row[u] * 20 + v - 1] = data;
            *ptr = data&0x80;
            data=data<<1;
            ptr++;
            *ptr = data&0x80;
            data=data<<1;
            ptr++;
            *ptr = data&0x80;
            data=data<<1;
            ptr++;
            *ptr = data&0x80;
            data=data<<1;
            ptr++;
            *ptr = data&0x80;
            data=data<<1;
            ptr++;
            *ptr = data&0x80;
            data=data<<1;
            ptr++;
            *ptr = data&0x80;
            data=data<<1;
            ptr++;
            *ptr = data&0x80;
            ptr++;
        }
        data = Image[Image_Row[u]][0];
        Image_Ptr[Image_Row[u] * 20 + v - 1] = data;
        *ptr = data&0x80;
        data=data<<1;
        ptr++;
        *ptr = data&0x80;
        data=data<<1;
        ptr++;
        *ptr = data&0x80;
        data=data<<1;
        ptr++;
        *ptr = data&0x80;
        data=data<<1;
        ptr++;
        *ptr = data&0x80;
        data=data<<1;
        ptr++;
        *ptr = data&0x80;
        data=data<<1;
        ptr++;
        *ptr = data&0x80;
        data=data<<1;
        ptr++;
        *ptr = data&0x80;
    }
#elif 0
    for(u = 0; u<IMG_H; u++){
        for(v = 1;v < IMG_W_8; v++){
            data = Image[Image_Row[u]][v];
            if(data&0x80)
                Image_Ptr[u * 40 + correctTable[u][(v-1)*8+7]/8] |= 1<<7;
            data=data<<1;
            if(data&0x80)
                Image_Ptr[u * 40 + correctTable[u][(v-1)*8+6]/8] |= 1<<6;
            data=data<<1;
            if(data&0x80)
                Image_Ptr[u * 40 + correctTable[u][(v-1)*8+5]/8] |= 1<<5;
            data=data<<1;
            if(data&0x80)
                Image_Ptr[u * 40 + correctTable[u][(v-1)*8+4]/8] |= 1<<4;
            data=data<<1;
            if(data&0x80)
                Image_Ptr[u * 40 + correctTable[u][(v-1)*8+3]/8] |= 1<<3;
            data=data<<1;
            if(data&0x80)
                Image_Ptr[u * 40 + correctTable[u][(v-1)*8+2]/8] |= 1<<2;
            data=data<<1;
            if(data&0x80)
                Image_Ptr[u * 40 + correctTable[u][(v-1)*8+1]/8] |= 1<<1;
            data=data<<1;
            if(data&0x80)
                Image_Ptr[u * 40 + correctTable[u][(v-1)*8+0]/8] |= 1;
            data=data<<1;
        }
        data = Image[Image_Row[u]][0];
            if(data&0x80)
                Image_Ptr[u * 40 + correctTable[u][(v-1)*8+7]/8] |= 1<<7;
            data=data<<1;
            if(data&0x80)
                Image_Ptr[u * 40 + correctTable[u][(v-1)*8+6]/8] |= 1<<6;
            data=data<<1;
            if(data&0x80)
                Image_Ptr[u * 40 + correctTable[u][(v-1)*8+5]/8] |= 1<<5;
            data=data<<1;
            if(data&0x80)
                Image_Ptr[u * 40 + correctTable[u][(v-1)*8+4]/8] |= 1<<4;
            data=data<<1;
            if(data&0x80)
                Image_Ptr[u * 40 + correctTable[u][(v-1)*8+3]/8] |= 1<<3;
            data=data<<1;
            if(data&0x80)
                Image_Ptr[u * 40 + correctTable[u][(v-1)*8+2]/8] |= 1<<2;
            data=data<<1;
            if(data&0x80)
                Image_Ptr[u * 40 + correctTable[u][(v-1)*8+1]/8] |= 1<<1;
            data=data<<1;
            if(data&0x80)
                Image_Ptr[u * 40 + correctTable[u][(v-1)*8+0]/8] |= 1;
            data=data<<1;
    }

#endif
#elif (IMG_ANALYSE_MODE==0)

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
#endif

    //TUart0_Puts("PIT1!\r\n");
    MCF_PIT1_PCSR &= ~MCF_PIT_PCSR_EN;
    MCF_PIT1_PCSR |= MCF_PIT_PCSR_PIF;  //清中断标志
    bool = 0;
}
#endif
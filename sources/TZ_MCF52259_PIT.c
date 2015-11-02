    #include "DIY_ComDef.h"
#ifdef TZ_MCF52259_PIT
/*
*****2015.4.21 PIT模块编写完毕
*****功能：PIT可编程定时器模块进行定时
*****编写中遇到的主要问题：（
    1.预分频器和MCF_PIT0_PMR技术值需要注意计算
）
*/

/*
由于此部分代码经常运行，牵扯到代码优化，但代码优化后部分地方代码意思讲变得不通俗易懂
所以需要在代码基本成型后在进行代码优化
优化策略：
1：a=a+1 a=a-1  一步加减运算改变为 自增++ 自减--
2：a = a% 8  求余运算改变为与运算    a = a & 7
3：b = a^2  平方运算改为乘法运算    b = a * a
4：b=a* 2 b=a/ 2 乘除运算改为左右移位运算 b = a << 1 b = a >> 1
5：各种循环中 自减量 比 自增量 代码少
6：do while 快于 while
7：复杂运算都是用查表法
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
#if 1
    switch(controlMode){
        case 3:{
            pwmCnt = lineResSteerAngle*lineRealRate + 300/radiusOfCurva_1 + 200/radiusOfCurva_1 ;
            allMotorCnt = 5000;
        }break;
        case 2:{
            pwmCnt = lineResSteerAngle*5*lineRealRate + 300/radiusOfCurva_1;
            allMotorCnt = 3000;
        }break;
        case 1:{
            pwmCnt = lineResSteerAngle*11*lineRealRate;
            allMotorCnt = 1000;
        }break;
    }

    if(rightMotorSpeed <allMotorCnt)
        rightMotorCnt-=5;
    else
        rightMotorCnt+=5;
    if(leftMotorSpeed <allMotorCnt)
        leftMotorCnt-=5;
    else
        leftMotorCnt+=5;

    TPWM0_SetDTY(leftMotorCnt);
    TPWM1_SetDTY(rightMotorCnt);
    TPWM45_SetDTY(pwmCnt);
#elif 0
#endif
    MCF_PIT0_PCSR &= ~MCF_PIT_PCSR_EN;
    MCF_PIT0_PCSR |= MCF_PIT_PCSR_PIF;  //清中断标志

}
/*
unsigned int TZ_sqrt(uint16 x){
    uint16 ans = 0,p = 0x80;
    while(p!=0)
    {
        ans+=p;
        if(ans*ans>x)
        {
            ans-=p;
        }
        p=(uint16)(p*0.5);
    }
    return(ans);
}
*/
unsigned int sqrt_16(unsigned long M)
{
    unsigned int N, i;
    unsigned long tmp, ttp;   // 结果、循环计数
    if (M == 0)               // 被开方数，开方结果也为0
        return 0;
    N = 0;
    tmp = (M >> 30);          // 获取最高位：B[m-1]
    M <<= 2;
    if (tmp > 1)              // 最高位为1
    {
        N ++;                 // 结果当前位为1，否则为默认的0
        tmp -= N;
    }
    for (i=15; i>0; i--)      // 求剩余的15位
    {
        N <<= 1;              // 左移一位
        tmp <<= 2;
        tmp += (M >> 30);     // 假设
        ttp = N;
        ttp = (ttp<<1)+1;
        M <<= 2;
        if (tmp >= ttp)       // 假设成立
        {
            tmp -= ttp;
            N ++;
        }
    }
    return N;
}

void TPIT1_interrupt2(void){
#define EDGE_OFFSET 2
/*图像边线提取后下一行起始位置偏移值*/
#define EDGE_INVALID 255
/*边线无效值定义，必须不为图像列的取值*/
#define IMG_ANALYSE_MODE    13
/*
5:uint8型线提取边线
11:上位机显示偏移校正后的图像
12:上位机显示原始采集的图像
13:上位机显示偏移校正后隔行采集的图像
14:上位机显示偏移校正后隔行采集的图像(多种方式选择)
0:调试模式，串口输出变量值，不进行图像处理
*/


#if (IMG_ANALYSE_MODE==11)
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
#elif (IMG_ANALYSE_MODE==13)
/*********************************************/
/*     上位机显示偏移校正后隔行采集的图像     */
/********************************************/
    vuint8 *ptr;
    for(row = 0; 0&&row<50; row++){   //图像偏移还原
        data = Image[Image_Row[row]][0];
        for(col = 0;col < 19; col++){
            Image_Ptr[Image_Row[row] * 20 + col] = Image[Image_Row[row]][col + 1];
        }
        Image_Ptr[Image_Row[row] * 20 + col] = data;
    }
    for(u = 0; 0&&u<CAMERA_H;u++){
        for(v = 0;v<CAMERA_W_8;v++){
            Image_Ptr[u * 20 + v] = 0;
        }
    }

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
#endif
    MCF_PIT1_PCSR &= ~MCF_PIT_PCSR_EN;
    MCF_PIT1_PCSR |= MCF_PIT_PCSR_PIF;  //清中断标志
    bool = 0;

}
__declspec(interrupt:0)  void TPIT1_interrupt(void){

    vuint8 *ptr;

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

    //img_EdgeInfo[CAMERA_H][5] = {0};//图像边界信息
    //[0]：左边界 [1]：右边界 [2]：中线 [3]：获取到的边界信息 [4]：斜率\曲率

    leftEdgeBool = 1;rightEdgeBool = 1;			//左右边界

    colEdgeEnable = 0;

    edgeFillMode = 0;
    leftEdgeMissCnt = 0;rightEdgeMissCnt = 0;	//左右边界开始miss个数计数

    leftEdgeStart = IMG_W_HALF;  	//左边线检测的起始位置
    leftEdgeEnd = 0;      		    //左边线检测的结束位置
    rightEdgeStart = IMG_W_HALF+1; //右边线检测的起始位置
    rightEdgeEnd = IMG_W - 1;	    //右边线检测的结束位置

    centerLine = IMG_W_HALF+1;//最后的中线

    leftEdgeFind = 0;   //左边线找到标志量清零
    rightEdgeFind = 0;  //右边线找到标志量清零

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
        world_EdgeInfo[row][4] = 0;

        //Image_Ptr[Image_Row[row] * 20 + leftEdgeStart/8] |= 1<<(7-leftEdgeStart&0x07);
        //Image_Ptr[Image_Row[row] * 20 + rightEdgeStart/8] |= 1<<(7-rightEdgeStart&0x07);

        if(ImageC[row][centerLine] == 0x80){    //若中间线为黑线，则表示检测到结尾
            img_EdgeInfo[row][3] = 5;           //当前行类型置为5并结束循环
            break;
        }
        for(col = leftEdgeStart;col>leftEdgeEnd;col--){     //左边线寻找
            if((ImageC[row][col]&0x80) && ImageC[row][col+1] == 0 ){
                img_EdgeInfo[row][0] = col;                         //获取图像边界信息
                world_EdgeInfo[row][0] = correctTable[row][col];    //获取世界边界信息

                //更新最后的左边线的行列
                leftLastLine[0] = row;
                leftLastLine[1] = col;

                leftEdgeFind = 1;   //左边线标志量置位
                break;
            }else if(ImageC[row][col+1]&0x80){//左边线开始时丢失，暂时未解决
                leftEdgeStart = centerLine;
            }
        }
        for(col = rightEdgeStart;col<rightEdgeEnd;col++){   //右边线寻找
            if((ImageC[row][col]&0x80) && ImageC[row][col-1] == 0 ){
                img_EdgeInfo[row][1] = col;                         //获取图像边界信息
                world_EdgeInfo[row][1] = correctTable[row][col];    //获取世界边界信息

                //更新最后的右边线的行列
                rightLastLine[0] = row;
                rightLastLine[1] = col;

                rightEdgeFind = 1;  //右边线标志量置位
                break;
            }else if(ImageC[row][col-1]&0x80){//右边线开始时丢失，暂时未解决
                rightEdgeStart = centerLine;
            }
        }

        if(leftEdgeFind&&rightEdgeFind){

        	img_EdgeInfo[row][2] = (img_EdgeInfo[row][0] + img_EdgeInfo[row][1])*0.5;       //求取图像坐标中线
        	world_EdgeInfo[row][2] = (world_EdgeInfo[row][0] + world_EdgeInfo[row][1])*0.5; //求取世界坐标中线

        	centerLine = img_EdgeInfo[row][2];  //上次中线置为图像坐标中线

        	//Image_Ptr[Image_Row[row] * 20 + img_EdgeInfo[row][2]/8] |= 1<<(7-img_EdgeInfo[row][2]&0x07);
        	//Image_Ptr[Image_Row[row] * 20 + world_EdgeInfo[row][2]/8] |= 1<<(7-world_EdgeInfo[row][2]&0x07);

            leftEdgeStart =   img_EdgeInfo[row][0] + 15;    //改变下一行左边线搜寻开始位置
            leftEdgeEnd   =   img_EdgeInfo[row][0] - 5;    //改变下一行左边线搜寻结束位置
            rightEdgeStart =  img_EdgeInfo[row][1] - 15;    //改变下一行右边线搜寻开始位置
            rightEdgeEnd  =   img_EdgeInfo[row][1] + 5;    //改变下一行右边线搜寻结束位置

            img_EdgeInfo[row][3] = 1;   //中线类型设为 左中右都可用


        }else if(leftEdgeFind){
            //temp = rightLastLine[1];

        	//img_EdgeInfo[row][2] = (img_EdgeInfo[row][0] + temp)*0.5;
        	//world_EdgeInfo[row][2] = (world_EdgeInfo[row][0] + world_EdgeInfo[temp][1])*0.5;

            leftEdgeStart = img_EdgeInfo[row][0] + 15;    //改变下一行左边线搜寻开始位置
            leftEdgeEnd =   img_EdgeInfo[row][0] - 5;    //改变下一行左边线搜寻结束位置

            img_EdgeInfo[row][3] = 2;   //中线类型设为 左可用

            leftEdgeMissCnt++;

        }else if(rightEdgeFind){
            //temp = leftLastLine[1];

        	//img_EdgeInfo[row][2] = (img_EdgeInfo[row][1] + leftLastLine[1])*0.5;
        	//world_EdgeInfo[row][2] = (world_EdgeInfo[temp][0] + world_EdgeInfo[row][1])*0.5;

            rightEdgeStart = img_EdgeInfo[row][1] - 15;     //改变下一行右边线搜寻开始位置
            rightEdgeEnd =   img_EdgeInfo[row][1] + 5;     //改变下一行右边线搜寻结束位置

            img_EdgeInfo[row][3] = 3;   //中线类型设为 右可用

            rightEdgeMissCnt++;

        }else{
            leftEdgeStart = centerLine;     //重置下一行左边线搜寻开始位置
            rightEdgeStart = centerLine+1;  //重置下一行右边线搜寻开始位置

            /*
            if(leftEdgeStart-leftEdgeEnd>15)
                leftEdgeEnd++;
            if(rightEdgeStart-rightEdgeEnd>15)
                rightEdgeEnd--;
            */

            img_EdgeInfo[row][3] = 4;   //中线类型设为 三线丢失

        }

        if(row == IMG_H-6){
            if(leftEdgeMissCnt == 5&&!rightEdgeMissCnt){
                edgeFillMode = 1;
            }else if(rightEdgeMissCnt == 5&&!leftEdgeMissCnt){
                edgeFillMode = 1;
            }
        }

        /*校正左右边线开始结束位置变量，防止超出数组边界*/
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

        leftEdgeFind = 0;   //左标志位清零
        rightEdgeFind = 0;  //右标志位清零

    }

#endif
#if 1
    imgEdgeFill4Cnt = 0;
    imgEdgeFill4[imgEdgeFill4Cnt][0] = 0;
    imgEdgeFill4[imgEdgeFill4Cnt][2] = 0;
    for(row = IMG_H-2;row>0;row--){     //中线处理循环

        if(img_EdgeInfo[row-1][2]&&img_EdgeInfo[row+1][2]){     //对三边线都可用时进行边线滤波

            img_EdgeInfo[row][2] = (img_EdgeInfo[row-1][2] + img_EdgeInfo[row+1][2])*0.5;   //中线滤波
            world_EdgeInfo[row][2] = correctTable[row][img_EdgeInfo[row][2]];               //重新获取获取世界边界信息

            edgeFillMode = 1;
            lastCenterLine = img_EdgeInfo[row][2];
        }else if(edgeFillMode && img_EdgeInfo[row][3] == 2 || img_EdgeInfo[row][3] == 3){   //对一边线可用时进行中线补线
            if(img_EdgeInfo[row][3] == 2){
                //temp = world_EdgeInfo[row][1];
                img_EdgeInfo[row][2] = img_EdgeInfo[row][0] + colCorrectTable[row]*14;   //获取获取世界边界信息根据真实世界赛道宽度获取中线
                world_EdgeInfo[row][2] = correctTable[row][img_EdgeInfo[row][2]];       //获取获取世界边界信息

                if(img_EdgeInfo[row][2]<IMG_W)  //根据情况是否显示到图像数组中
                    Image_Ptr[Image_Row[row] * 20 + (uint8)(img_EdgeInfo[row][2]/8)] |= 1<<(7-img_EdgeInfo[row][2]&0x07);

            }else if(img_EdgeInfo[row][3] == 3){
                //temp = world_EdgeInfo[row][2];
                img_EdgeInfo[row][2] = img_EdgeInfo[row][1] - colCorrectTable[row]*14;   //获取获取世界边界信息根据真实世界赛道宽度获取中线
                world_EdgeInfo[row][2] = correctTable[row][img_EdgeInfo[row][2]];       //获取获取世界边界信息

                if(img_EdgeInfo[row][2]>0)      //根据情况是否显示到图像数组中
                    Image_Ptr[Image_Row[row] * 20 + (uint8)(img_EdgeInfo[row][2]/8)] |= 1<<(7-img_EdgeInfo[row][2]&0x07);

            }
        }else if(img_EdgeInfo[row][3] == 4){    //对三边线都不可用时进行记录，以便判断是否需要补线
            if(!imgEdgeFill4[imgEdgeFill4Cnt][0]){  //记录起始位置
                if(img_EdgeInfo[row+1][3]!=4){
                    imgEdgeFill4[imgEdgeFill4Cnt][0] = row+1;
                    imgEdgeFill4[imgEdgeFill4Cnt][1] = img_EdgeInfo[row+1][2];
                }
                if(row >= IMG_H-2){       //开始边界情况
                    imgEdgeFill4[imgEdgeFill4Cnt][0] = IMG_H-2;
                    imgEdgeFill4[imgEdgeFill4Cnt][1] = IMG_W_HALF;
                }
            }else if(!imgEdgeFill4[imgEdgeFill4Cnt][2]){
                if(img_EdgeInfo[row-1][3]!=4){
                    imgEdgeFill4[imgEdgeFill4Cnt][2] = row-1;
                    //imgEdgeFill4[imgEdgeFill4Cnt][3] = img_EdgeInfo[row-1][2];

                    imgEdgeFill4Cnt++;
                    imgEdgeFill4[imgEdgeFill4Cnt][0] = 0;
                    imgEdgeFill4[imgEdgeFill4Cnt][2] = 0;
                }
                if(row <=1){              //结束边界情况
                    imgEdgeFill4[imgEdgeFill4Cnt][2] = 1;
                    imgEdgeFill4[imgEdgeFill4Cnt][3] = IMG_W_HALF;

                    imgEdgeFill4Cnt++;
                    imgEdgeFill4[imgEdgeFill4Cnt][0] = 0;
                    imgEdgeFill4[imgEdgeFill4Cnt][2] = 0;
                }
            }
            edgeFillMode = 1;
        }else if(img_EdgeInfo[row][3] == 5&&img_EdgeInfo[row+2][3] != 1&&row>5){
            if(leftLastLine[0] > rightLastLine[0]){
                v = leftLastLine[1];
                u = (leftLastLine[0]+row)*0.5;
                topEdgeStart = u;
                bottomEdgeStart = u;
                topEdgeEnd = row - 5;
                bottomEdgeEnd = leftLastLine[0] + 5;
                direct = -1;
                colEnd = 0;
                colEdgeEnable = 1;

            }else if(leftLastLine[0] < rightLastLine[0]){
                v = rightLastLine[1];
                u = (rightLastLine[0]+row)*0.5;
                topEdgeStart = u;
                bottomEdgeStart = u;
                topEdgeEnd = row - 5;
                bottomEdgeEnd = rightLastLine[0] + 5;
                direct = 1;
                colEnd = IMG_W-1;
                colEdgeEnable = 1;
            }
        }
        if(img_EdgeInfo[row][2] !=0)
            Image_Ptr[Image_Row[row] * 20 + (uint8)(img_EdgeInfo[row][2]/8)] |= 1<<(7-img_EdgeInfo[row][2]&0x07);
    }
    Image_Ptr[Image_Row[row] * 20 + (uint8)(img_EdgeInfo[row][2]/8)] |= 1<<(7-img_EdgeInfo[row][2]&0x07);

#endif

#if 1
    while(imgEdgeFill4Cnt){
        imgEdgeFill4Cnt--;
        v = imgEdgeFill4[imgEdgeFill4Cnt][1];

        imgEdgeFill4Row = imgEdgeFill4[imgEdgeFill4Cnt][0] - imgEdgeFill4[imgEdgeFill4Cnt][2];
        if(imgEdgeFill4[imgEdgeFill4Cnt][2]==1)
                imgEdgeFill4Col = imgEdgeFill4[imgEdgeFill4Cnt][1] - imgEdgeFill4[imgEdgeFill4Cnt][3];
        else
                imgEdgeFill4Col = img_EdgeInfo[imgEdgeFill4[imgEdgeFill4Cnt][2]][2] - imgEdgeFill4[imgEdgeFill4Cnt][1];

        for(row = imgEdgeFill4[imgEdgeFill4Cnt][0] ;row > imgEdgeFill4[imgEdgeFill4Cnt][2] ;row--){
        	img_EdgeInfo[row][2] = (uint16)(v + ((float)(imgEdgeFill4[imgEdgeFill4Cnt][0]  - row))/imgEdgeFill4Row * imgEdgeFill4Col);
            world_EdgeInfo[row][2] = correctTable[row][img_EdgeInfo[row][2]];//获取世界边界信息
            Image_Ptr[Image_Row[row] * 20 + (uint8)(img_EdgeInfo[row][2]/8)] |= 1<<(7-img_EdgeInfo[row][2]&0x07);

        }
    }
#endif

#if 1

    if(colEdgeEnable)
    {
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

        	//Image_Ptr[Image_Row[topEdgeStart] * 20 + col/8] |= 1<<(7-col&0x07);
        	//Image_Ptr[Image_Row[bottomEdgeStart] * 20 + col/8] |= 1<<(7-col&0x07);
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

                img_ColEdgeInfo[colEdgeCnt][2] = (uint8)((img_ColEdgeInfo[colEdgeCnt][0]+img_ColEdgeInfo[colEdgeCnt][1])*0.5);
                world_ColEdgeInfo[colEdgeCnt][2] = correctTable[img_ColEdgeInfo[colEdgeCnt][2]][col];//获取世界边界信息

        	    Image_Ptr[Image_Row[img_ColEdgeInfo[colEdgeCnt][2]] * 20 + (uint8)(img_ColEdgeInfo[colEdgeCnt][5]/8)] |= 1<<(7-img_ColEdgeInfo[colEdgeCnt][5]&0x07);
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
            sprintf(TXBuffer,"[%u]",colEdgeEnable);
            TUart0_Puts(TXBuffer);

        if(colBool){

            if(direct == 1){
                temp = rightLastLine[0] - firstRow;
                data = firstCol - img_EdgeInfo[rightLastLine[0]][2];
                //sprintf(TXBuffer,"[%d+%d]",temp,data);
                //TUart0_Puts(TXBuffer);
                for(row = rightLastLine[0];row >= rightLastLine[0]-temp;row--){
                	img_EdgeInfo[row][2] = (uint16)(img_EdgeInfo[rightLastLine[0]][2] + ((float)(rightLastLine[0] - row))/(temp+1) * data);
                    world_EdgeInfo[row][2] = correctTable[row][img_EdgeInfo[row][2]];//获取世界边界信息

                    Image_Ptr[Image_Row[row] * 20 + (uint8)(img_EdgeInfo[row][2]/8)] |= 1<<(7-img_EdgeInfo[row][2]&0x07);
                    //sprintf(TXBuffer,"[%d]\n",img_EdgeInfo[row][2]);
                    //TUart0_Puts(TXBuffer);
                }
            }else{
                temp = leftLastLine[0] - firstRow;
                data = img_EdgeInfo[rightLastLine[0]][2] - firstCol;
                for(row = leftLastLine[0];row>topEdgeEnd;row--){
                	img_EdgeInfo[row][2] = (uint16)(img_EdgeInfo[leftLastLine[0]][2] + ((float)(leftLastLine[0] - row))/(temp+1) * data);
                    world_EdgeInfo[row][2] = correctTable[row][img_EdgeInfo[row][2]];//获取世界边界信息

                    Image_Ptr[Image_Row[row] * 20 + (uint8)(img_EdgeInfo[row][2]/8)] |= 1<<(7-img_EdgeInfo[row][2]&0x07);
                }
            }
        }

    }


#endif

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
                        lineOffsetRow = row;
                        lineOffsetCnt ++;

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
                        midThreePointCol1 = (threePoint[2][0] + threePoint[0][0])*0.5;
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
                        midThreePointCol1 = (threePoint[2][0] + threePoint[0][0])*0.5;
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
                            controlMode++;

                        }else if(tp2ColCnt<=tp2RowCnt){
                            threePoint2[1][0] = threePoint2[2][0];
                            threePoint2[1][1] = threePoint2[2][1];
                            threePoint2[2][0] = img_ColEdgeInfo[temp-1][2];
                            threePoint2[2][1] = img_ColEdgeInfo[temp-1][5];
                            controlFinsh = 1;
                            controlMode++;
                        }
                    }else{
                        midThreePoint2Col1 = (threePoint2[2][0] + threePoint2[0][0])*0.5;
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
                            controlMode++;
                        }

                    }
                }
            }break;
        }
    }
#endif
#if 1
    switch(controlMode){
        case 3:{
            tempa = threePoint2[0][0];
            tempb = threePoint2[0][1];
            temp = ((threePoint2[1][0]-tempa)*(threePoint2[2][1]-tempb) - (threePoint2[2][0]-tempa)*(threePoint2[1][1]-tempb))*0.5;
            if(temp == 0){
                controlMode = 2;
                break;
            }
            tempa = threePoint2[1][0] - threePoint2[0][0];
            tempb = threePoint2[1][1] - threePoint2[0][1];
            threePoint2[0][2] = sqrt_16(tempa*tempa+tempb*tempb);
            tempa = threePoint2[2][0] - threePoint2[1][0];
            tempb = threePoint2[2][1] - threePoint2[1][1];
            threePoint2[1][2] = sqrt_16(tempa*tempa+tempb*tempb);
            tempa = threePoint2[2][0] - threePoint2[0][0];
            tempb = threePoint2[2][1] - threePoint2[0][1];
            threePoint2[2][2] = sqrt_16(tempa*tempa+tempb*tempb);

            radiusOfCurva_2 = threePoint2[0][2] * threePoint2[1][2] * threePoint2[2][2] / temp * 0.25;
            //tp2ResSteerAngle = 90 / radiusOfCurva_2;
        }
        case 2:{
            tempa = threePoint[0][0];
            tempb = threePoint[0][1];
            temp = ((threePoint[1][0]-tempa)*(threePoint[2][1]-tempb) - (threePoint[2][0]-tempa)*(threePoint[1][1]-tempb))*0.5;
            if(temp <= 1){
                temp = ((threePoint[1][0]-IMG_W_HALF)*(threePoint[2][1]-tempb) - (threePoint[2][0]-IMG_W_HALF)*(threePoint[1][1]-tempb))*0.5;
                tempa = threePoint[1][0] - IMG_W_HALF;
                tempb = threePoint[1][1] - threePoint[0][1];
                threePoint[0][2] = sqrt_16(tempa*tempa+tempb*tempb);
                tempa = threePoint[2][0] - threePoint[1][0];
                tempb = threePoint[2][1] - threePoint[1][1];
                threePoint[1][2] = sqrt_16(tempa*tempa+tempb*tempb);
                tempa = threePoint[2][0] - IMG_W_HALF;
                tempb = threePoint[2][1] - threePoint[0][1];
                threePoint[2][2] = sqrt_16(tempa*tempa+tempb*tempb);
            }else{
                tempa = threePoint[1][0] - threePoint[0][0];
                tempb = threePoint[1][1] - threePoint[0][1];
                threePoint[0][2] = sqrt_16(tempa*tempa+tempb*tempb);
                tempa = threePoint[2][0] - threePoint[1][0];
                tempb = threePoint[2][1] - threePoint[1][1];
                threePoint[1][2] = sqrt_16(tempa*tempa+tempb*tempb);
                tempa = threePoint[2][0] - threePoint[0][0];
                tempb = threePoint[2][1] - threePoint[0][1];
                threePoint[2][2] = sqrt_16(tempa*tempa+tempb*tempb);

            }

            radiusOfCurva_1 = threePoint[0][2] * threePoint[1][2] * threePoint[2][2] / temp*0.25;
            //tpResSteerAngle = 90 / radiusOfCurva_1;
        }
        case 1:{
            if((50-lineOffsetRow-lineOffsetCnt)>5){
                lineRealDis = world_ActualRange[lineOffsetRow];
                lineRealRate = 50-lineOffsetRow;

                lineResSteerAngle = ((float)lineOffsetValueCnt/lineRealRate*colCorDisTable[lineOffsetRow])/lineRealDis*28.647;
            }else{
                lineRealDis = world_ActualRange[50-lineOffsetCnt];
                lineRealRate = lineOffsetCnt;
                lineResSteerAngle = ((float)lineOffsetValueCnt/lineRealRate*colCorDisTable[50-lineOffsetCnt])/lineRealDis*28.647;
            }
        }break;
    }
#endif
    //TUart0_Puts("PIT1!\r\n");
    MCF_PIT1_PCSR &= ~MCF_PIT_PCSR_EN;
    MCF_PIT1_PCSR |= MCF_PIT_PCSR_PIF;  //清中断标志
    bool = 0;
}
#endif
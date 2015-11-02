#include "DIY_ComDef.h"
#ifdef TZ_MCF52259_PIT

#include "math.h"
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
int cntt;

float err_sl = 0.0,err_sr = 0.0;                  //定义偏差值
float err_slastl = 0.0,err_slastr = 0.0;             //定义上一个偏差值
float err_sprevl = 0.0,err_sprevr = 0.0;             //定义上上个偏差值
float Kp = 0.4,Ki = 0.2,Kd = 0.0;             //定义比例、积分、微分系数

int SetSpeed = 0;             //定义设定值
int ActSpeed = 0;             //定义实际值

int SetAngle = 0;             //定义设定值
int ActAngle = 0;             //定义实际值

float err_a = 0.0;                  //定义偏差值
float err_alast = 0.0,err_aprev = 0.0;             //定义上一个偏差值
float Kp_a = 0.3,Ki_a = 0.0,Kd_a = 0.0;       //定义比例、积分、微分系数
float integral_a = 0.0;             //定义积分值

void TPITx_Init(uint8 mode){
    switch(mode){
        default:case 0:{
            MCF_PIT0_PCSR = 0;
            //                 自动重载         计数为0中断         使能中断            立即载入PMR值       80000000 / 2^10 = 78125 2^11 = 39062 2^12=19531
            MCF_PIT0_PCSR = MCF_PIT_PCSR_RLD | MCF_PIT_PCSR_PIF | MCF_PIT_PCSR_PIE | MCF_PIT_PCSR_OVW | MCF_PIT_PCSR_PRE(TPIT0_PRECLK);
            //MCF_PIT0_PMR = TPIT0_ZQ * 1000;   //设置计数值
            MCF_PIT0_PMR = 1;   //设置计数值
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

    if(leftSpeedBool){
        leftSSSum = 0;
    }else{
        leftSSSum = (uint16)((leftMotorCnt[0]+leftMotorCnt[1]+leftMotorCnt[2]+leftMotorCnt[3]+leftMotorCnt[4]+leftMotorCnt[5]+leftMotorCnt[6]+leftMotorCnt[7])>>3);//算术平均
        leftSpeedBool = 1;
    }

    if(rightSpeedBool){
        rightSSSum = 0;
    }else{
        rightSSSum = (uint16)((rightMotorCnt[0]+rightMotorCnt[1]+rightMotorCnt[2]+rightMotorCnt[3]+rightMotorCnt[4]+rightMotorCnt[5]+rightMotorCnt[6]+rightMotorCnt[7])>>3);//算术平均
        rightSpeedBool = 1;
    }

    switch(leftSSSum+rightSSSum){
        case 0:{
            motorSpeed = 0;
        }break;
        default:{
            motorSpeed = (float)(((1576032.314/leftSSSum) + (1576032.314/rightSSSum))*0.5);
            if(leftSSSum>35000||leftSSSum<1600){
                leftMotorSpeed = 0;
                motorSpeed = (float)((1576032.314/rightSSSum)*0.5);
            }else
                leftMotorSpeed = (uint16)(1576032.314/leftSSSum);
            if(rightSSSum>35000||rightSSSum<1600){
                rightMotorSpeed = 0;
                motorSpeed = (float)((1576032.314/leftSSSum)*0.5);
            }else
                rightMotorSpeed = (uint16)(1576032.314/rightSSSum);
        }break;
    }

/*
    for(row = IMG_H-1;motorSpeed>world_ActualRange[50-row];row--){
        if(img_EdgeInfo[row][3] == 5 || img_EdgeInfo[row][3] == 7 ){
            row++;
            break;
        }
        if(lineOffsetValueCnt[row]>350 || lineOffsetValueCnt[row]<-350)
            break;
    }

    motorCnt1 = lineOffsetValueCnt[row];
    motorSpeed = (uint16)((leftMotorSpeed + rightMotorSpeed)>>1);

    u = 0;
    motorCnt2 = 0;
    if(colEdgeEnable&&motorCnt1>0&&motorCnt1<350||motorCnt1>-350&&motorCnt1<0){

        for(;motorSpeed>world_ActualRange[row-u];u++){
        //for(;u<10;u++){
            if(img_ColEdgeInfo[u][3] == 5)
                break;
            if(img_ColEdgeInfo[u][3] == 1){
                motorCnt2 = motorCnt2 + (img_ColEdgeInfo[u][5] - IMG_W_HALF);
            }else if(img_ColEdgeInfo[u][0] == 0&&img_ColEdgeInfo[u][1] == 0){
                break;
            }
        }
    }

    //lineOffsetValue[row]

    if(u)
        motorCnt = motorCnt1/(50.0 - row) + motorCnt2/u;
    else
        motorCnt = motorCnt1/(50.0 - row);

    //motorCnt = motorCnt/(50 - row + u);
*/
    //motorCnt = SlopeAve * 250;
#if 0

    //检测下一幅图像开始的行数
    for(row = 49;row>0;row--){
        if(motorSpeed*0.01333<world_ActualRange[row]){
            break;
        }
    }
    lenOf2Screen_Row = row;

    //检测速度指向的行数和下一幅图像开始的行数到速度指向的行数的差值
    lenOfSpeed_Cnt = 0;
    for(row = lenOf2Screen_Row;row>0;row--){
        if(motorSpeed<world_ActualRange[row]){
            break;
        }
        lenOfSpeed_Cnt++;
    }
    lenOfSpeed_Row = row;

    //权重求出舵机偏移源值
    steerOffsetCnt = 0.0;
    for(row = lenOf2Screen_Row;row>lenOfSpeed_Row;row--){
        if(img_EdgeInfo[row][3] == 5 || img_EdgeInfo[row][3] == 7 ){
            row++;
            break;
        }
        if((row - lenOfSpeed_Row)<lenOfSpeed_Cnt*0.5)
            steerOffsetCnt += (float)(row - lenOfSpeed_Row)/lenOfSpeed_Cnt * lineOffsetValue[row];
        else
            steerOffsetCnt += (float)(1-(row - lenOfSpeed_Row)/lenOfSpeed_Cnt + 0.5) * lineOffsetValue[row];
    }
    //steerOffsetCnt =  lineOffsetValueCnt[lenOfSpeed_Row] - lineOffsetValueCnt[lenOf2Screen_Row];
    if(0&&colEdgeEnable&&row>lenOfSpeed_Row){  //如果没获得足够的点并且横向扫描成功，则将横向扫描加进偏移值
        u = 0;
        for(;row>lenOfSpeed_Row;row--,u++){
            if(img_ColEdgeInfo[u][3] == 5){
                break;
            }
            steerOffsetCnt += (float)(row - lenOfSpeed_Row)/lenOfSpeed_Cnt * (img_ColEdgeInfo[u][5] - 80);
            //steerOffsetCnt += img_ColEdgeInfo[u][5] - 80;
        }
        if(row>lenOfSpeed_Row){                 //如果没获得足够的点，则将上次值不断重复加进偏移值
            u--;
            for(;row>lenOfSpeed_Row;row--){
                steerOffsetCnt += (float)(row - lenOfSpeed_Row)/lenOfSpeed_Cnt * (img_ColEdgeInfo[u][5] - 80);
                //steerOffsetCnt += img_ColEdgeInfo[u][5] - 80;
            }
        }
    }else if(0&&row>lenOfSpeed_Row){           //如果没获得足够的点并且横向扫描不成功，则将上次值不断重复加进偏移值
        row++;
        for(;row>lenOfSpeed_Row;row--){
            steerOffsetCnt += (float)(row - lenOfSpeed_Row)/lenOfSpeed_Cnt * lineOffsetValue[row];
            //steerOffsetCnt += lineOffsetValue[row];
        }
    }
    if(lenOfSpeed_Cnt){
        if(img_EdgeInfo[49][2])
            //motorCnt = steerOffsetCnt/(lenOfSpeed_Cnt*40.0)*(1+((img_EdgeInfo[49][2]-80)/80.0))*350;
            motorCnt = steerOffsetCnt/(lenOfSpeed_Cnt*40.0)*350;
            //motorCnt = steerOffsetCnt/lenOfSpeed_Cnt/80.0*350;
        else
            //motorCnt = steerOffsetCnt/(lenOfSpeed_Cnt*40.0)*(1+((img_EdgeInfo[48][2]-80)/80.0))*350;
            motorCnt = steerOffsetCnt/(lenOfSpeed_Cnt*40.0)*350;
            //motorCnt = steerOffsetCnt/lenOfSpeed_Cnt/80.0*350;
    }
    //舵机偏移值限幅？
    if(motorCnt){
        if(lineOffsetValueCnt[row]>0){
            if(motorCnt>0){
                SetSpeed = 80+(1 - lineOffsetValueCnt[row]/((50-row)*40.0))*120;
            }else{
                SetSpeed = 80+(1 - lineOffsetValueCnt[row]/((50-row)*40.0))*120;
            }
        }else{
            if(motorCnt>0){
                SetSpeed = 80+(1 + lineOffsetValueCnt[row]/((50-row)*40.0))*120;
            }else{
                SetSpeed = 80+(1 + lineOffsetValueCnt[row]/((50-row)*40.0))*120;
            }
        }
    }

#endif
#if 0

    for(row = 49;row>0;row--){
        if((img_EdgeInfo[row][3] == 5 || img_EdgeInfo[row][3] == 7 )){
            row++;
            break;
        }
        if(lineOffsetValue[row]<-10||lineOffsetValue[row]>10){
            break;
        }
    }
    if(row<49){
        SlopeAve = ((49-row)*1.332+26.8)*(50-row)*0.5;
        if(lineOffsetValueCnt[row]>0){
            if(motorCnt>0){
                SetSpeed = 80+(1 - lineOffsetValueCnt[row]/SlopeAve)*60;
                //motorCnt =  (lineOffsetValue[row]/80.0 - lineOffsetValue[0])*350*(1 - lineOffsetValueCnt[row]/((50-row)*40.0));
            }else{
                SetSpeed = 80+(1 - lineOffsetValueCnt[row]/SlopeAve)*60;
                //motorCnt =  (lineOffsetValue[row]/80.0 - lineOffsetValue[0])*350*(1 - lineOffsetValueCnt[row]/((50-row)*40.0));
            }
        }else{
            if(motorCnt>0){
                SetSpeed = 80+(1 + lineOffsetValueCnt[row]/SlopeAve)*60;
                //motorCnt =  (lineOffsetValue[row]/80.0 - lineOffsetValue[0])*350*(1 + lineOffsetValueCnt[row]/((50-row)*40.0));
            }else{
                SetSpeed = 80+(1 + lineOffsetValueCnt[row]/SlopeAve)*60;
                //motorCnt =  (lineOffsetValue[row]/80.0 - lineOffsetValue[0])*350*(1 + lineOffsetValueCnt[row]/((50-row)*40.0));
            }
        }
    }

    for(row = 49;row>0;row--){
        if((img_EdgeInfo[row][3] == 5 || img_EdgeInfo[row][3] == 7 )){
            row++;
            break;
        }
        if(SetSpeed*0.5<world_ActualRange[row]){
            break;
        }
    }
    if(SetSpeed*0.5>world_ActualRange[row]&&colEdgeEnable){
        for(u = 0;row>0;row--){
            u++;
            if(SetSpeed<world_ActualRange[row]){
                break;
            }
        }
        motorCnt = ((img_ColEdgeInfo[u][5] - 80)/((50-row)*1.332+13.4)*350);
    }else{
        motorCnt = (lineOffsetValue[row]/((50-row)*1.332+13.4)*350);
    }
    if(motorCnt>350)
        motorCnt = 350;
    if(motorCnt<-350)
        motorCnt = -350;

    if(SetSpeed<80)
        SetSpeed = 80;
#endif

#if 1
    switch(enCoder){
        case 0:{Kp_a = 0.5;Kd_a = 0.1;}break;
        case 1:{Kp_a = 0.5;Kd_a = 0.3;}break;
        case 2:{Kp_a = 0.5;Kd_a = 0.5;}break;
        case 3:{Kp_a = 0.5;Kd_a = 0.7;}break;
        case 4:{Kp_a = 0.55;Kd_a = 0.1;}break;
        case 5:{Kp_a = 0.55;Kd_a = 0.3;}break;
        case 6:{Kp_a = 0.55;Kd_a = 0.5;}break;
        case 7:{Kp_a = 0.55;Kd_a = 0.7;}break;
        case 8:{Kp_a = 0.6;Kd_a = 0.1;}break;
        case 9:{Kp_a = 0.6;Kd_a = 0.3;}break;
        case 10:{Kp_a = 0.6;Kd_a = 0.5;}break;
        case 11:{Kp_a = 0.6;Kd_a = 0.7;}break;
        case 12:{Kp_a = 0.65;Kd_a = 0.1;}break;
        case 13:{Kp_a = 0.65;Kd_a = 0.3;}break;
        case 14:{Kp_a = 0.65;Kd_a = 0.5;}break;
        case 15:{Kp_a = 0.65;Kd_a = 0.7;}break;
        default:{Kp_a = 1;Kd_a = 0;}break;
    }
    for(row = 49;row>30;row--){
        if((img_EdgeInfo[row][3] == 5 || img_EdgeInfo[row][3] == 7 )){
            row++;
            break;
        }
    }
    motorCnt = ((img_EdgeInfo[row][2] - 80)/80.0*350);
    sprintf(TXBuffer,"%d\t%d\t",img_EdgeInfo[row][2] - 80,motorCnt);
    TUart0_Puts(TXBuffer);

    SetSpeed = 160;
#endif
#if 0
    for(row = 49;row>0;row--){
        if((img_EdgeInfo[row][3] == 5 || img_EdgeInfo[row][3] == 7 )){
            row++;
            break;
        }
        if(motorSpeed*0.5<world_ActualRange[row]){
            break;
        }
    }
    if(motorSpeed*0.5>world_ActualRange[row]&&colEdgeEnable){
        for(u = 0;row>0;row--){
            u++;
            if(motorSpeed<world_ActualRange[row]){
                break;
            }
        }
        motorCnt = ((img_ColEdgeInfo[u][5] - 80)/((50-row)*1.332+13.4)*350);
    }else{
        motorCnt = (lineOffsetValue[row]/((50-row)*1.332+13.4)*350);
    }
    if(motorCnt>350)
        motorCnt = 350;
    if(motorCnt<-350)
        motorCnt = -350;
    for(row = 49;row>0;row--){
        if((img_EdgeInfo[row][3] == 5 || img_EdgeInfo[row][3] == 7 )){
            row++;
            break;
        }
        if(lineOffsetValue[row]<-10||lineOffsetValue[row]>10){
            break;
        }
    }

    SlopeAve = (1 - lineOffsetValueCnt[row]/(((49-row)*1.332+26.8)*(50-row)*0.5))*60;
    if(motorCnt>0)
        SlopeAve = SlopeAve - SlopeAve * motorCnt/350.0;
    else
        SlopeAve = SlopeAve - SlopeAve * motorCnt/350.0;

    SetSpeed = 80+SlopeAve;

    if(SetSpeed<80)
        SetSpeed = 80;
#endif
#if 0
    if(motorSpeed>80)
        motorOffset = 40*(1-(motorSpeed-80)/120.0);
    else
        motorOffset = 40*(1-motorSpeed/120.0);
    for(row = 49;row>25;row--){
        if((img_EdgeInfo[row][3] == 5 || img_EdgeInfo[row][3] == 7 )){
            row++;
            break;
        }
        if(lineOffsetValue[row]<-10||lineOffsetValue[row]>10){
            break;
        }
    }
    lenOfSpeed_Row = row;
    motorCnt = -(lineOffsetValue[row]/40.0*350);
    if(row<49){
        if(lineOffsetValueCnt[row]>0){
            if(motorCnt>0){
                SetSpeed = 80+(1 - lineOffsetValueCnt[row]/((50-row)*40.0))*120;
                //motorCnt =  (lineOffsetValue[row]/80.0 - lineOffsetValue[0])*350*(1 - lineOffsetValueCnt[row]/((50-row)*40.0));
            }else{
                SetSpeed = 80+(1 - lineOffsetValueCnt[row]/((50-row)*40.0))*120;
                //motorCnt =  (lineOffsetValue[row]/80.0 - lineOffsetValue[0])*350*(1 - lineOffsetValueCnt[row]/((50-row)*40.0));
            }
        }else{
            if(motorCnt>0){
                SetSpeed = 80+(1 + lineOffsetValueCnt[row]/((50-row)*40.0))*120;
                //motorCnt =  (lineOffsetValue[row]/80.0 - lineOffsetValue[0])*350*(1 + lineOffsetValueCnt[row]/((50-row)*40.0));
            }else{
                SetSpeed = 80+(1 + lineOffsetValueCnt[row]/((50-row)*40.0))*120;
                //motorCnt =  (lineOffsetValue[row]/80.0 - lineOffsetValue[0])*350*(1 + lineOffsetValueCnt[row]/((50-row)*40.0));
            }
        }
    }
    u = 0;
    if(0&&colEdgeEnable&&motorCnt&&motorSpeed>world_ActualRange[row]){
        for(;u<colEdgeCnt&&row>1;u++){
            if(img_ColEdgeInfo[u][3] == 5){
                u--;
                break;
            }
            if(img_ColEdgeInfo[u][2]==0 && motorSpeed<world_ActualRange[row]){
                break;
            }
            row--;
        }
        if(u>2){
            motorCnt =  (img_ColEdgeInfo[u][5] - 80)/80.0*350*(50/(50-img_ColEdgeInfo[u][2]));
        }
    }

#endif
#if 0
    for(row = 49;row>0;row--){
        if((img_EdgeInfo[row][3] == 5 || img_EdgeInfo[row][3] == 7 )&&row<49){
            row++;
            break;
        }
        if(motorSpeed<world_ActualRange[row]){
            break;
        }
    }

    if(row<49){
        motorCnt =  lineOffsetValue[row]/80.0*350*(50/(50-row));
    }


    u = 0;
    if(colEdgeEnable&&motorCnt&&motorSpeed>world_ActualRange[row]){
        for(;u<colEdgeCnt&&row>1;u++){
            if(img_ColEdgeInfo[u][3] == 5){
                u--;
                break;
            }
            if(img_ColEdgeInfo[u][2]==0 && motorSpeed<world_ActualRange[row]){
                break;
            }
            row--;
        }
        if(u>2){
            motorCnt =  (img_ColEdgeInfo[u][5] - 80)/80.0*350*(50/(50-img_ColEdgeInfo[u][2]));
        }
    }
/*
    if(motorCnt>350)
        motorCnt = 350;
    if(motorCnt<-350)
        motorCnt = -350;
*/
    if(motorCnt){
        if(lineOffsetValueCnt[row]>0){
            if(motorCnt>0){
                SetSpeed = 80+(1 - lineOffsetValueCnt[row]/((50-row)*40.0))*100;
            }else{
                SetSpeed = 80+(1 - lineOffsetValueCnt[row]/((50-row)*40.0))*100;
            }
        }else{
            if(motorCnt>0){
                SetSpeed = 80+(1 + lineOffsetValueCnt[row]/((50-row)*40.0))*100;
            }else{
                SetSpeed = 80+(1 + lineOffsetValueCnt[row]/((50-row)*40.0))*100;
            }
        }
    }
    //SetSpeed = 80+(1 - lineOffsetValueCnt[row]/((50-row)*40.0))*130;
    //SetSpeed = 150;
    /*
    for(row = IMG_H-1;row>0;row--){
        if(img_EdgeInfo[row][3] == 5 || img_EdgeInfo[row][3] == 7 ){
            row++;
            break;
        }
        if(lineOffsetValue[row]<10&&lineOffsetValue[row]>-10){
            SetSpeed = world_ActualRange[row]*0.5+60;
            cntt = row;
        }
    }
    Kp_a = (50-row)/50 * 0.1;
    //SetSpeed = world_ActualRange[row]*0.5+60;
    //SetSpeed = 100*(50-row)/row+60;
    for(row = IMG_H-1;row>0;row--){
        if(img_EdgeInfo[row][3] == 5 || img_EdgeInfo[row][3] == 7 ){
            break;
        }
        if(motorSpeed<world_ActualRange[row]){
            break;
        }
    }
    motorCnt1 = lineOffsetValue[row+1];

    if(colEdgeEnable&&colEdgeCnt>1&&SetSpeed>world_ActualRange[img_ColEdgeInfo[0][2]]){
        motorCnt = (img_ColEdgeInfo[colEdgeCnt-1][5] - IMG_W_HALF)/80.0*350;
    }else{
        motorCnt = motorCnt1/40.0*350;;
    }
    */

#endif
/*********************************************/
/*****************电机PID*********************/
/*********************************************/


    ActSpeed = motorSpeed;

    motorOffsetSpeed = SetSpeed - ActSpeed;

    err_sl = motorOffsetSpeed+ActSpeed-leftMotorSpeed;
    err_sr = motorOffsetSpeed+ActSpeed-rightMotorSpeed;

    leftMSPwm =     leftMSPwm  + (float)((Kp * (err_sl - err_slastl + Ki * err_sl + Kd * (err_sl - 2 * err_slastl - err_sprevl)))*0.35714);
    err_sprevl = err_slastl;
    err_slastl = err_sl;
    if(leftMSPwm>250)
        leftMSPwm = 250;
    if(leftMSPwm<0)
        leftMSPwm = 0;

    rightMSPwm =    rightMSPwm + (float)((Kp * (err_sr - err_slastr + Ki * err_sr + Kd * (err_sr - 2 * err_slastr - err_sprevr)))*0.35714);
    err_sprevr = err_slastr;
    err_slastr = err_sr;
    if(rightMSPwm>250)
        rightMSPwm = 250;
    if(rightMSPwm<0)
        rightMSPwm = 0;

    //sprintf(TXBuffer,"l:%um/s\tr:%um/s\tset:%um/s\tact:%dm/s\tlp:%d\trp:%d\n",leftMotorSpeed,rightMotorSpeed,SetSpeed,ActSpeed,(int)leftMSPwm,(int)rightMSPwm);
    //TUart0_Puts(TXBuffer);

/*********************************************/
/*****************舵机PID*********************/
/*********************************************/

    SetAngle = 2880 - motorCnt;
    ActAngle = pwmCnt;

    err_a = (float)(SetAngle - ActAngle);

    integral_a += err_a;

    pwmCnt = pwmCnt + Kp_a * (err_a + Ki_a * integral_a + Kd_a * (err_a - err_alast));
    //pwmCnt = (uint16)(pwmCnt + (Kp_a * (err_a - err_alast + Ki_a * err_a + Kd_a * (err_a - 2 * err_alast - err_aprev))));
    //err_aprev = err_alast;
    err_alast = err_a;
    sprintf(TXBuffer,"%d\n",pwmCnt);
    TUart0_Puts(TXBuffer);

/*********************************************/
/*********************************************/

    if(pwmCnt>3230)
        pwmCnt = 3230;
    if(pwmCnt<2510)
        pwmCnt = 2510;

    TPWM45_SetDTY(pwmCnt);

    LEFT_MOTOR_PWM((uint8)leftMSPwm);
    RIGHT_MOTOR_PWM((uint8)rightMSPwm);

#elif 0
//电机PID测试程序
/*********************************************/
/*****************电机PID*********************/
/*********************************************/

    // SetSpeed = (1 - row / 50.0) * 120 + 80;


    ActSpeed = motorSpeed;

    sprintf(TXBuffer,"l:%um/s\tr:%um/s\tset:%um/s\tact:%dm/s\t",leftMotorSpeed,rightMotorSpeed,SetSpeed,ActSpeed);
    TUart0_Puts(TXBuffer);

    motorOffsetSpeed = SetSpeed - ActSpeed;

    err_sl = motorOffsetSpeed+ActSpeed-leftMotorSpeed;
    err_sr = motorOffsetSpeed+ActSpeed-rightMotorSpeed;

    leftMSPwm =     (uint8)(leftMSPwm  + (Kp * (err_sl - err_slastl + Ki * err_sl + Kd * (err_sl - 2 * err_slastl - err_sprevl)))*0.35714);
    err_slastl = err_sl;
    err_sprevl = err_slastl;

    rightMSPwm =    (uint8)(rightMSPwm + (Kp * (err_sr - err_slastr + Ki * err_sr + Kd * (err_sr - 2 * err_slastr - err_sprevr)))*0.35714);
    err_slastr = err_sr;
    err_sprevr = err_slastr;

    sprintf(TXBuffer,"ofc:%d\tlc:%u\trc:%u\tlp:%d\trp:%d\n",(int)motorOffsetCnt,leftSSSum,rightSSSum,(int)leftMSPwm,(int)rightMSPwm);
    TUart0_Puts(TXBuffer);
    LEFT_MOTOR_PWM((uint8)leftMSPwm);
    RIGHT_MOTOR_PWM((uint8)rightMSPwm);
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
uint16 sqrt_16(unsigned int M)
{
    int8 N, i;
    unsigned int tmp, ttp;   // 结果、循环计数
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

int16 invSqrt(int a,int b){
    float x;
    float xhalf;
    int i;          // get bits for floating value

    if(a==0||a==1){
        return (float)(b>0?b:(-b));
    }else if(b==0||b==1){
        return (float)(a>0?a:(-a));
    }
    x = (float)(a*a + b*b);
    xhalf = 0.5f * x;
    i = *(int*)&x;

    i =  0x5f375a86 - (i>>1);    // gives initial guess
    x = *(float*)&i;            // convert bits back to float
    x = x * (1.5f - xhalf*x*x); // Newton step
    return (int16)(1/x);
}

int length(int a,int b){

    int8 N, i;
    unsigned int tmp, ttp;   // 结果、循环计数
    unsigned int M;
    if(a==0||a==1){
        return (uint8)(b>0?b:(-b));
    }else if(b==0||b==1){
        return (uint8)(a>0?a:(-a));
    }
    M = a*a + b*b;
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
        Image_Ptr[Image_Row[row] * 20 + col] = (uint8)data;
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
            Image_Ptr[Image_Row[u] * 20 + v - 1] = (uint8)data;
            *ptr = (uint8)(data&0x80);
            data=(uint8)(data<<1);
            ptr++;
            *ptr = (uint8)(data&0x80);
            data=(uint8)(data<<1);
            ptr++;
            *ptr = (uint8)(data&0x80);
            data=(uint8)(data<<1);
            ptr++;
            *ptr = (uint8)(data&0x80);
            data=(uint8)(data<<1);
            ptr++;
            *ptr = (uint8)(data&0x80);
            data=(uint8)(data<<1);
            ptr++;
            *ptr = (uint8)(data&0x80);
            data=(uint8)(data<<1);
            ptr++;
            *ptr = (uint8)(data&0x80);
            data=(uint8)(data<<1);
            ptr++;
            *ptr = (uint8)(data&0x80);
            ptr++;
        }
        data = Image[Image_Row[u]][0];
        Image_Ptr[Image_Row[u] * 20 + v - 1] = (uint8)data;
        *ptr = (uint8)(data&0x80);
        data=(uint8)(data<<1);
        ptr++;
        *ptr = (uint8)(data&0x80);
        data=(uint8)(data<<1);
        ptr++;
        *ptr = (uint8)(data&0x80);
        data=(uint8)(data<<1);
        ptr++;
        *ptr = (uint8)(data&0x80);
        data=(uint8)(data<<1);
        ptr++;
        *ptr = (uint8)(data&0x80);
        data=(uint8)(data<<1);
        ptr++;
        *ptr = (uint8)(data&0x80);
        data=(uint8)(data<<1);
        ptr++;
        *ptr = (uint8)(data&0x80);
        data=(uint8)(data<<1);
        ptr++;
        *ptr = (uint8)(data&0x80);
    }
#endif
    MCF_PIT1_PCSR &= ~MCF_PIT_PCSR_EN;
    MCF_PIT1_PCSR |= MCF_PIT_PCSR_PIF;  //清中断标志
    bool = 0;

}
__declspec(interrupt:0)  void TPIT1_interrupt(void){

    vuint8 *ptr;
    int abc[50][11];

    for(u = 0; u<IMG_H; u++){
        ptr = &ImageC[u][1];
        for(v = 1;v < IMG_W_8; v++){
            data = Image[Image_Row[u]][v];
            //Image_Ptr[Image_Row[u] * 20 + v - 1] = (uint8)data;
            Image_Ptr[u * 40 + v - 1] = (uint8)data;
            *ptr = (uint8)(data&0x80);
            data=(uint8)(data<<1);
            ptr++;
            *ptr = (uint8)(data&0x80);
            data=(uint8)(data<<1);
            ptr++;
            *ptr = (uint8)(data&0x80);
            data=(uint8)(data<<1);
            ptr++;
            *ptr = (uint8)(data&0x80);
            data=(uint8)(data<<1);
            ptr++;
            *ptr = (uint8)(data&0x80);
            data=(uint8)(data<<1);
            ptr++;
            *ptr = (uint8)(data&0x80);
            data=(uint8)(data<<1);
            ptr++;
            *ptr = (uint8)(data&0x80);
            data=(uint8)(data<<1);
            ptr++;
            *ptr = (uint8)(data&0x80);
            ptr++;
        }
        data = Image[Image_Row[u]][0];
        //Image_Ptr[Image_Row[u] * 20 + v - 1] = (uint8)data;
        Image_Ptr[u * 40 + v - 1] = (uint8)data;
        *ptr = (uint8)(data&0x80);
        data=(uint8)(data<<1);
        ptr++;
        *ptr = (uint8)(data&0x80);
        data=(uint8)(data<<1);
        ptr++;
        *ptr = (uint8)(data&0x80);
        data=(uint8)(data<<1);
        ptr++;
        *ptr = (uint8)(data&0x80);
        data=(uint8)(data<<1);
        ptr++;
        *ptr = (uint8)(data&0x80);
        data=(uint8)(data<<1);
        ptr++;
        *ptr = (uint8)(data&0x80);
        data=(uint8)(data<<1);
        ptr++;
        *ptr = (uint8)(data&0x80);
        data=(uint8)(data<<1);
        ptr++;
        *ptr = (uint8)(data&0x80);
    }
    for(v = 0;v < IMG_W_8; v++){
        Image_Ptr[100 * 20 + v - 1] = 0xff;
        Image_Ptr[101 * 20 + v - 1] = 0xff;
        Image_Ptr[102 * 20 + v - 1] = 0xff;
        Image_Ptr[103 * 20 + v - 1] = 0xff;
        Image_Ptr[104 * 20 + v - 1] = 0xff;
    }

#if 1

    //img_EdgeInfo[CAMERA_H][5] = {0};//图像边界信息
    //[0]：左边界 [1]：右边界 [2]：中线 [3]：获取到的边界信息 [4]：斜率\曲率

    colEdgeEnable = 0;  //横向扫描使能位禁止

    leftEdgeMissCnt = 0;    //左边界开始miss个数计数清零
    rightEdgeMissCnt = 0;	//右边界开始miss个数计数清零
    edgeBothMissCnt = 0;    //左右边界都miss计数

    leftEdgeStart = IMG_W_HALF;  	//左边线检测的起始位置
    leftEdgeEnd = 0;      		    //左边线检测的结束位置
    rightEdgeStart = IMG_W_HALF+1;  //右边线检测的起始位置
    rightEdgeEnd = IMG_W - 1;	    //右边线检测的结束位置

    leftEdgeOffset = 0;
    rightEdgeOffset = 0;

    centerLine = IMG_W_HALF+1;      //最后中线位置

    leftEdgeFind = 0;   //左边线找到标志量清零
    rightEdgeFind = 0;  //右边线找到标志量清零

    leftLastLine[0] = 0;
    leftLastLine[1] = 0;
    rightLastLine[0] = 0;
    rightLastLine[1] = 0;
    lastLine[0] = 0;
    lastLine[1] = 0;

    centerLineSub = 0;
    edgeMissBool = 0;

    for(row = IMG_H-1;row>=0;row --){
        debugCnt = 1;

        img_EdgeInfo[row][0] = 0;
        img_EdgeInfo[row][1] = 0;
        img_EdgeInfo[row][2] = 0;
        img_EdgeInfo[row][3] = 0;
        img_EdgeInfo[row][4] = 0;
        world_EdgeInfo[row] = 0;


        //edgeOffset = (uint8)(colCorrectTable[row]*1.8+1);
        edgeOffset = 13;

        //Image_Ptr[Image_Row[row] * 20 + leftEdgeStart/8] |= 1<<(7-leftEdgeStart&0x07);
        //Image_Ptr[Image_Row[row] * 20 + rightEdgeStart/8] |= 1<<(7-rightEdgeStart&0x07);
        //Image_Ptr[Image_Row[row] * 20 + leftEdgeEnd/8] &= ~(1<<(7-leftEdgeEnd&0x07));
        //Image_Ptr[Image_Row[row] * 20 + rightEdgeEnd/8] &= ~(1<<(7-rightEdgeEnd&0x07));
        //Image_Ptr[Image_Row[row] * 20 + centerLine/8] |= 1<<(7-centerLine&0x07);

        if(ImageC[row][centerLine] == 0x80){    //若中间线为黑线，则表示检测到结尾
            if((row>48||(row>5&&img_EdgeInfo[row+1][3] == 1))&&ImageC[(uint8)(row*0.875)][centerLine] != 0x80){
                u = (int)(row / 12);
                while(u>=0){
                    img_EdgeInfo[row-u][3] = 6;               //当前行类型置为6
                    if(row < 49)
                        img_EdgeInfo[row-u][2] = img_EdgeInfo[row+1][2];
                    else
                        img_EdgeInfo[row-u][2] = 80;
                    u--;
                }
                row -= 4;
                continue;
            }else{
                img_EdgeInfo[row][3] = 5;           //当前行类型置为5并结束循环
                break;
            }
        }
        if(ImageC[row][leftEdgeStart]&0x80){//左边线开始时丢失，暂时未解决
            leftEdgeStart = centerLine;
        }
        for(col = leftEdgeStart;col > leftEdgeEnd;col--){     //左边线寻找
            if((ImageC[row][col]&0x80) && ImageC[row][col+1] == 0 ){
                if(leftEdgeMissCnt&&leftEdgeMissCnt>= 49-row){
                    leftEdgeMissCnt -- ;
                    break;
                }
                img_EdgeInfo[row][0] = col;                         //获取图像边界信息
                //world_EdgeInfo[row][0] = correctTable[row][col];    //获取世界边界信息
                leftEdgeFind = 1;   //左边线标志量置位

                /*左边偏差值计算*/
                edgetempa = img_EdgeInfo[row+1][0];
                edgetempb = col - edgetempa;
                if(edgetempa&&edgetempb){
                    if(row<49){
                        if(img_EdgeInfo[row+1][0] == col){
                            leftEdgeOffset = 0;
                            break;
                        }
                        leftEdgeOffset = (0.1*leftEdgeOffset + 0.9*edgetempb)*3.5;
                    }else{
                        leftEdgeOffset = 0;
                    }
                }
                //更新最后的左边线的行列
                if(ImageC[row][centerLine] != 0x80){
                    leftLastLine[0] = (uint8)row;
                    leftLastLine[1] = (uint8)img_EdgeInfo[row][0];
                }
                break;

            }
        }

        if(ImageC[row][rightEdgeStart]&0x80){//右边线开始时丢失，暂时未解决
            rightEdgeStart = centerLine;
        }
        for(col = rightEdgeStart;col < rightEdgeEnd;col++){   //右边线寻找
            if((ImageC[row][col]&0x80) && ImageC[row][col-1] == 0 ){
                if(rightEdgeMissCnt&&rightEdgeMissCnt>= 49-row){
                    rightEdgeMissCnt -- ;
                    break;
                }
                img_EdgeInfo[row][1] = col;                         //获取图像边界信息
                //world_EdgeInfo[row][1] = correctTable[row][col];    //获取世界边界信息
                rightEdgeFind = 1;  //右边线标志量置位

                /*左边偏差值计算*/
                edgetempa = img_EdgeInfo[row+1][1];
                edgetempb = col - edgetempa;
                if(edgetempa&&edgetempb){
                    if(row<49){
                        if(img_EdgeInfo[row+1][1] == col){
                            rightEdgeOffset = 0;
                            break;
                        }
                        rightEdgeOffset = (0.1*rightEdgeOffset + 0.9*edgetempb)*3.5;
                    }else{
                        rightEdgeOffset = 0;
                    }
                }
                //更新最后的右边线的行列
                if(ImageC[row][centerLine] != 0x80){
                    rightLastLine[0] = (uint8)row;
                    rightLastLine[1] = (uint8)img_EdgeInfo[row][1];
                }
                break;

            }
        }

        abc[row][0] = leftEdgeEnd;
        abc[row][1] = img_EdgeInfo[row][0];
        abc[row][2] = leftEdgeStart;

        abc[row][3] = rightEdgeStart;
        abc[row][4] = img_EdgeInfo[row][1];
        abc[row][5] = rightEdgeEnd;

        abc[row][6] = centerLine;

        abc[row][7] = 1111;
        if(leftEdgeOffset<0)
            leftEdgeOffset = -leftEdgeOffset;
        if(rightEdgeOffset>0)
            rightEdgeOffset = -rightEdgeOffset;
        abc[row][8] = leftEdgeOffset;
        abc[row][9] = rightEdgeOffset;

        if(leftEdgeFind&rightEdgeFind){
            /*1 设置中线类型*/
            img_EdgeInfo[row][3] = 1;   //中线类型设为 左中右都可用

            /*2 预测并设置下一行边线的范围*/
            leftEdgeStart =  img_EdgeInfo[row][0] + leftEdgeOffset;     //改变下一行左边线搜寻开始位置
            leftEdgeEnd   =  img_EdgeInfo[row][0] - leftEdgeOffset;     //改变下一行左边线搜寻结束位置
            rightEdgeStart = img_EdgeInfo[row][1] + rightEdgeOffset;    //改变下一行右边线搜寻开始位置
            rightEdgeEnd  =  img_EdgeInfo[row][1] - rightEdgeOffset;    //改变下一行右边线搜寻结束位置

            /*3 求出中线的位置*/
        	img_EdgeInfo[row][2]    = (img_EdgeInfo[row][0] + img_EdgeInfo[row][1])*0.5;     //求取图像坐标中线
        	world_EdgeInfo[row]  = correctTable[row][img_EdgeInfo[row][2]]; //求取世界坐标中线

            /*4 保存最后一次中线的行列*/
            lastLine[0] = (uint8)row;
            lastLine[1] = (uint8)img_EdgeInfo[row][2];

            /*5 计算中线的偏差值*/
            if(row<49&&img_EdgeInfo[row+1][2]){
            	centerLineSub = centerLineSub*0.1 + (img_EdgeInfo[row][2]-img_EdgeInfo[row+1][2])*0.9;
            }else{
                centerLineSub = 0;
            }

            /*6 设置预测的中线值*/
            centerLine = img_EdgeInfo[row][2] + centerLineSub;


        }else if(leftEdgeFind){
            /*1 设置中线类型*/
            img_EdgeInfo[row][3] = 2;   //边线类型设为 左可用

            /*2 预测并设置下一行边线的范围*/
            if(edgeMissBool){
                leftEdgeStart = centerLine;             //重置下一行左边线搜寻开始位置
                rightEdgeStart = centerLine + 1;        //重置下一行右边线搜寻开始位置
                if(leftEdgeOffset){
                    leftEdgeEnd = leftEdgeEnd + leftEdgeOffset*0.2;
                }else{
                    leftEdgeEnd = leftEdgeEnd + 2;
                }
                if(rightEdgeOffset){
                    rightEdgeEnd = rightEdgeEnd - rightEdgeOffset*0.2;
                }else{
                    rightEdgeEnd = rightEdgeEnd - 2;
                }
                if(leftEdgeStart-leftEdgeEnd<20)
                    leftEdgeEnd = leftEdgeStart - 20;
                if(rightEdgeEnd - rightEdgeStart < 20)
                    rightEdgeEnd = rightEdgeStart + 20;
            }else if(leftEdgeOffset == 0){
                rightEdgeStart = centerLine;
                rightEdgeEnd = rightEdgeEnd + rightEdgeOffset*0.2;              //预测位置
            }else{
                leftEdgeStart =  img_EdgeInfo[row][0] + leftEdgeOffset;     //改变下一行左边线搜寻开始位置
                leftEdgeEnd   =  img_EdgeInfo[row][0] - leftEdgeOffset;     //改变下一行左边线搜寻结束位置
                rightEdgeStart = centerLine;
                rightEdgeEnd = rightEdgeEnd + rightEdgeOffset*0.2;              //预测位置

                if(leftEdgeStart-leftEdgeEnd<20)
                    leftEdgeEnd = leftEdgeStart - 15;
            }
            /*3 求出中线的估计比率*/
            if(row>=25){
                img_EdgeRowRatio = (50.0 - (row-25))/50.0;       //行比率计算
            }else{
                img_EdgeRowRatio = (50.0 - (25-row))/50.0;       //行比率计算
            }
            /*3 求出中线的估计值*/
    	    img_EdgeTemp = (int16)(img_EdgeInfo[row][0] + colCorrectTable[row]*19*img_EdgeRowRatio);   //获取世界边界信息根据真实世界赛道宽度获取中线
            /*3 求出中线的位置*/
            img_EdgeInfo[row][2] = img_EdgeTemp;    //最后将中心线保存
            world_EdgeInfo[row] = correctTable[row][img_EdgeTemp];       //获取世界边界信息

            /*4 设置预测的中线值*/
            centerLine = img_EdgeInfo[row][2] + centerLineSub;
            //centerLine += centerLineSub;

            /*5 边线miss个数计数*/
            rightEdgeMissCnt++; //右边线miss个数增加


        }else if(rightEdgeFind){
            /*1 设置中线类型*/
            img_EdgeInfo[row][3] = 3;   //中线类型设为 右可用

            /*2 预测并设置下一行边线的范围*/
            if(edgeMissBool){
                leftEdgeStart = centerLine;             //重置下一行左边线搜寻开始位置
                rightEdgeStart = centerLine + 1;        //重置下一行右边线搜寻开始位置
                if(leftEdgeOffset){
                    leftEdgeEnd = leftEdgeEnd + leftEdgeOffset*0.2;
                }else{
                    leftEdgeEnd = leftEdgeEnd + 2;
                }
                if(rightEdgeOffset){
                    rightEdgeEnd = rightEdgeEnd - rightEdgeOffset*0.2;
                }else{
                    rightEdgeEnd = rightEdgeEnd - 2;
                }
                if(leftEdgeStart-leftEdgeEnd<20)
                    leftEdgeEnd = leftEdgeStart - 20;
                if(rightEdgeEnd - rightEdgeStart < 20)
                    rightEdgeEnd = rightEdgeStart + 20;
            }else if(rightEdgeOffset==0){
                leftEdgeStart = centerLine;
                leftEdgeEnd = leftEdgeEnd - leftEdgeOffset*0.2;
            }else{
                rightEdgeStart = img_EdgeInfo[row][1] + rightEdgeOffset;     //改变下一行右边线搜寻开始位置
                rightEdgeEnd =   img_EdgeInfo[row][1] - rightEdgeOffset;     //改变下一行右边线搜寻结束位置
                leftEdgeStart = centerLine;
                leftEdgeEnd = leftEdgeEnd - leftEdgeOffset*0.2;

                if(rightEdgeEnd - rightEdgeStart < 20)
                    rightEdgeEnd = rightEdgeStart + 15;
            }
            /*3 求出中线的估计比率*/
            if(row>=25){
                img_EdgeRowRatio = (50.0 - (row-25))/50.0;       //行比率计算
            }else{
                img_EdgeRowRatio = (50.0 - (25-row))/50.0;       //行比率计算
            }
            /*3 求出中线的估计值*/
    	    img_EdgeTemp = (int16)(img_EdgeInfo[row][1] - colCorrectTable[row]*19*img_EdgeRowRatio);   //获取世界边界信息根据真实世界赛道宽度获取中线
            /*3 求出中线的位置*/
            img_EdgeInfo[row][2] = img_EdgeTemp;
            world_EdgeInfo[row] = correctTable[row][img_EdgeTemp];       //获取世界边界信息

            /*4 设置预测的中线值*/
            centerLine = img_EdgeInfo[row][2] + centerLineSub;
            //centerLine += centerLineSub;

            /*5 边线miss个数计数*/
            leftEdgeMissCnt++;  //左边线miss个数增加


        }else{
            /*1 设置中线类型*/
            img_EdgeInfo[row][3] = 4;   //中线类型设为 三线丢失
            img_EdgeInfo[row][2] = centerLine;

            /*2 预测并设置下一行边线的范围*/
            leftEdgeStart = centerLine;             //重置下一行左边线搜寻开始位置
            leftEdgeEnd = leftEdgeEnd - leftEdgeOffset*0.2;
            rightEdgeStart = centerLine + 1;        //重置下一行右边线搜寻开始位置
            rightEdgeEnd = rightEdgeEnd - rightEdgeOffset*0.2;
            if(edgeMissBool){
                leftEdgeEnd = leftEdgeEnd + 2;
                rightEdgeEnd = rightEdgeEnd - 2;
            }
            if(leftEdgeStart-leftEdgeEnd<20)
                leftEdgeEnd = leftEdgeStart - 20;
            if(rightEdgeEnd - rightEdgeStart < 20)
                rightEdgeEnd = rightEdgeStart + 20;

            /*
            leftEdgeStart = centerLine;     //重置下一行左边线搜寻开始位置
            //leftEdgeEnd = (int16)(centerLine-colCorrectTable[row]*16);
            rightEdgeStart = centerLine + 1;  //重置下一行右边线搜寻开始位置
            //rightEdgeEnd = (int16)(centerLine+colCorrectTable[row]*16);
            /*
            leftEdgeStart = (int16)(centerLine + edgeOffset);
            leftEdgeEnd = (int16)(centerLine - edgeOffset);
            rightEdgeStart = (int16)(centerLine - edgeOffset);
            rightEdgeEnd = (int16)(centerLine + edgeOffset);
            */

            /*4 设置预测的中线值*/
            centerLine = centerLine + centerLineSub;
            //centerLine += centerLineSub;

            /*5 边线miss个数计数*/
            edgeBothMissCnt++;//两边都miss计数增加
            if(edgeBothMissCnt>8)
                edgeMissBool = 1;

        }
        if(centerLine>160){
            centerLine = 159;
        }else if(centerLine<0){
            centerLine = 1;
        }
        /*校正左右边线开始结束位置变量，防止超出数组边界*/
        if(rightEdgeStart<leftEdgeStart){
            rightEdgeStart = (rightEdgeStart+leftEdgeStart)*0.5;
            leftEdgeStart = rightEdgeStart;
        }

        if(leftEdgeStart>=CAMERA_W){
            leftEdgeStart = 159;
        }else if(leftEdgeEnd < 0){
            leftEdgeEnd = 0;
        }

        if(rightEdgeStart < 0){
            rightEdgeStart = 0;
        }else if(rightEdgeEnd >= CAMERA_W){
            rightEdgeEnd = CAMERA_W - 1;
        }


        leftEdgeFind = 0;   //左标志位清零
        rightEdgeFind = 0;  //右标志位清零

    }
    lastRow = (uint8)(row+1);
    img_EdgeInfo[0][3] = 5; //最后一行不管有没有中线都设为5
    debugCnt = 2;

#endif
#if 1
    img_BuxianCnt = 0;
    img_BuxianBool = 0;
    img_BuxianRow = -1;
    img_BuxianEndRow = 0;

    imgEdgeFill4Cnt = 0;                    //补线计数
    imgEdgeFill4[imgEdgeFill4Cnt][0] = 0;   //补线起点行数
    imgEdgeFill4[imgEdgeFill4Cnt][2] = 0;   //补线终点行数

    bothLastLine[0] = 0;
    bothLastLine[1] = 0;

    if(leftLastLine[0]<rightLastLine[0]){
        lastLine[0] = leftLastLine[0];
        lastLine[1] = rightLastLine[0];
    }else{
        lastLine[1] = leftLastLine[0];
        lastLine[0] = rightLastLine[0];
    }

    if(img_EdgeInfo[49][3] == 4){
        img_BuxianRow = 48;
        imgEdgeFill4[0][0] = 49;
        imgEdgeFill4[0][1] = 80;
    }

    for(row = IMG_H-1;row>=0;row --){       //补线预处理
        switch(img_BuxianBool){
            case 0:{
                if(img_BuxianRow == -1 && img_EdgeInfo[row][3] == 3 || img_EdgeInfo[row][3] == 2){
                    img_BuxianRow = row;
                    continue;
                }else if(img_EdgeInfo[row][3] == 1){
                    img_BuxianRow = -1;
                    continue;
                }
                if(img_BuxianRow!=-1&&img_EdgeInfo[row][3] == 4){
                    img_BuxianCnt++;
                    if(img_BuxianCnt > 3){
                        img_BuxianBool = 1;
                        img_BuxianCnt = 0;
                        row = (int16)img_BuxianRow;

                        if(!imgEdgeFill4[imgEdgeFill4Cnt][0]){
                            imgEdgeFill4[imgEdgeFill4Cnt][0] = img_BuxianRow;
                            imgEdgeFill4[imgEdgeFill4Cnt][1] = img_EdgeInfo[img_BuxianRow][2];
                        }
                    }
                }
            }break;
            case 1:{
                if(img_EdgeInfo[row][3] == 1){

                    imgEdgeFill4[imgEdgeFill4Cnt][2] = row;
                    imgEdgeFill4[imgEdgeFill4Cnt][3] = img_EdgeInfo[row][2];

                    imgEdgeFill4Cnt++;

                    imgEdgeFill4[imgEdgeFill4Cnt][0] = 0;
                    imgEdgeFill4[imgEdgeFill4Cnt][2] = 0;

                    img_BuxianBool = 0;
                    img_BuxianRow = -1;
                    img_BuxianEndRow = 0;
                }else if(img_BuxianEndRow==0&&img_EdgeInfo[row][3] == 2 || img_EdgeInfo[row][3] == 3){
                    img_BuxianEndRow = row;
                    img_EdgeInfo[row][3] = 4;
                }else if(img_EdgeInfo[row][3] == 5 ){

                    imgEdgeFill4[imgEdgeFill4Cnt][2] = (int16)(row - 1);
                    if(row>25)
                        imgEdgeFill4[imgEdgeFill4Cnt][3] = img_EdgeInfo[row+1][2]+(50.0 - (row-25))/50.0*colCorrectTable[row]*20;
                    else
                        imgEdgeFill4[imgEdgeFill4Cnt][3] = img_EdgeInfo[row+1][2]+(50.0 - (25-row))/50.0*colCorrectTable[row]*20;


                    imgEdgeFill4Cnt++;
                }else if(img_EdgeInfo[row][3] == 4 ){
                    img_BuxianEndRow = 0;
                }else{
                    img_EdgeInfo[row][3] = 4;
                }
            }break;
        }
        if(img_EdgeInfo[row][3] == 5)
            break;
    }

    if(img_EdgeInfo[lastRow][2]>5 && img_EdgeInfo[lastRow][2]<155){

        for(row = IMG_H-1;row>0;row--){     //补线分析和横向扫描初始化
            debugCnt = 3;
            if(img_EdgeInfo[row][3] == 5&&img_EdgeInfo[row+2][3] != 1&&row>5){

                 if(rightLastLine[0]==0){

                    /*3 求出中线的估计比率*/
                    if(row>=25){
                        img_EdgeRowRatio = (50.0 - (row-25))/50.0;       //行比率计算
                    }else{
                        img_EdgeRowRatio = (50.0 - (25-row))/50.0;       //行比率计算
                    }

        	        v = (img_EdgeInfo[row+1][0] + colCorrectTable[row+1]*19*img_EdgeRowRatio*2);   //获取世界边界信息根据
                    //v = leftLastLine[1] + colCorrectTable[leftLastLine[0]]*20;
                    u = (50+row)*0.5;

                    img_EdgeInfo[u][3] = 7;

                    topEdgeStart = u;
                    topEdgeEnd = (uint8)(row -5);
                    direct = 1;
                    colEnd = IMG_W-1;
                    colEdgeEnable = 2;

                }else if(leftLastLine[0]==0){


                    if(row>=25){
                        img_EdgeRowRatio = (50.0 - (row-25))/50.0;       //行比率计算
                    }else{
                        img_EdgeRowRatio = (50.0 - (25-row))/50.0;       //行比率计算
                    }

                    v = (img_EdgeInfo[row+1][1] - colCorrectTable[row+1]*19*img_EdgeRowRatio*2);   //获取世界边界信息根据
                    u = (50+row)*0.5;

                    img_EdgeInfo[u][3] = 7;

                    topEdgeStart = u;
                    topEdgeEnd = (uint8)(row - 5);
                    direct = -1;
                    colEnd = 0;
                    colEdgeEnable = 2;

                }else if(leftLastLine[0] > rightLastLine[0]){
                    top_bottomOffset = (uint16)((leftLastLine[0] -  rightLastLine[0])*0.7);
                    v = leftLastLine[1];
                    u = (leftLastLine[0]+rightLastLine[0])*0.5;

                    img_EdgeInfo[u][3] = 7;

                    topEdgeStart = u;
                    bottomEdgeStart = u;
                    topEdgeEnd = (uint8)(rightLastLine[0] - 3);
                    bottomEdgeEnd = (uint8)(leftLastLine[0] + 3);

                    direct = -1;
                    colEnd = 0;
                    colEdgeEnable = 1;
                }else if(leftLastLine[0] < rightLastLine[0]){
                    //top_bottomOffset = (uint16)((rightLastLine[0] -  leftLastLine[0])*0.7);
                    v = rightLastLine[1];
                    u = (leftLastLine[0]+rightLastLine[0])*0.5;

                    img_EdgeInfo[u][3] = 7;

                    topEdgeStart = u;
                    bottomEdgeStart = u;
                    topEdgeEnd = (uint8)(leftLastLine[0] - 3);
                    bottomEdgeEnd = (uint8)(rightLastLine[0] + 3);

                    direct = 1;
                    colEnd = IMG_W-1;
                    colEdgeEnable = 1;
                }
                if(v<0)
                    v = 1;
                if(v>158)
                    v = 158;
                if(u >= IMG_H)
                    u = IMG_H-1;
                break;
            }
            if(img_EdgeInfo[row][3] == 5){
                break;
            }

        }
    }
        debugCnt = 4;
#endif

#if 1
    while(imgEdgeFill4Cnt--){
        debugCnt = 5;

        temp = imgEdgeFill4[imgEdgeFill4Cnt][1];
        imgEdgeFill4Row = (int16)(imgEdgeFill4[imgEdgeFill4Cnt][0] - imgEdgeFill4[imgEdgeFill4Cnt][2]);//求出行数差值
        imgEdgeFill4Col = (int16)(imgEdgeFill4[imgEdgeFill4Cnt][1] - imgEdgeFill4[imgEdgeFill4Cnt][3]);//求出行数差值

        for(row = imgEdgeFill4[imgEdgeFill4Cnt][0] ;row > imgEdgeFill4[imgEdgeFill4Cnt][2] ;row--){
        	img_EdgeInfo[row][2] = (uint16)(temp + ((float)(row-imgEdgeFill4[imgEdgeFill4Cnt][0]))/imgEdgeFill4Row * imgEdgeFill4Col);
            world_EdgeInfo[row] = correctTable[row][img_EdgeInfo[row][2]];//获取世界边界信息
            //Image_Ptr[Image_Row[row] * 20 + (uint8)(img_EdgeInfo[row][2]/8)] |= 1<<(7-img_EdgeInfo[row][2]&0x07);
        }
    }
    debugCnt = 6;
#endif
#if 1

    debugCnt = 8;

    if(colEdgeEnable==1){

        colEdgeCnt = 0;
        colCenterLine = (uint8)u;
        colBool = 0;

        for(col = v;col!=colEnd;col+=direct){

            img_ColEdgeInfo[colEdgeCnt][0] = 0;
            img_ColEdgeInfo[colEdgeCnt][1] = 0;
            img_ColEdgeInfo[colEdgeCnt][2] = 0;
            img_ColEdgeInfo[colEdgeCnt][3] = 0;
            img_ColEdgeInfo[colEdgeCnt][4] = 0;
            img_ColEdgeInfo[colEdgeCnt][5] = col;
            world_ColEdgeInfo[colEdgeCnt] = 0;

            if(ImageC[colCenterLine][col] == 0x80){
                img_ColEdgeInfo[colEdgeCnt][3] = 5;
                break;
            }

        	//Image_Ptr[Image_Row[topEdgeStart] * 20 + col/8] |= 1<<(7-col&0x07);
        	//Image_Ptr[Image_Row[bottomEdgeStart] * 20 + col/8] |= 1<<(7-col&0x07);
        	//Image_Ptr[Image_Row[topEdgeEnd] * 20 + col/8] |= 1<<(7-col&0x07);
        	//Image_Ptr[Image_Row[bottomEdgeEnd] * 20 + col/8] |= 1<<(7-col&0x07);

            for(row = topEdgeStart;row>topEdgeEnd;row--){
                if((ImageC[row][col]&0x80) && ImageC[row+1][col] == 0 ){
                    img_ColEdgeInfo[colEdgeCnt][0] = row;         //获取图像边界信息
                    //world_ColEdgeInfo[colEdgeCnt][0] = correctTable[row][col];//获取世界边界信息

                    topEdgeFind = 1;
                    break;

                }
            }
            if(row == 0){
                img_ColEdgeInfo[colEdgeCnt][0] = 0;         //获取图像边界信息
                topEdgeFind = 1;
            }

            for(row = bottomEdgeStart;row<bottomEdgeEnd;row++){
                if((ImageC[row][col]&0x80) && ImageC[row-1][col] == 0 ){
                    img_ColEdgeInfo[colEdgeCnt][1] = row;         //获取图像边界信息
                    //world_ColEdgeInfo[colEdgeCnt][1] = correctTable[row][col];//获取世界边界信息

                    bottomEdgeFind = 1;
                    break;

                }
            }
            if(row == 0){
                img_ColEdgeInfo[colEdgeCnt][1] = 0;         //获取图像边界信息
                bottomEdgeFind = 1;
            }

            if(topEdgeFind&&bottomEdgeFind){

                img_ColEdgeInfo[colEdgeCnt][2] = (uint8)((img_ColEdgeInfo[colEdgeCnt][0]+img_ColEdgeInfo[colEdgeCnt][1])*0.5);
                world_ColEdgeInfo[colEdgeCnt] = correctTable[img_ColEdgeInfo[colEdgeCnt][2]][col];//获取世界边界信息

        	    //Image_Ptr[Image_Row[img_ColEdgeInfo[colEdgeCnt][2]] * 20 + (uint8)(img_ColEdgeInfo[colEdgeCnt][5]/8)] |= 1<<(7-img_ColEdgeInfo[colEdgeCnt][5]&0x07);
                colCenterLine = (uint8)img_ColEdgeInfo[colEdgeCnt][2];

                if(colBool == 0){
                    firstRow = (int16)img_ColEdgeInfo[colEdgeCnt][2];
                    firstCol = (int16)col;
                    colBool = 1;
                }

                topEdgeStart =   (uint8)(img_ColEdgeInfo[colEdgeCnt][0] + 5);
                topEdgeEnd   =   (uint8)(img_ColEdgeInfo[colEdgeCnt][0] - 5);
                bottomEdgeStart =  (uint8)(img_ColEdgeInfo[colEdgeCnt][1] - 5);
                bottomEdgeEnd  =   (uint8)(img_ColEdgeInfo[colEdgeCnt][1] + 5);

                img_ColEdgeInfo[colEdgeCnt][3] = 1;

                colEdgeCnt++;
            }else if(topEdgeFind){
                topEdgeStart =   (uint8)(img_ColEdgeInfo[colEdgeCnt][0] + 5);
                topEdgeEnd   =   (uint8)(img_ColEdgeInfo[colEdgeCnt][0] - 5);
                bottomEdgeStart = topEdgeStart;

                img_ColEdgeInfo[colEdgeCnt][3] = 2;

                colEdgeCnt++;
            }else if(bottomEdgeFind){
                bottomEdgeStart =  (uint8)(img_ColEdgeInfo[colEdgeCnt][1] - 5);
                bottomEdgeEnd  =   (uint8)(img_ColEdgeInfo[colEdgeCnt][1] + 5);
                topEdgeStart = bottomEdgeStart;

                img_ColEdgeInfo[colEdgeCnt][3] = 3;

                colEdgeCnt++;
            }else{
                img_ColEdgeInfo[colEdgeCnt][3] = 4;
                colEdgeCnt++;
            }

            if(topEdgeStart>=50){
                topEdgeStart = 45;
            }
            if(bottomEdgeStart < 0){
                bottomEdgeStart = 0;
            }
            if(topEdgeEnd < 0){
                topEdgeEnd = 0;
            }
            if(bottomEdgeEnd >= 50){
                bottomEdgeEnd = 49;
            }

            topEdgeFind = 0;
            bottomEdgeFind = 0;

        }
        if(img_ColEdgeInfo[0][2] == 0 || img_ColEdgeInfo[0][3] != 1 || img_ColEdgeInfo[1][2] == 0){
            colEdgeEnable = 0;
            img_ColEdgeInfo[0][3] = 5;
        }else{
            //sprintf(TXBuffer,"[%u]",colEdgeEnable);
            //TUart0_Puts(TXBuffer);
            img_ColEdgeInfo[colEdgeCnt][3] = 5;
            //Image_Ptr[Image_Row[(int)img_ColEdgeInfo[0][2]] * 20 + (uint8)(img_ColEdgeInfo[0][5]/8)] |= 1<<(7-img_ColEdgeInfo[0][5]&0x07);
            Image_Ptr[(int)img_ColEdgeInfo[0][2] * 40 + (uint8)(img_ColEdgeInfo[0][5]/8)] |= 1<<(7-img_ColEdgeInfo[0][5]&0x07);
            for(u = 1;u<colEdgeCnt-1;u++){     //中线滤波处理循环
                if(img_ColEdgeInfo[u][2]==0){
                    img_ColEdgeInfo[u][2] = img_ColEdgeInfo[u-1][2];
                    world_ColEdgeInfo[u] = correctTable[img_ColEdgeInfo[u][2]][img_ColEdgeInfo[u][5]];
                }
                //Image_Ptr[Image_Row[img_ColEdgeInfo[u]] * 20 + (uint8)(img_ColEdgeInfo[u][5]/8)] |= 1<<(7-img_ColEdgeInfo[u][5]&0x07);
                //Image_Ptr[Image_Row[img_ColEdgeInfo[u][2]] * 20 + ((int)world_ColEdgeInfo[u]/8)] |= 1<<(7-(int)world_ColEdgeInfo[u]&0x07);
                Image_Ptr[img_ColEdgeInfo[u][2] * 40 + ((int)world_ColEdgeInfo[u]/8)] |= 1<<(7-(int)world_ColEdgeInfo[u]&0x07);
            }

            if(colBool){

                if(direct == 1){
                    tempa = rightLastLine[0] - firstRow;
                    tempb = firstCol - img_EdgeInfo[rightLastLine[0]][2];
                    for(row = rightLastLine[0];row >= firstRow;row--){
                    	img_EdgeInfo[row][2] = (uint16)(img_EdgeInfo[rightLastLine[0]][2] - ((float)(row - rightLastLine[0]))/(tempa+1) * tempb);
                        world_EdgeInfo[row] = correctTable[row][img_EdgeInfo[row][2]];//获取世界边界信息

                    }
                }else{
                    tempa = leftLastLine[0] - firstRow;
                    tempb = img_EdgeInfo[leftLastLine[0]][2] - firstCol;
                    for(row = leftLastLine[0];row>=firstRow;row--){
                    	img_EdgeInfo[row][2] = (uint16)(img_EdgeInfo[leftLastLine[0]][2] + ((float)(row - leftLastLine[0]))/(tempa+1) * tempb);
                        world_EdgeInfo[row] = correctTable[row][img_EdgeInfo[row][2]];//获取世界边界信息

                    }
                }
            }
            img_ColEdgeInfo[colEdgeCnt][3] = 5;
        }

    }else if(colEdgeEnable == 2){
        colEdgeCnt = 0;
        colCenterLine = (uint8)u;
        colBool = 0;

        for(col = v;col!=colEnd;col+=direct){

            img_ColEdgeInfo[colEdgeCnt][0] = 0;
            img_ColEdgeInfo[colEdgeCnt][1] = 0;
            img_ColEdgeInfo[colEdgeCnt][2] = 0;
            img_ColEdgeInfo[colEdgeCnt][3] = 0;
            img_ColEdgeInfo[colEdgeCnt][4] = 0;
            img_ColEdgeInfo[colEdgeCnt][5] = col;
            world_ColEdgeInfo[colEdgeCnt] = 0;
            //world_ColEdgeInfo[colEdgeCnt][1] = 0;
            //world_ColEdgeInfo[colEdgeCnt][2] = 0;
            //world_ColEdgeInfo[colEdgeCnt][3] = 0;
            //world_ColEdgeInfo[colEdgeCnt][4] = 0;
            //world_ColEdgeInfo[colEdgeCnt][5] = col;

            if(ImageC[colCenterLine][col] == 0x80){
                img_ColEdgeInfo[colEdgeCnt][3] = 5;
                break;
            }

        	//Image_Ptr[Image_Row[topEdgeStart] * 20 + col/8] |= 1<<(7-col&0x07);
        	//Image_Ptr[Image_Row[bottomEdgeStart] * 20 + col/8] |= 1<<(7-col&0x07);
        	//Image_Ptr[Image_Row[topEdgeEnd] * 20 + col/8] |= 1<<(7-col&0x07);
        	//Image_Ptr[Image_Row[bottomEdgeEnd] * 20 + col/8] |= 1<<(7-col&0x07);

            for(row = topEdgeStart;row>topEdgeEnd;row--){
                if((ImageC[row][col]&0x80) && ImageC[row+1][col] == 0 ){
                    img_ColEdgeInfo[colEdgeCnt][0] = row;         //获取图像边界信息
                    //world_ColEdgeInfo[colEdgeCnt][0] = correctTable[row][col];//获取世界边界信息

                    topEdgeFind = 1;
                    break;

                }
            }

            if(topEdgeFind){

                img_ColEdgeInfo[colEdgeCnt][2] = (uint8)((img_ColEdgeInfo[colEdgeCnt][0]+50)*0.5);
                world_ColEdgeInfo[colEdgeCnt] = correctTable[img_ColEdgeInfo[colEdgeCnt][2]][img_ColEdgeInfo[colEdgeCnt][5]];//获取世界边界信息

        	    //Image_Ptr[Image_Row[(int)world_ColEdgeInfo[colEdgeCnt]] * 20 + (uint8)(img_ColEdgeInfo[colEdgeCnt][5]/8)] |= 1<<(7-img_ColEdgeInfo[colEdgeCnt][5]&0x07);
                colCenterLine = (uint8)img_ColEdgeInfo[colEdgeCnt][2];

                if(colBool == 0){
                    colBool = 1;
                }

                topEdgeStart =   (uint8)(img_ColEdgeInfo[colEdgeCnt][0] + 5);
                topEdgeEnd   =   (uint8)(img_ColEdgeInfo[colEdgeCnt][0] - 5);

                img_ColEdgeInfo[colEdgeCnt][3] = 1;

                colEdgeCnt++;
            }else{
                img_ColEdgeInfo[colEdgeCnt][2] = img_ColEdgeInfo[colEdgeCnt-1][2];
                colEdgeCnt++;
            }

            if(topEdgeStart>=50){
                topEdgeStart = 45;
            }
            if(topEdgeEnd < 0){
                topEdgeEnd = 0;
            }

            topEdgeFind = 0;

        }
        if(img_ColEdgeInfo[0][2] == 0 || img_ColEdgeInfo[0][3] != 1 || img_ColEdgeInfo[1][2] == 0){
            colEdgeEnable = 0;
            img_ColEdgeInfo[0][3] = 5;
        }else{
            //sprintf(TXBuffer,"[%u]",colEdgeEnable);
            //TUart0_Puts(TXBuffer);
            img_ColEdgeInfo[colEdgeCnt][3] = 5;
            //Image_Ptr[Image_Row[(int)img_ColEdgeInfo[0][2]] * 20 + (uint8)(img_ColEdgeInfo[0][5]/8)] |= 1<<(7-img_ColEdgeInfo[0][5]&0x07);
            Image_Ptr[(int)img_ColEdgeInfo[0][2] * 40 + (uint8)(img_ColEdgeInfo[0][5]/8)] |= 1<<(7-img_ColEdgeInfo[0][5]&0x07);
            for(u = 1;u<colEdgeCnt;u++){     //中线补线处理循环
                if(img_ColEdgeInfo[u][2]==0){
                    img_ColEdgeInfo[u][2] = img_ColEdgeInfo[u-1][2];
                    world_ColEdgeInfo[u] = correctTable[img_ColEdgeInfo[u][2]][img_ColEdgeInfo[u][5]];
                }
                //Image_Ptr[Image_Row[(int)img_ColEdgeInfo[u][2]] * 20 + (uint8)(img_ColEdgeInfo[u][5]/8)] |= 1<<(7-img_ColEdgeInfo[u][5]&0x07);
                //Image_Ptr[Image_Row[img_ColEdgeInfo[u][2]] * 20 + ((int)world_ColEdgeInfo[u]/8)] |= 1<<(7-(int)world_ColEdgeInfo[u]&0x07);
                Image_Ptr[img_ColEdgeInfo[u][2] * 40 + ((int)world_ColEdgeInfo[u]/8)] |= 1<<(7-(int)world_ColEdgeInfo[u]&0x07);
            }
            if(colBool){

                if(direct == 1){
                    tempa = 50 - img_ColEdgeInfo[colEdgeCnt-1][2];
                    tempb = v - img_EdgeInfo[49][2];
                    for(row = 49;row >= (50-tempa);row--){
                    	img_EdgeInfo[row][2] = (uint16)(img_EdgeInfo[49][2] + ((49.0 - row)/(tempa+1)) * tempb);
                        world_EdgeInfo[row] = correctTable[row][img_EdgeInfo[row][2]];//获取世界边界信息

                    }
                }else{
                    tempa = 50 - img_ColEdgeInfo[colEdgeCnt-1][2];
                    tempb = (img_EdgeInfo[49][1] - v)*0.5;
                    for(row = 49;row>=(50-tempa);row--){
                    	img_EdgeInfo[row][2] = (uint16)(img_EdgeInfo[49][2] - ((49.0 - row)/(tempa+1)) * tempb);
                        world_EdgeInfo[row] = correctTable[row][img_EdgeInfo[row][2]];//获取世界边界信息

                    }
                }
            }
            img_ColEdgeInfo[colEdgeCnt][3] = 5;
        }
    }
    debugCnt = 9;
#endif
#if 1

    for(row = IMG_H-2;row>0;row--){     //中线滤波处理循环
        debugCnt = 7;
        if(img_EdgeInfo[row][3] == 5 || img_EdgeInfo[row][3] == 7)
            break;
        if(img_EdgeInfo[row-1][2]&&img_EdgeInfo[row+1][2]){     //对三边线都可用时进行边线滤波

            img_EdgeTemp = (img_EdgeInfo[row-1][2] + img_EdgeInfo[row+1][2])*0.5;   //中线滤波
            world_EdgeInfo[row] = (world_EdgeInfo[row]-1 + world_EdgeInfo[row+1])*0.5;               //重新获取获取世界边界信息
            img_EdgeInfo[row][2] = img_EdgeTemp;

            if(img_EdgeTemp>159||img_EdgeTemp<0)
                continue;
            //Image_Ptr[Image_Row[row] * 20 + (uint8)(img_EdgeTemp/8)] |= 1<<(7-img_EdgeTemp&0x07);
            //Image_Ptr[row * 40 + (uint8)(img_EdgeTemp/8)] |= 1<<(7-img_EdgeTemp&0x07);
            //Image_Ptr[Image_Row[row] * 20 + (uint8)(world_EdgeInfo[row]/8)] |= 1<<(7-(int)world_EdgeInfo[row]&0x07);
            Image_Ptr[row * 40 + (uint8)(world_EdgeInfo[row]/8)] |= 1<<(7-(int)world_EdgeInfo[row]&0x07);
        }
    }
    debugCnt = 10;
#endif

#if 1

    lineOffsetCnt = 0;
    for(row = IMG_H-1;row>0;row --){
        if(img_EdgeInfo[row][3] == 5)
            break;
        lineOffsetCnt += (world_EdgeInfo[row] - IMG_W_HALF);
        lineOffsetValueCnt[row] = lineOffsetCnt;
        lineOffsetValue[row] = world_EdgeInfo[row] - IMG_W_HALF;
        if(img_EdgeInfo[row][3] == 7)
            break;
    }
    debugCnt = 18;
#endif
#if 0
    SlopeCnt = 0.0;
    SlopeCnt = 0.0;
    img_EdgeTemp = img_EdgeInfo[49][2];
    for(row = IMG_H-2;row>0;row --){
        if(img_EdgeInfo[row][3] == 5||img_EdgeInfo[row][3] == 7)
            break;
        Slope[row] = (img_EdgeInfo[row][2] - img_EdgeTemp)/((50.0-row));
        SlopeAve += Slope[row];
        SlopeCnt++;
        //if(img_EdgeInfo[row][3] == 7)
        //    break;
    }
    if(0&&colEdgeEnable&&img_ColEdgeInfo[0][2]>20){
        for(u = 0;u<30;u++){
            if(img_ColEdgeInfo[u][3] == 5)
                break;
            if(img_ColEdgeInfo[u][2]){
                Slope[SlopeCnt] = (img_ColEdgeInfo[u][5] - img_EdgeTemp)/((50.0-img_ColEdgeInfo[u][2])*2.4);
                SlopeAve += Slope[SlopeCnt];
                SlopeCnt++;
            }
        }
    }

    SlopeAve = SlopeAve/SlopeCnt;
    debugCnt = 19;
#endif
#if 0
    tpRow1 = 1;//赋值三点数组
    tpCol1 = IMG_W_HALF;
    for(row = IMG_H-3;row>=0;row --){
        allRadiusOfCurva[row] = 0.0;
        if(img_EdgeInfo[row][3] == 5 || img_EdgeInfo[row][3] == 7)
            break;

        tpRow3 = (int16)(IMG_H-row);
        tpCol3 = img_EdgeInfo[row][2];
        tpRow2 = (int16)((tpRow1 + IMG_H - row)/2);
        tpCol2 = img_EdgeInfo[IMG_H-tpRow2][2];

        tpLen1 = (int16)invSqrt(tpRow1-tpRow2,tpCol1-tpCol2);
        tpLen2 = (int16)invSqrt(tpRow1-tpRow3,tpCol1-tpCol3);
        tpLen3 = (int16)invSqrt(tpRow2-tpRow3,tpCol2-tpCol3);

        tempe = ((tpRow2-tpRow1)*(tpCol3-tpCol1) - (tpRow3-tpRow1)*(tpCol2-tpCol1))*0.5;
        if(tempe>1||tempe<-1)
            allRadiusOfCurva[row] = (float)((tpLen1 * tpLen2 * tpLen3 / tempe * 0.25));
        else
            allRadiusOfCurva[row] = 0.0;
    }
    allRadiusOfCurva[row] = 0.0;



#endif
#if 0
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

    for(row = IMG_H-1;row>0;row --){
        debugCnt = 11;
        if(img_EdgeInfo[row][3] == 5 || img_EdgeInfo[row][3] == 7)
            rowBreak = 1;
        if(controlError||controlFinsh)
            break;
        switch(controlMode){
            case 0:{
                //if(rowBreak == 0 && img_EdgeInfo[row][2] != 0){
                if(rowBreak == 0){
                    temp = (uint8)(img_EdgeInfo[row][2] - IMG_W_HALF + centLineOffset);
                    if(temp < (centLineOffset<<1) || lineOffsetCnt<5){
                        lineOffsetRow = (uint8)row;
                        lineOffsetCnt ++;
                        lineOffsetValueCnt += (temp - centLineOffset);
                    }else{
                        lineOffsetRow = (uint8)row;
                        lineOffsetCnt ++;
                        lineOffsetValueCnt += (temp - centLineOffset);

                        controlMode++;    //改变控制方式

                        if(lineOffsetRow < 15){
                            controlFinsh = 1;
                            break;
                        }

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
                //if(img_EdgeInfo[row][2] != 0){
                if(rowBreak == 0){
                    threePoint[2][0] = row;
                    threePoint[2][1] = img_EdgeInfo[row][2];
                    if( (world_ActualRange[row] - tpStartDist) > 80){

                        //由于补线完成，所以可以直接读中间那条线，而不需要用循环找线
                        threePoint[1][0] =  (threePoint[2][0] + threePoint[0][0])*0.5;
                        threePoint[1][1] = img_EdgeInfo[threePoint[1][0]][2];

                        if(world_ActualRange[row]<120){
                            threePoint2[1][0] = 0;
                            threePoint2[2][0] = 0;

                            threePoint2[0][0] = threePoint[1][0];//赋值三点数组
                            threePoint2[0][1] = threePoint[1][1];
                            controlMode ++;
                        }else{
                            controlFinsh = 1;
                        }
                    }
                }else if(rowBreak){
                    threePoint[1][0] =  (threePoint[2][0] + threePoint[0][0])*0.5;
                    threePoint[1][1] = img_EdgeInfo[threePoint[1][0]][2];

                    if(world_ActualRange[row]<160){
                        threePoint2[1][0] = 0;
                        threePoint2[2][0] = 0;

                        threePoint2[0][0] = row;//赋值三点数组
                        threePoint2[0][1] = img_EdgeInfo[row][2];
                        controlMode ++;
                    }else{
                        controlMode ++;
                        controlFinsh = 1;
                    }
                }
            }break;
            case 2:{
                if(colEdgeEnable == 0){
                    controlFinsh = 1;
                    break;
                }
                tp2RowCnt++;
                if(rowBreak == 0){
                    threePoint2[2][0] = row;
                    threePoint2[2][1] = img_EdgeInfo[row][2];
                }else if(rowBreak){
                    temp = 0;
                    if(img_ColEdgeInfo[temp][2] != 0){
                        while(img_ColEdgeInfo[temp][2] !=0 && img_ColEdgeInfo[temp][3] !=5){
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
                        midThreePoint2Col2 = (uint8)(midThreePoint2Col1+1);
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
    debugCnt = 12;
#endif
#if 0
    debugCnt = 13;
    if(controlMode >=3){
        tempa = threePoint2[0][0];
        tempb = threePoint2[0][1];
        tempe = ((threePoint2[1][0]-tempa)*(threePoint2[2][1]-tempb) - (threePoint2[2][0]-tempa)*(threePoint2[1][1]-tempb))*0.5;
        if(tempe == 0){
            controlMode = 2;
        }else{
            tempc = (int16)(threePoint2[1][0] - threePoint2[0][0]);
            tempd = (int16)(threePoint2[1][1] - threePoint2[0][1]);
            threePoint2[0][2] = (int16)sqrt_16(tempc*tempc+tempd*tempd);
            //sprintf(TXBuffer,"%u\n",tempc*tempc+tempd*tempd);
            //TUart0_Puts(TXBuffer);
            tempc = (int16)(threePoint2[2][0] - threePoint2[1][0]);
            tempd = (int16)(threePoint2[2][1] - threePoint2[1][1]);
            threePoint2[1][2] = (int16)sqrt_16(tempc*tempc+tempd*tempd);
            //sprintf(TXBuffer,"%u\n",tempc*tempc+tempd*tempd);
            //TUart0_Puts(TXBuffer);
            tempc = (int16)(threePoint2[2][0] - threePoint2[0][0]);
            tempd = (int16)(threePoint2[2][1] - threePoint2[0][1]);
            threePoint2[2][2] = (int16)sqrt_16(tempc*tempc+tempd*tempd);
            //sprintf(TXBuffer,"%u\n",tempc*tempc+tempd*tempd);
            //TUart0_Puts(TXBuffer);

            radiusOfCurva_2 = (float)(threePoint2[0][2] * threePoint2[1][2] * threePoint2[2][2] / tempe * 0.25);
            //tp2ResSteerAngle = 90 / radiusOfCurva_2;
        }
    }
    debugCnt = 14;
    if(controlMode >=2){
        //TUart0_Putchar('G');
        tempa = threePoint[0][0];
        tempb = threePoint[0][1];
        tempe = ((threePoint[1][0]-tempa)*(threePoint[2][1]-tempb) - (threePoint[2][0]-tempa)*(threePoint[1][1]-tempb))*0.5;
        if(tempe>=-1&&tempe <= 1){
            tempe = ((threePoint[1][0]-IMG_W_HALF)*(threePoint[2][1]-tempb) - (threePoint[2][0]-IMG_W_HALF)*(threePoint[1][1]-tempb))*0.5;
            tempc = threePoint[1][0] - IMG_W_HALF;
            tempd = threePoint[1][1] - threePoint[0][1];
            threePoint[0][2] = (int16)sqrt_16(tempc*tempc+tempd*tempd);
            tempc = threePoint[2][0] - threePoint[1][0];
            tempd = threePoint[2][1] - threePoint[1][1];
            threePoint[1][2] = (int16)sqrt_16(tempc*tempc+tempd*tempd);
            tempc = threePoint[2][0] - IMG_W_HALF;
            tempd = threePoint[2][1] - threePoint[0][1];
            threePoint[2][2] = (int16)sqrt_16(tempc*tempc+tempd*tempd);
        }else{
            tempc = threePoint[1][0] - threePoint[0][0];
            tempd = threePoint[1][1] - threePoint[0][1];
            threePoint[0][2] = (int16)sqrt_16(tempc*tempc+tempd*tempd);
            //sprintf(TXBuffer,"%u\n",tempc*tempc+tempd*tempd);
            //TUart0_Puts(TXBuffer);
            tempc = threePoint[2][0] - threePoint[1][0];
            tempd = threePoint[2][1] - threePoint[1][1];
            threePoint[1][2] = (int16)sqrt_16(tempc*tempc+tempd*tempd);
            //sprintf(TXBuffer,"%u\n",tempc*tempc+tempd*tempd);
            //TUart0_Puts(TXBuffer);
            tempc = threePoint[2][0] - threePoint[0][0];
            tempd = threePoint[2][1] - threePoint[0][1];
            threePoint[2][2] = (int16)sqrt_16(tempc*tempc+tempd*tempd);
            //sprintf(TXBuffer,"%u\n",tempc*tempc+tempd*tempd);
            //TUart0_Puts(TXBuffer);

        }

        radiusOfCurva_1 = (float)(threePoint[0][2] * threePoint[1][2] * threePoint[2][2] / tempe*0.25);
        //tpResSteerAngle = 90 / radiusOfCurva_1;
    }
    debugCnt = 15;
    if(controlMode >=1){
        if((50-lineOffsetRow-lineOffsetCnt)>5){
            lineRealDis = world_ActualRange[lineOffsetRow];
            lineRealRate = (uint8)(50-lineOffsetRow);

            lineResSteerAngle = (float)(28.647*(lineOffsetValueCnt/(lineRealRate+1)*colCorDisTable[lineOffsetRow])/lineRealDis);
            //lineResSteerAngle = (float)(114.591*(lineOffsetValueCnt*colCorDisTable[50-lineOffsetCnt])/lineRealDis);
        }else{
            lineRealDis = world_ActualRange[50-lineOffsetCnt];
            lineRealRate = lineOffsetCnt;

            lineResSteerAngle = (float)(28.647*(lineOffsetValueCnt/(lineRealRate+1)*colCorDisTable[50-lineOffsetCnt])/lineRealDis);
            //lineResSteerAngle = (float)(114.591*(lineOffsetValueCnt*colCorDisTable[50-lineOffsetCnt])/lineRealDis);

        }
    }
    debugCnt = 16;
#endif
    //TUart0_Puts("PIT1!\r\n");
    bool = 0;
    MCF_PIT1_PCSR &= ~MCF_PIT_PCSR_EN;
    MCF_PIT1_PCSR |= MCF_PIT_PCSR_PIF;  //清中断标志
}
#endif
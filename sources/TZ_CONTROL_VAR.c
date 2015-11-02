#include "DIY_ComDef.h"
#ifdef TZ_CONTROL_VAR

uint8 debugCnt;//调试出错位置

uint8 controlMode = 0;    //控制方式
uint8 rowBreak = 0;
uint8 controlError = 0;// 控制量计算错误变量
uint8 controlFinsh = 0;//

/*
    0:正常
    1：近距离直线控制中线结束满足
    2：三点求曲率1距离满足 中间点丢失错误
    3：三点求曲率1中线结束满足 中间点丢失错误
*/

/*近距离直线控制变量*/


uint8 lineRealDis;
uint8 lineRealRate;

float lineResSteerAngle;
/*中距离曲率控制变量*/
/*
int threePoint[3][3];
uint8 tpStartDist = 0;
uint8 tpEndDist = 0;
uint8 midThreePointCol1,midThreePointCol2;

float radiusOfCurva_1;

uint8 tpResSteerAngle;

/*远距离曲率控制变量*/
/*
int threePoint2[3][3];
uint8 tp2StartDist = 0;
uint8 tp2EndDist = 0;
uint8 midThreePoint2Col1,midThreePoint2Col2;
uint8 tp2RowCnt = 0;
uint8 tp2ColCnt = 0;

float radiusOfCurva_2;

uint8 tp2ResSteerAngle;

/*远距离曲率控制变量*/
/*
int threePoint3[3][3];
uint8 tp3StartDist = 0;
uint8 tp3EndDist = 0;
uint8 midThreePoint3Col1,midThreePoint3Col2;
uint8 tp3RowCnt = 0;
uint8 tp3ColCnt = 0;

float radiusOfCurva_3;

uint8 tp3ResSteerAngle;

/*总控制变量*/
float allRadiusOfCurva[IMG_H];
int16 tpCol1,tpRow1;
int16 tpCol2,tpRow2;
int16 tpCol3,tpRow3;
int16 tpLen1,tpLen2,tpLen3;

uint8 centLineOffset = 5;
uint8 lineOffsetRow = 0;

/*总控制变量2*/
uint8 lenOf2Screen;     //两幅图像间行驶的距离
int lenOf2Screen_Row; //下幅图像开始在这幅图像的位置
uint8 lenOfSpeed_Cnt;   //速度指向的图像行数减去下幅图像指向的行数的计数
int lenOfSpeed_Row;   //速度指向的图像行数



/*电机相关变量*/
uint16 baseSpeed = 160;       //基础速度

float leftMotorBase;        //左GPT计数器基准时间
float rightMotorBase;       //右GPT计数器基准时间

uint16 leftMSCnt,rightMSCnt;    //左右电机GPT值

uint8 leftCnt,rightCnt;     //左右电机GPT计数值

uint16 leftMotorCntAve;     //GPT左平均值
uint16 rightMotorCntAve;    //GPT右平均值
uint16 leftMotorCnt[8];     //GPT左8次值
uint16 rightMotorCnt[8];    //GPT右8次值

float motorSpeed;          //当前速度值 单位(cm/s)
uint16 leftMotorSpeed;      //当前左真实速度
uint16 rightMotorSpeed;         //当前右真实速度
float leftMSPwm;               //左电机当前PWM值
float rightMSPwm;              //右电机当前PWM值
float motorOffsetSpeed;        //PID调节后电机真实速度的差值
float motorOffsetCnt;         //PID调节后的GPT差值
int motorMSPwm;             //PID调节后总的电机PWM值

float motorOffset;      //根据电机转速求出的偏移值

int motorCnt;
int motorCnt1;
int motorCnt2;

/*舵机相关变量*/
uint16 pwmCnt;              //当前舵机PWM值
float steerOffsetCnt;       //计算后的舵机偏移值

/*斜率相关变量*/
float Slope[80];
uint8 SlopeCnt;
float SlopeAve;

/*PID相关变量*/

/*总的控制变量*/
int lineOffsetValueCnt[IMG_H];          //累积偏移值
int lineOffsetValue[IMG_H];             //偏移值
int lineOffsetCnt = 0;                  //临时偏移值记录变量
uint8 PITCnt = 0;                       //PIT中断计数

#endif

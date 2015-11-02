#include "DIY_ComDef.h"
#ifdef TZ_CONTROL_VAR

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

uint8 centLineOffset = 5;
uint8 lineOffsetCnt = 0;
uint8 lineOffsetRow = 0;
int16 lineOffsetValueCnt = 0;

uint8 lineRealDis;
uint8 lineRealRate;

float lineResSteerAngle;
/*中距离曲率控制变量*/

int threePoint[3][3];
uint8 tpStartDist = 0;
uint8 tpEndDist = 0;
uint8 midThreePointCol1,midThreePointCol2;

float radiusOfCurva_1;

uint8 tpResSteerAngle;

/*远距离曲率控制变量*/

int threePoint2[3][3];
uint8 tp2StartDist = 0;
uint8 tp2EndDist = 0;
uint8 midThreePoint2Col1,midThreePoint2Col2;
uint8 tp2RowCnt = 0;
uint8 tp2ColCnt = 0;

float radiusOfCurva_2;

uint8 tp2ResSteerAngle;

/*总控制变量*/

/*舵机相关变量*/
uint16 pwmCnt;
/*电机相关变量*/
float leftMotorBase;
float rightMotorBase;

uint16 leftMSCnt,rightMSCnt;           //左右电机GPT值

uint8 leftMotorSpeed;
uint8 leftCnt,rightCnt;
uint8 rightMotorSpeed;
uint16 motorSpeed;
float tanValue;

uint16 leftMotorCnt[8];
uint16 rightMotorCnt[8];
uint16 motorCnt;

uint16 allMotorCnt;

/*PID相关变量*/

#endif

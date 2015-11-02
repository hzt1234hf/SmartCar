#ifndef TZ_CONTROL_VAR
#define TZ_CONTROL_VAR

extern uint8 controlMode;    //控制方式
extern uint8 rowBreak;
extern uint8 controlError;// 控制量计算错误变量
extern uint8 controlFinsh;//
/*
    0:正常
    1：近距离直线控制中线结束满足
    2：三点求曲率1距离满足 中间点丢失错误
    3：三点求曲率1中线结束满足 中间点丢失错误
*/

//近距离直线控制变量

extern uint8 centLineOffset;
extern uint8 lineOffsetCnt;
extern uint8 lineOffsetRow;
extern int16 lineOffsetValueCnt;

extern uint8 lineRealDis;
extern uint8 lineRealRate;

extern float lineResSteerAngle;

//中距离曲率控制变量

extern int threePoint[3][3];
extern uint8 tpStartDist;
extern uint8 tpEndDist;
extern uint8 midThreePointCol1,midThreePointCol2;

extern float radiusOfCurva_1;

extern uint8 tpResSteerAngle;

//远距离曲率控制变量

extern int threePoint2[3][3];
extern uint8 midThreePoint2Col1,midThreePoint2Col2;
extern uint8 tp2RowCnt;
extern uint8 tp2ColCnt;

extern float radiusOfCurva_2;

extern uint8 tp2ResSteerAngle;

/*电机相关变量*/

extern float leftMotorBase;
extern float rightMotorBase;

extern uint16 leftMSCnt,rightMSCnt;           //左右电机GPT值\

extern uint8 leftMotorSpeed;   //左右电机速度值
extern uint8 rightMotorSpeed;  //左右电机速度值
extern uint8 leftCnt,rightCnt;
extern uint16 motorSpeed;
extern float tanValue;

extern uint16 leftMotorCnt[8];
extern uint16 rightMotorCnt[8];
extern uint16 motorCnt;

extern uint16 allMotorCnt;

/*舵机相关变量*/
extern uint16 pwmCnt;

#endif
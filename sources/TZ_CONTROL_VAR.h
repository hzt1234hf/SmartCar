#ifndef TZ_CONTROL_VAR
#define TZ_CONTROL_VAR

extern uint8 debugCnt;//调试出错位置

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
extern int8 centerLineSub;
extern uint8 lineOffsetRow;


extern uint8 lineRealDis;
extern uint8 lineRealRate;

extern float lineResSteerAngle;

//中距离曲率控制变量
/*
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

//远距离曲率控制变量

extern int threePoint3[3][3];
extern uint8 midThreePoint3Col1,midThreePoint3Col2;
extern uint8 tp3RowCnt;
extern uint8 tp3ColCnt;

extern float radiusOfCurva_3;

extern uint8 tp3ResSteerAngle;
/*总控制变量*/
extern float allRadiusOfCurva[IMG_H];
extern int16 tpCol1,tpRow1;
extern int16 tpCol2,tpRow2;
extern int16 tpCol3,tpRow3;
extern int16 tpLen1,tpLen2,tpLen3;

/*总控制变量2*/
extern uint8 lenOf2Screen;     //两幅图像间行驶的距离
extern int lenOf2Screen_Row; //下幅图像开始在这幅图像的位置
extern uint8 lenOfSpeed_Cnt;   //速度指向的图像行数减去下幅图像指向的行数的计数
extern int lenOfSpeed_Row;   //速度指向的图像行数

/*电机相关变量*/

extern float leftMotorBase;
extern float rightMotorBase;

extern uint16 leftMSCnt,rightMSCnt;           //左右电机GPT值\

extern uint16 leftMotorSpeed;
extern uint16 rightMotorSpeed;
extern uint16 leftMotorCntAve;     //左平均值
extern uint16 rightMotorCntAve;    //右平均值
extern uint16 leftMotorCnt[8];     //左8次值
extern uint16 rightMotorCnt[8];    //右8次值

extern uint8 leftCnt,rightCnt;
extern float motorSpeed;
extern float leftMSPwm;
extern float rightMSPwm;
extern int motorMSPwm;
extern float motorOffsetCnt;         //PID调节后的GPT差值
extern float motorOffsetSpeed;

extern int motorCnt;
extern int motorCnt1;
extern int motorCnt2;

extern float motorOffset;      //根据电机转速求出的偏移值

/*舵机相关变量*/
extern uint16 pwmCnt;
extern float steerOffsetCnt;

/*斜率相关变量*/
extern float Slope[80];
extern uint8 SlopeCnt;
extern float SlopeAve;
/*总的控制变量*/
extern int lineOffsetValueCnt[IMG_H];       //累积偏移值
extern int lineOffsetValue[IMG_H];          //偏移值
extern int lineOffsetCnt;       //临时偏移值记录变量
extern uint8 PITCnt;                       //PIT中断计数


#endif
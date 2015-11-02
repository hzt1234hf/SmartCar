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

//近距离直线控制变量

uint8 centLineOffset = 5;
uint8 lineOffsetCnt = 0;
uint8 lineOffsetRow = 0;
int16 lineOffsetValueCnt = 0;

//中距离曲率1控制变量

uint8 threePoint[3][2];
uint8 tpStartDist = 0;
uint8 tpEndDist = 0;
uint8 midThreePointCol1,midThreePointCol2;
//远距离曲率1控制变量

uint8 threePoint2[3][2];
uint8 tp2StartDist = 0;
uint8 tp2EndDist = 0;
uint8 midThreePoint2Col1,midThreePoint2Col2;
uint8 tp2RowCnt = 0;
uint8 tp2ColCnt = 0;

#endif

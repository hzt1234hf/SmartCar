#ifndef TZ_MCF52259_I2C
#define TZ_MCF52259_I2C

extern void TI2Cx_Init(uint8 mode);
extern void I2CsendByte(uint8 data, uint8 address, uint8 id);
extern uint8 I2CreceiveByte(uint8 address, uint8 id);

#endif
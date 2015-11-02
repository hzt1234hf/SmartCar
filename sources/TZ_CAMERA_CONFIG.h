#ifndef TZ_CAMERA_CONFIG
#define TZ_CAMERA_CONFIG

#define CAMERA_W    80
#define CAMERA_W_8  10
#define CAMERA_H    60
#define CAMERA_OFFSET   8
#define CAMERA_OFFSET_8   8/8

extern vuint8 Image[CAMERA_H][CAMERA_W_8];
extern vuint8 * ImagePtr;

extern void TZ_OV7725_Init();

#endif

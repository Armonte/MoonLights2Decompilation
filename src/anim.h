#ifndef ANIM_H
#define ANIM_H

#include <windows.h>
#include <stdbool.h>
#include <stdint.h>


int g_totalFrames;
int g_currentFrame;

extern int g_maxScreenWidth;
extern int g_maxScreenHeight;
extern int g_totalFrames;
extern int g_currentFrame;

int updateAnimations(void);

extern int setPixel(uint8_t colorIndex, int x, int y);


int disableAnimation(void);

void* InitAnimationControl(HWND hwnd, int width, int height, int bitDepth);

int isGraphicsSystemInitialized(void);

int clearGlobalAnimationControl(void);

int updateAnimationState(int gridSize);

int initializeAnimationParameters(int frameCount, bool isReverse, int totalFrames);

int updateAnimationState(int gridSize);

int fillRectColorWrapper(const int* data);

int fillRectangleWithColor(unsigned char color, int startX, int startY, unsigned int width, int height);

int renderPixelAreaWrapper(int* params);

int renderPixelArea(char* sourceBuffer, char baseColor, int renderMode, int startX, int startY,
    int width, int height, int useDirectMemoryAccess);



#endif // ANIM_H
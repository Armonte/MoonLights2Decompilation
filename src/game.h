#include "bitmap.h"
#include "anim.h"
#include "ResourceUtils.h"
#include "main.h"
#include "reg.h"
#include "joy.h"
#include "stdint.h"
#include <windows.h>
#include "KeyboardManager.h"
extern int gamestateVariable_Unk;
extern BYTE g_keyboardStateData[256];

extern void processGraphicsAndBitmap(void);
extern void ProcessScreenUpdatesAndResources(void);

//extern void* g_keyboardStateData;
extern int g_isGamePaused;
extern int g_gameSpeed;
extern DWORD g_lastActionTickTime;
extern DWORD g_frameTickCount;
extern DWORD g_gameMaxTickCount;
extern DWORD g_frameMaxTickCount;


extern int g_gameStateVariable;
extern int g_currentResourceSize2;

extern int timerInitialValue;
extern int globalTimerValue;

extern HDC g_hdc;
extern int g_isMouseActive;
extern int g_currentTickIncrementIndex;
extern bool g_isTimerDivisionActive;
extern int g_timerDivisionFactor;
extern int g_timerCurrentValue;
extern int g_timerDividedValue;
extern HWND mainWindowHandle;
extern int nBitCount;
extern int g_timerInitialValue;
extern int globalTimerValue;

extern int g_isFullscreenReg;
extern HDC g_hdc;
extern int g_isMouseActive;
extern int g_currentTickIncrementIndex;
extern HWND mainWindowHandle;
extern int nBitCount;


extern int fillRectColorWrapper(const int* data);
extern int renderPixelAreaWrapper(int* params);
extern void cleanupResources(void);

extern void* GlobalResourceArray;

int DecrementAndResetVariables(void);
int resetGlobalVariables();


DWORD PerformDelayAction(int delayAmount);

int InitializeWindow(HDC hdc);

int mainGameLoop(void);

int updateGameState1();

void resetGameState2(void);
void resetResourceCounter2(void);

int decrementAndResetVariables(void);

int isRectangleWithinScreen(int posX, int posY, int width, int height);

int updateRenderingData(uint32_t imageId, int renderFlag, int xOffset, int yOffset,
    int xPos, int yPos, int width, int height, int renderType);

DWORD setupGameEnvironment(void);

HFONT createCustomFont(HDC deviceContext, const char* fontName, int height, int width);

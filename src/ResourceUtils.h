#ifndef RESOURCEUTILS_H
#define RESOURCEUTILS_H

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <Windows.h>
#include "reg.h"
//#include "bitmap.h"
#include "anim.h"

// External variable declarations
extern void* GlobalResourceArray;
extern int resourceArraySize;
extern void* dword_4C0770;
extern int dword_4C0774;
extern void* dword_4C077C;
extern int dword_4C0780;

extern int32_t** g_globalResourceArray;
extern uint32_t g_resourceArraySize;
extern int g_currentGameStateVariable;
extern int g_currentResourceSize;

extern int32_t** g_globalResourceArray;
extern uint32_t g_resourceArraySize;

extern int g_resourceHandlerState;

extern void* g_resourceManager;
extern unsigned char g_initializationCounter;
extern int* g_screenPosHorizontal;
extern int g_isFullscreenReg;
extern int g_screenResolutionFlag;
extern HWND g_parentWindow;
extern void* g_graphicsInterface;

int InitializeBuffer(unsigned int* buffer, int x_start, int y_start, int width, int height);

int initializeResourceHandlers(void);

int processScreenUpdatesAndResources(void);

int CopyAndManipulateMemoryBlock(char* destBuffer, int elementCount);

int __cdecl ReallocateGlobalResourceArray(int newSize);

int __cdecl ReallocateRenderBuffer(int newSize);

int __cdecl ReallocateMemoryBuffer(int newSize);

int updatePointerArray(uint32_t index, int32_t* newEntry);

void resetGameStateVariable(void);
void cleanupResources(void);
void resetResourceCounter(void);




#endif // RESOURCEUTILS_H

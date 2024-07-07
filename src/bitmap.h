#ifndef BITMAP_H
#define BITMAP_H

#include <windows.h>
#include <ddraw.h>
#include <stdint.h>


extern int g_resourceHandlerState;
extern LPDIRECTDRAW g_directDrawObject;
//extern void* g_graphicsInterface;
extern HPALETTE g_globalPalette;
extern PALETTEENTRY g_gamePaletteEntries[256];

extern int g_maxScreenWidth;
extern int g_maxScreenHeight;
extern char* lpBits;
extern HWND gameWND;

//void* g_graphicsInterface = NULL;


int updateColorInformation(void);
PALETTEENTRY* getPaletteEntry(void);
HPALETTE createDefaultPalette(void);


int FindAndLoadBitmap(HWND windowHandle, const char* filter, char* fullPath, HANDLE* fileHandle, const char* defaultExtension);
extern const char* a256ColorBmpFil;
extern void* off_43F2DC;

WORD* AllocateBuffer(char* graphicsBuffer, int width, int height, const void* paletteData,
    int paletteSize, int colorOffset, unsigned int modifyZeroFlag,
    DWORD* outputSize, unsigned int flipFlag);


void processGraphicsAndBitmap(void);

char* createGraphicsBuffer(int x, int y, int width, int height);

DWORD WriteBitmapToFile(LPCSTR filePath, LPCVOID buffer, LONG offset, DWORD bufferSize);

BYTE* ProcessBitmapData(void* bitmapHandle, HBITMAP* outBitmap, DWORD* outColorCount, int* outWidth, int* outHeight);

int isGraphicsInitialized(void);


int setPixel(uint8_t colorIndex, int x, int y);

int __cdecl flipBitmapVertically(void* bitmapHandle);

int __cdecl GetBitmapColorTableSize(void* bitmapHandle, int* numEntries);

void* GetBitmapBufferDetails(void* bitmapHandle, DWORD* width, DWORD* height);

int flipImageVertically(char* imageData, int width, int height);

HPALETTE CreateCustomPalette(intptr_t colorTableSize, int numEntries);

HPALETTE GetBitmapPalette(int bitmapHandle);

HPALETTE handleGlobalPalette(HPALETTE palette);



#endif // BITMAP_H


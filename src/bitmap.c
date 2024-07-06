#include "bitmap.h"
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include "game.h"
#include <excpt.h>

#define PALETTE_SIZE 256
#define LOGPALETTE_SIZE 1032  // 0x408 bytes
#define PALETTE_ENTRIES_SIZE 1024  // 256 * 4 bytes per entry

#define COLOR_TABLE_1_SIZE 40    // 0x28 bytes
#define COLOR_TABLE_2_SIZE 984   // 0x3D8 bytes

int g_resourceHandlerState = 0;
LPDIRECTDRAW g_directDrawObject = NULL;
HPALETTE g_globalPalette = NULL;
PALETTEENTRY g_gamePaletteEntries[256];


extern uint8_t* g_screenBuffer;


extern void* g_globalGraphicsBuffer;

extern HWND g_animationWindow = NULL;
extern void* g_localBitsBuffer = NULL;
extern void* g_globalGraphicsBuffer = NULL;
extern DWORD g_bufferSize = 0;
extern int g_maxScreenWidth = 0;
extern int g_maxScreenHeight = 0;
extern int g_bitDepth = 0;
extern int g_bitsBuffer = 0;


//extern HWND;



//HPALETTE CreateCustomPalette(int colorTableSize, int numEntries);

const char* a256ColorBmpFil = "256 color BMP file (*.bmp)|*.bmp|";
void* off_43F2DC = NULL;  // Initialize this to the correct value if known

int FindAndLoadBitmap(HWND windowHandle, const char* filter, char* fullPath, HANDLE* fileHandle, const char* defaultExtension)
{
    char defaultFilter[] = "All files (*.*)|*.*|";
    char currentDirectory[MAX_PATH];
    char selectedFile[MAX_PATH] = { 0 };
    char fileFilter[MAX_PATH];
    char tempFileName[MAX_PATH];
    OPENFILENAMEA ofn = { 0 };
    char drive[_MAX_DRIVE], dir[_MAX_DIR], fileName[_MAX_FNAME], ext[_MAX_EXT];

    if (!GetCurrentDirectoryA(MAX_PATH, currentDirectory))
        return 0;

    const char* usedFilter = filter ? filter : defaultFilter;
    strcpy_s(fileFilter, sizeof(fileFilter), usedFilter);

    // Replace '|' with '\0'
    for (int i = 0; fileFilter[i]; ++i)
    {
        if (fileFilter[i] == '|')
            fileFilter[i] = '\0';
    }

    ofn.lStructSize = sizeof(OPENFILENAMEA);
    ofn.hwndOwner = windowHandle;
    ofn.lpstrFilter = fileFilter;
    ofn.lpstrFile = selectedFile;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFileTitle = tempFileName;
    ofn.nMaxFileTitle = MAX_PATH;
    ofn.lpstrInitialDir = currentDirectory;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    ofn.lpstrDefExt = defaultExtension;

    if (!GetOpenFileNameA(&ofn))
        return 0;

    if (fullPath)
    {
        strcpy_s(fullPath, MAX_PATH, selectedFile);
        errno_t err = _splitpath_s(fullPath,
            drive, _MAX_DRIVE,
            dir, _MAX_DIR,
            fileName, _MAX_FNAME,
            ext, _MAX_EXT);
        if (err != 0)
            return 0;

        // Convert extension to uppercase
        for (int i = 0; ext[i]; ++i)
        {
            ext[i] = toupper((unsigned char)ext[i]);
        }

        err = _makepath_s(fullPath, MAX_PATH, drive, dir, fileName, ext);
        if (err != 0)
            return 0;
    }

    if (fileHandle)
    {
        *fileHandle = CreateFileA(selectedFile, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (*fileHandle == INVALID_HANDLE_VALUE)
            return 0;
    }

    return 1;
}

WORD* AllocateBuffer(char* sourceBuffer, int width, int height, const void* paletteData,
    int paletteSize, int colorOffset, unsigned int modifyZeroFlag,
    DWORD* outputSize, unsigned int flipFlag)
{
    char* currentSource;
    char* alignedDestination;
    int rowCount;
    signed int remainingPixels;
    WORD* allocatedBuffer;
    BYTE* pixelPtr;
    WORD* resultBuffer;
    char* tempBuffer;
    char* alignedBuffer;
    unsigned int originalWidth;
    size_t bmpHeaderInfo[13];

    resultBuffer = NULL;
    currentSource = sourceBuffer;
    tempBuffer = NULL;
    alignedBuffer = NULL;

    if (sourceBuffer && width && height && paletteData && paletteSize && flipFlag <= 1 && modifyZeroFlag <= 1)
    {
        originalWidth = width;
        if ((width & 3) != 0)
        {
            width = width - (width & 3) + 4;
            alignedBuffer = (char*)malloc(width * height);
            if (alignedBuffer == NULL) {
                return NULL;
            }
            memset(alignedBuffer, 0, width * height);
            alignedDestination = alignedBuffer;
            if (height > 0)
            {
                rowCount = height;
                do
                {
                    memcpy(alignedDestination, currentSource, originalWidth);
                    currentSource += originalWidth;
                    alignedDestination += width;
                    --rowCount;
                } while (rowCount);
            }
            currentSource = alignedBuffer;
        }
        if (flipFlag == 1)
        {
            tempBuffer = (char*)malloc(width * height);
            if (tempBuffer == NULL) {
                free(alignedBuffer);
                return NULL;
            }
            memcpy(tempBuffer, currentSource, width * height);
            if (flipImageVertically(tempBuffer, width, height)) {
                goto CLEANUP;
            }
            currentSource = tempBuffer;
        }
        remainingPixels = width * height;
        bmpHeaderInfo[1] = 0;
        bmpHeaderInfo[3] = 40;
        bmpHeaderInfo[2] = 4 * paletteSize + 54;
        bmpHeaderInfo[4] = width;
        bmpHeaderInfo[5] = height;
        memset(&bmpHeaderInfo[7], 0, 16);
        bmpHeaderInfo[12] = 0;
        bmpHeaderInfo[6] = 524289;
        bmpHeaderInfo[0] = width * height + bmpHeaderInfo[2];
        bmpHeaderInfo[11] = paletteSize;
        allocatedBuffer = (WORD*)malloc(bmpHeaderInfo[0]);
        resultBuffer = allocatedBuffer;
        if (allocatedBuffer)
        {
            memset(allocatedBuffer, 0, bmpHeaderInfo[0]);
            *allocatedBuffer = 19778;
            memcpy(allocatedBuffer + 1, bmpHeaderInfo, 0x34u);
            memcpy(allocatedBuffer + 27, paletteData, 4 * ((4 * paletteSize) >> 2));
            CopyAndManipulateMemoryBlock(allocatedBuffer + 27, paletteSize);
            pixelPtr = (BYTE*)(resultBuffer + bmpHeaderInfo[2]);
            memcpy(resultBuffer + bmpHeaderInfo[2], currentSource, remainingPixels);
            if (colorOffset && remainingPixels > 0)
            {
                do
                {
                    if (modifyZeroFlag || *pixelPtr)
                        *pixelPtr += colorOffset;
                    ++pixelPtr;
                    --remainingPixels;
                } while (remainingPixels);
            }
            if (outputSize)
                *outputSize = (DWORD)bmpHeaderInfo[0];
        }
    }
CLEANUP:
    if (tempBuffer)
        free(tempBuffer);
    if (alignedBuffer)
        free(alignedBuffer);
    return resultBuffer;
}

DWORD WriteBitmapToFile(LPCSTR filePath, LPCVOID buffer, LONG offset, DWORD bufferSize)
{
    if (!filePath || !buffer || bufferSize == 0)
    {
        return 0;
    }

    HANDLE fileHandle = CreateFileA(filePath,
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (fileHandle == INVALID_HANDLE_VALUE)
    {
        return 0;
    }

    DWORD bytesWritten = 0;

    if (LockFile(fileHandle, 0, 0, bufferSize, 0))
    {
        if (offset != 0)
        {
            if (SetFilePointer(fileHandle, offset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
            {
                UnlockFile(fileHandle, 0, 0, bufferSize, 0);
                CloseHandle(fileHandle);
                return 0;
            }
        }

        DWORD bytesWrittenLocal;
        if (WriteFile(fileHandle, buffer, bufferSize, &bytesWrittenLocal, NULL))
        {
            bytesWritten = bytesWrittenLocal;
        }

        UnlockFile(fileHandle, 0, 0, bufferSize, 0);
    }

    CloseHandle(fileHandle);

    return bytesWritten;
}

void processGraphicsAndBitmap(void)
{
    WORD* buffer = NULL;
    char* graphicsBuffer = createGraphicsBuffer(0, 0, 256, 256);
    if (graphicsBuffer)
    {
        PALETTEENTRY* paletteData = getPaletteEntry();
        int paletteSize = 256; // Assuming 256-color palette
        DWORD bufferSize;
        buffer = AllocateBuffer(graphicsBuffer, 256, 256, paletteData,
            paletteSize, -10, 0, &bufferSize, 1);
        if (buffer)
        {
            char fullPath[MAX_PATH];
            if (FindAndLoadBitmap(gameWND, a256ColorBmpFil, fullPath, 0, off_43F2DC))
            {
                char drive[_MAX_DRIVE], dir[_MAX_DIR], fileName[_MAX_FNAME], ext[_MAX_EXT];
                errno_t err = _splitpath_s(fullPath,
                    drive, _MAX_DRIVE,
                    dir, _MAX_DIR,
                    fileName, _MAX_FNAME,
                    ext, _MAX_EXT);
                if (err == 0 && strcmp(ext, ".bmp") == 0)
                {
                    WriteBitmapToFile(fullPath, (LPCVOID)buffer, 0, bufferSize);

                }
            }
        }
        free(graphicsBuffer);
    }
    if (buffer)
    {
     //   free(*buffer);
        free(buffer);
    }
}

int setPixel(uint8_t colorIndex, int x, int y)
{
    // Check if the coordinates are within the screen boundaries
    if (x < 0 || y < 0 || x >= g_maxScreenWidth || y >= g_maxScreenHeight)
    {
        return -1;  // Return error code if coordinates are out of bounds
    }

    // Calculate the offset in the screen buffer
    int offset = x + g_maxScreenWidth * y;

    // Set the pixel color
    g_screenBuffer[offset] = colorIndex;

    return 0;  // Return success
}

PALETTEENTRY* getPaletteEntry(void)
{
    return (g_globalPalette == NULL) ? NULL : g_gamePaletteEntries;
}

HPALETTE createDefaultPalette(void)
{
    LOGPALETTE* logPalette = (LOGPALETTE*)malloc(LOGPALETTE_SIZE);
    if (!logPalette)
    {
        return NULL;
    }

    memset(logPalette, 0, LOGPALETTE_SIZE);
    logPalette->palVersion = 0x300;  // Windows palette version (768 in hexadecimal)
    logPalette->palNumEntries = PALETTE_SIZE;

    for (int i = 0; i < PALETTE_SIZE; i++)
    {
        logPalette->palPalEntry[i].peRed = 0;
        logPalette->palPalEntry[i].peGreen = 0;
        logPalette->palPalEntry[i].peBlue = 0;
        logPalette->palPalEntry[i].peFlags = PC_NOCOLLAPSE;
    }

    if (!g_globalPalette)
    {
        g_globalPalette = CreatePalette(logPalette);
        if (!g_globalPalette)
        {
            free(logPalette);
            return NULL;
        }
    }

    free(logPalette);
    return g_globalPalette;
}

int updateColorInformation(void)
{
    LPDIRECTDRAWPALETTE paletteHandle = NULL;
    PALETTEENTRY colorTable1[10];  // 40 bytes / 4 bytes per entry = 10 entries
    PALETTEENTRY colorTable2[246]; // 984 bytes / 4 bytes per entry = 246 entries

    if (g_resourceHandlerState != 3)
    {
        return 0;
    }

    PALETTEENTRY* bufferPointer = getPaletteEntry();
    if (bufferPointer == NULL)
    {
        return -1;
    }

    memcpy(colorTable1, bufferPointer, COLOR_TABLE_1_SIZE);
    memcpy(colorTable2, bufferPointer, COLOR_TABLE_2_SIZE);

    HRESULT result = IDirectDraw_CreatePalette(g_directDrawObject,
        DDPCAPS_8BIT | DDPCAPS_ALLOW256,
        colorTable1,
        &paletteHandle,
        NULL);
    if (FAILED(result))
    {
        return -1;
    }

    // Assuming g_graphicsInterface is a pointer to an object with a vtable
    void (**vtable)(void*, LPDIRECTDRAWPALETTE) = (void (**)(void*, LPDIRECTDRAWPALETTE))(*(void***)g_graphicsInterface);
    vtable[31](g_graphicsInterface, paletteHandle);  // Call the 31st function (124 / 4 = 31)

    IDirectDrawPalette_Release(paletteHandle);

    return 0;
}


HPALETTE handleGlobalPalette(HPALETTE palette)
{
    if (!palette)
    {
        palette = createDefaultPalette();
    }

    if (palette)
    {
        g_globalPalette = palette;
    }

    memset(g_gamePaletteEntries, 0, PALETTE_ENTRIES_SIZE);

    for (int i = 0; i < PALETTE_SIZE; i++)
    {
        g_gamePaletteEntries[i].peFlags = PC_NOCOLLAPSE;
    }

    return g_globalPalette;
}

int GetBitmapBufferDetails(void* bitmapHandle, DWORD* width, DWORD* height) {
    if (!bitmapHandle) {
        return 0;
    }

    // Ensure bitmapHandle is a valid pointer before dereferencing
    if (IsBadReadPtr(bitmapHandle, sizeof(int))) {
        return 0;
    }

    int colorTableSize = 0;
    GetBitmapColorTableSize((void*)bitmapHandle, &colorTableSize);  // Assuming GetBitmapColorTableSize takes an int

    // Check again to ensure colorTableSize is valid
    if (colorTableSize < 0) {
        return 0;
    }

    uint8_t* bitmapPtr = (uint8_t*)bitmapHandle;
    uintptr_t bufferDetails = (uintptr_t)bitmapPtr + 14 + *(int*)(bitmapPtr + 14) + 4 * colorTableSize;

    // Ensure bufferDetailsPtr is a valid memory address
    if (IsBadReadPtr((void*)bufferDetails, sizeof(int))) {
        return 0;
    }

    if (width) {
        *width = *(DWORD*)(bitmapPtr + 18);
    }

    if (height) {
        *height = *(DWORD*)(bitmapPtr + 22);
    }

    return (int)bufferDetails; // Returning as int, although it is not strictly safe on all platforms
}

int flipImageVertically(char* imageData, int width, int height)
{
    int remainingRows;
    char* tempBuffer;
    char* sourceRow;
    char* destRow;
    char* allocatedBuffer;

    if (!imageData || !width || !height)
        return -1;

    tempBuffer = (char*)malloc(width * height);
    if (!tempBuffer)
        return -1;

    allocatedBuffer = tempBuffer;
    remainingRows = height;

    if (height > 0)
    {
        sourceRow = &imageData[width * (height - 1)];
        do
        {
            destRow = sourceRow;
            sourceRow -= width;
            memcpy(allocatedBuffer, destRow, width);
            allocatedBuffer += width;
            --remainingRows;
        } while (remainingRows);
    }

    memcpy(imageData, tempBuffer, width * height);
    free(tempBuffer);

    return 0;
}

HPALETTE CreateCustomPalette(intptr_t colorTableSize, int numEntries)
{
    int remainingEntries;
    LOGPALETTE* tempPalette;
    BYTE* palEntry;
    HPALETTE createdPalette;

    if (!colorTableSize || !numEntries)
        return NULL;

    tempPalette = (LOGPALETTE*)malloc(sizeof(LOGPALETTE) + numEntries * sizeof(PALETTEENTRY));
    if (!tempPalette)
        return NULL;

    tempPalette->palVersion = 0x300;
    tempPalette->palNumEntries = numEntries;
    remainingEntries = numEntries;

    if (numEntries > 0)
    {
        palEntry = (BYTE*)&tempPalette->palPalEntry[0];
        do
        {
            palEntry[0] = *(BYTE*)(colorTableSize + 2); // Red
            palEntry[1] = *(BYTE*)(colorTableSize + 1); // Green
            palEntry[2] = *(BYTE*)colorTableSize;       // Blue
            palEntry[3] = 0;                                // Reserved

            colorTableSize += 4;
            palEntry += 4;
            --remainingEntries;
        } while (remainingEntries);
    }

    createdPalette = CreatePalette(tempPalette);
    free(tempPalette);

    return createdPalette;
}


BYTE* __cdecl ProcessBitmapData(
    void* bitmapHandle,
    HBITMAP* outBitmap,
    DWORD* outColorCount,
    int* outWidth,
    int* outHeight)
{
    BYTE* buffer; // edi
    BYTE* pixelPtr; // ecx
    int index; // edx
    HBITMAP newBitmap; // eax
    int imgHeight; // [esp+Ch] [ebp-8h] BYREF
    int imgWidth; // [esp+10h] [ebp-4h] BYREF

    if (!bitmapHandle)
        return 0;

    if (outColorCount)
        *outColorCount = GetBitmapPalette((void*)bitmapHandle);

    flipBitmapVertically((void*)bitmapHandle);
    buffer = GetBitmapBufferDetails((intptr_t)bitmapHandle, &imgWidth, &imgHeight);
    if (!buffer)
        return 0;

    if (outWidth)
        *outWidth = imgWidth;
    if (outHeight)
        *outHeight = imgHeight;

    if (*(int*)((char*)bitmapHandle + 28) <= 8u)
    {
        pixelPtr = buffer;
        for (index = 0; imgWidth * imgHeight > index; ++index)
            *pixelPtr++ += 10;
    }

    if (!outBitmap)
        return buffer;

    newBitmap = CreateBitmap(imgWidth, imgHeight, 1u, *(int*)((char*)bitmapHandle + 28), buffer);
    *outBitmap = newBitmap;

    if (newBitmap)
        return buffer;
    else
        return 0;
}

int __cdecl flipBitmapVertically(int bitmapHandle)
{
    int bufferDetails; // edi
    unsigned int adjustedWidth; // eax
    unsigned int bitmapWidth; // [esp+Ch] [ebp-8h] BYREF
    int bitmapHeight; // [esp+10h] [ebp-4h] BYREF

    if (!bitmapHandle)
        return -1;

    bufferDetails = GetBitmapBufferDetails(bitmapHandle, &bitmapWidth, &bitmapHeight);
    if (!bufferDetails)
        return -1;

    bitmapWidth = (bitmapWidth * *(int*)(bitmapHandle + 28)) / 8
        - ((((bitmapWidth * *(int*)(bitmapHandle + 28)) >> 31) ^ abs(bitmapWidth * *(int*)(bitmapHandle + 28)) & 7) == (bitmapWidth * *(int*)(bitmapHandle + 28)) >> 31)
        + 1;
    adjustedWidth = bitmapWidth >> 2;
    if ((bitmapWidth & 3) != 0)
        ++adjustedWidth;

    return -(flipImageVertically(bufferDetails, 4 * adjustedWidth, bitmapHeight) == -1);
}

HPALETTE __cdecl GetBitmapPalette(int bitmapHandle)
{
    int colorTableSize; // eax
    int numEntries; // [esp+0h] [ebp-4h] BYREF

    numEntries = 0;
    if (!bitmapHandle)
        return 0;

    colorTableSize = GetBitmapColorTableSize(bitmapHandle, &numEntries);
    if (colorTableSize && numEntries)
        return CreateCustomPalette(colorTableSize, numEntries);
    else
        return 0;
}


int __cdecl GetBitmapColorTableSize(int bitmapHandle, int* numEntries) {
    if (numEntries) {
        *numEntries = 0;
    }

    if (bitmapHandle <= 0) {
        // Invalid bitmap handle
        printf("GetBitmapColorTableSize: Invalid bitmap handle (<= 0).\n");
        return 0;
    }

    __try {
        int colorTableSize = *(int*)(bitmapHandle + 46);

        printf("GetBitmapColorTableSize: Initial colorTableSize = %d\n", colorTableSize);

        if (colorTableSize == 0) {
            int bitsPerPixel = *(int*)(bitmapHandle + 28);

            printf("GetBitmapColorTableSize: bitsPerPixel = %d\n", bitsPerPixel);

            switch (bitsPerPixel) {
            case 1:
                colorTableSize = 2;
                break;
            case 4:
                colorTableSize = 16;
                break;
            case 8:
                colorTableSize = 256;
                break;
            default:
                colorTableSize = 0;
                break;
            }

            if (colorTableSize == 0) {
                return 0;
            }
        }

        int colorTableOffset = bitmapHandle + 54;

        // Additional check to ensure we don't return an invalid offset
        if (colorTableOffset < 0) {
            colorTableSize = 0;
        }

        if (numEntries) {
            *numEntries = colorTableSize;
        }

        printf("GetBitmapColorTableSize: colorTableSize = %d, colorTableOffset = %d\n", colorTableSize, colorTableOffset);

        return colorTableOffset;
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        // Handle the access violation
        printf("GetBitmapColorTableSize: Exception occurred while accessing memory at bitmapHandle.\n");
        return 0;
    }
}

char* createGraphicsBuffer(int x, int y, int width, int height)
{
    if (isGraphicsInitialized())
    {
        return NULL;
    }

    if (isRectangleWithinScreen(x, y, width, height))
    {
        return NULL;
    }

    char* buffer = (char*)malloc(width * height);
    if (!buffer)
    {
        return NULL;
    }

    memset(buffer, 0, width * height);

    for (int currentY = y; currentY < y + height; currentY++)
    {
        if (currentY < 0 || currentY >= g_maxScreenHeight)
        {
            continue;
        }

        char* screenRow = g_bitDepth + g_maxScreenWidth * currentY;
        char* bufferRow = buffer + (currentY - y) * width;

        int startX = (x < 0) ? -x : 0;
        int endX = (x + width > g_maxScreenWidth) ? (g_maxScreenWidth - x) : width;

        if (endX <= 0)
        {
            continue;
        }

        memcpy(bufferRow + startX, screenRow + x + startX, endX - startX);
    }

    return buffer;
}

int isGraphicsInitialized(void)
{
    if (g_maxScreenWidth && g_maxScreenHeight && g_bitDepth &&
        g_screenBuffer && g_globalGraphicsBuffer)
    {
        return 0;  // Initialized
    }
    else
    {
        return -1;  // Not initialized
    }
}



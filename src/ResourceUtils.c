
#include "ResourceUtils.h"
#include <stddef.h>
#include <string.h>
#include <Windows.h>
#include "reg.h"
#include "bitmap.h"
#include "anim.h"


// Define the external variables
void* GlobalResourceArray = NULL;
int resourceArraySize = 0;
void* dword_4C0770 = NULL;
int dword_4C0774 = 0;
void* dword_4C077C = NULL;
int dword_4C0780 = 0;

int32_t** g_globalResourceArray = NULL;
uint32_t g_resourceArraySize = 0;
int g_currentGameStateVariable = 0;
int g_currentResourceSize = 0;


char byte_43F1BC;
extern void* g_resourceManager;
void* dword_439854;
extern int* g_screenPosHorizontal;
extern int g_screenResolutionFlag;
extern int g_isFullscreenReg;
extern HWND gameWND; // Updated to use gameWND
 


void* g_resourceManager = NULL;
unsigned char g_initializationCounter = 0;
int* g_screenPosHorizontal = 0;
int g_screenResolutionFlag = 0;
void* g_renderingInterface = NULL;
HWND g_parentWindow = NULL;


#define MAX_RENDER_OBJECTS 100  // Adjust this value as needed
#define RENDER_DATA_SIZE 44     // Size of each render data entry in bytes

#define GRAPHICS_DATA_SIZE 27
#define GRAPHICS_INTERFACE_INIT_FUNC_OFFSET 100
#define GRAPHICS_INTERFACE_FINALIZE_FUNC_OFFSET 128
#define BUFFER_DATA_SIZE 16

typedef void (*GraphicsInterfaceFunc)(void*, int, int*, int, int);
typedef int (*GraphicsInterfaceFinalizeFunc)(void*, int);

typedef void (*RenderingInterfaceFunc)(void*, int, int, void*, void*, int);

extern void* g_graphicsInterface;
extern int g_resourceHandlerState;

typedef int (*GraphicsInterfaceFinalizeFunc)(void*, int);


int __cdecl InitializeBuffer(unsigned int* buffer, int x_start, int y_start, int width, int height)
{
    if (!buffer)
        return -1;
    *buffer = x_start;
    buffer[1] = y_start;
    buffer[2] = width + x_start;
    buffer[3] = height + y_start;
    return 0;
}

int processScreenUpdatesAndResources() {
    int horizontalResourceOffset;
    int remainingRows;
    char* destinationBuffer;
    const void* sourceBuffer;
    POINT screenOffset;
    char tempBuffer1[BUFFER_DATA_SIZE];
    char tempBuffer2[BUFFER_DATA_SIZE];
    int screenUpdateData[GRAPHICS_DATA_SIZE];
    int resourceUpdateData[MAX_RENDER_OBJECTS];

    // Initialize screen update data
    memset(screenUpdateData, 0, sizeof(screenUpdateData));
    screenUpdateData[0] = 108;

    if (g_resourceHandlerState != 2) {
        if (byte_43F1BC < 2u) {
            initializeResourceHandlers();
            ++byte_43F1BC;
        }
        (*(void(*)(void*, int, int*, int, int))(*(uintptr_t*)g_resourceManager + GRAPHICS_INTERFACE_INIT_FUNC_OFFSET))(g_resourceManager, 0, screenUpdateData, 0, 0);
        memcpy(&screenUpdateData[9], g_screenPosHorizontal, 0x10000u);
        (*(void(*)(void*, int))(*(uintptr_t*)g_resourceManager + GRAPHICS_INTERFACE_FINALIZE_FUNC_OFFSET))(g_resourceManager, 0);
        InitializeBuffer((unsigned int*)tempBuffer1, 0, 0, 256, 240);

        if (g_isFullscreenReg) {
            if (!g_screenResolutionFlag) {
                (*(void(*)(void*, int, int, void*, void*, int))(*(uintptr_t*)dword_439854 + 28))(dword_439854, 192, 112, g_resourceManager, tempBuffer1, 16);
                goto APPLY_GRAPHICS_UPDATE;
            }
            InitializeBuffer((unsigned int*)tempBuffer2, 0, 0, 640, 480);
        }
        else {
            if (!g_screenResolutionFlag) {
                (*(void(*)(void*, int, int, void*, void*, int))(*(uintptr_t*)dword_439854 + 28))(dword_439854, 32, 0, g_resourceManager, tempBuffer1, 16);
            APPLY_GRAPHICS_UPDATE:
                (*(void(*)(void*, int, int))(*(uintptr_t*)g_graphicsInterface + 44))(g_graphicsInterface, 0, 1);
                return 0;
            }
            InitializeBuffer((unsigned int*)tempBuffer2, 0, 0, 320, 240);
        }

        // Initialize resource update data
        memset(resourceUpdateData, 0, sizeof(resourceUpdateData));
        resourceUpdateData[0] = 100;
        (*(void(*)(void*, void*, void*, void*, unsigned int, int*))(*(uintptr_t*)dword_439854 + 20))(dword_439854, tempBuffer2, g_resourceManager, tempBuffer1, 0x1000000, resourceUpdateData);
        goto APPLY_GRAPHICS_UPDATE;
    }

    // Screen offset initialization
    screenOffset.y = 0;
    screenOffset.x = 0;
    ClientToScreen(gameWND, &screenOffset);

    (*(void(*)(void*, int, int*, int, int))(*(uintptr_t*)g_graphicsInterface + GRAPHICS_INTERFACE_INIT_FUNC_OFFSET))(g_graphicsInterface, 0, screenUpdateData, 0, 0);

    horizontalResourceOffset = *g_screenPosHorizontal;  // Assuming g_screenPosHorizontal is an int pointer
    remainingRows = 256;
    if (screenUpdateData[4] > 0 && screenOffset.y >= 0 && (uintptr_t)screenOffset.y <= SIZE_MAX / (uintptr_t)screenUpdateData[4] &&
        screenOffset.x >= 0 && (uintptr_t)screenOffset.x <= SIZE_MAX - (uintptr_t)screenUpdateData[9]) {
        destinationBuffer = (char*)((uintptr_t)screenUpdateData[9] + (uintptr_t)screenOffset.x + (uintptr_t)screenUpdateData[4] * (uintptr_t)screenOffset.y);
    }
    else {
        // Handle the overflow case, possibly by returning an error or handling it appropriately
        return -1;
    }
    do {
        sourceBuffer = (const void*)(uintptr_t)horizontalResourceOffset;
        horizontalResourceOffset += 256;
        memcpy(destinationBuffer, sourceBuffer, 0x100u);
        destinationBuffer += screenUpdateData[4];
        --remainingRows;
    } while (remainingRows);

    (*(void(*)(void*, int))(*(uintptr_t*)g_graphicsInterface + GRAPHICS_INTERFACE_FINALIZE_FUNC_OFFSET))(g_graphicsInterface, 0);
    return 0;
}

int initializeResourceHandlers(void) {
    if (g_resourceHandlerState != 3) {
        return -1;
    }

    int graphicsData[GRAPHICS_DATA_SIZE];
    memset(graphicsData, 0, sizeof(graphicsData));
    graphicsData[0] = 108;  // Magic number, purpose unknown

    // Cast g_graphicsInterface to a function pointer type
    GraphicsInterfaceFunc initFunc = (GraphicsInterfaceFunc)((char*)g_graphicsInterface + GRAPHICS_INTERFACE_INIT_FUNC_OFFSET);

    // Call the initialization function
    initFunc(g_graphicsInterface, 0, graphicsData, 0, 0);

    // Clear the graphics buffer
    memset((void*)(uintptr_t)graphicsData[9], 0, graphicsData[3] * graphicsData[2]);

    // Cast g_graphicsInterface to a function pointer type for finalization
    GraphicsInterfaceFinalizeFunc finalizeFunc = (GraphicsInterfaceFinalizeFunc)((char*)g_graphicsInterface + GRAPHICS_INTERFACE_FINALIZE_FUNC_OFFSET);

    // Call the finalization function and return its result
    return finalizeFunc(g_graphicsInterface, 0);
}

int __cdecl CopyAndManipulateMemoryBlock(char* destBuffer, int elementCount)
{
    char* currentDest;
    int remainingElements;
    unsigned char* sourcePtr;
    char tempByte;
    unsigned int bufferSize;
    void* tempBuffer;

    currentDest = destBuffer;

    if (!destBuffer)
        return -1;

    remainingElements = elementCount;

    if (elementCount < 0)
        return -1;

    if (elementCount == 0)
        return 0;

    bufferSize = 4 * elementCount;
    sourcePtr = (unsigned char*)malloc(bufferSize);

    if (!sourcePtr)
        return -1;

    tempBuffer = sourcePtr;
    memcpy(sourcePtr, destBuffer, bufferSize);
    memset(destBuffer, 0, bufferSize);

    do
    {
        tempByte = *sourcePtr;
        sourcePtr += 4;
        currentDest[2] = tempByte;
        currentDest += 4;
        --remainingElements;
        *(currentDest - 3) = *(sourcePtr - 3);
        *(currentDest - 4) = *(sourcePtr - 2);
        *(currentDest - 1) = 0;
    } while (remainingElements);

    free(tempBuffer);
    return 0;
}

void resetGameStateVariable(void)
{
    g_currentGameStateVariable = 0;
}

void cleanupResources(void)
{
    if (g_globalResourceArray && g_resourceArraySize > 0)
    {
        size_t bytesToClear = g_resourceArraySize * sizeof(int32_t*);
        memset(g_globalResourceArray, 0, bytesToClear);
    }

    resetGameStateVariable();
    resetResourceCounter();
}

void resetResourceCounter(void)
{
    g_currentResourceSize = 0;
}

int updatePointerArray(uint32_t index, int32_t* newEntry)
{
    // Check for invalid input parameters
    if (g_globalResourceArray == NULL || g_resourceArraySize == 0 ||
        index >= g_resourceArraySize || newEntry == NULL)
    {
        return -1;
    }

    int32_t** pointerToArrayEntry = g_globalResourceArray + index;

    // If the array entry is not empty, copy its content to the new entry
    if (*pointerToArrayEntry != NULL)
    {
        *newEntry = **pointerToArrayEntry;
    }

    // Update the array entry with the new pointer
    *pointerToArrayEntry = newEntry;

    return 0;
}

int __cdecl ReallocateGlobalResourceArray(int newSize)
{
    void* newArray; // Pointer for the new allocated array

    if (GlobalResourceArray)
        free(GlobalResourceArray);

    resourceArraySize = 0;
    newArray = malloc(4 * newSize);
    if (!newArray)
        return 0;

    GlobalResourceArray = newArray;
    resourceArraySize = newSize;
    memset(newArray, 0, 4 * newSize);
    return newSize;
}

int __cdecl ReallocateRenderBuffer(int newSize)
{
    if (dword_4C0770)
        free(dword_4C0770);
    dword_4C0774 = 0;
    dword_4C0770 = malloc(44 * newSize);
    if (!dword_4C0770)
        return 0;
    dword_4C0774 = newSize;
    memset(dword_4C0770, 0, 44 * newSize);
    return newSize;
}

int __cdecl ReallocateMemoryBuffer(int newSize)
{
    void* newBuffer; // Pointer for the new allocated buffer

    if (dword_4C077C)
        free(dword_4C077C);
    dword_4C0780 = 0;
    newBuffer = malloc(28 * newSize);
    dword_4C077C = newBuffer;
    if (!newBuffer)
        return 0;
    dword_4C0780 = newSize;
    memset(newBuffer, 0, 28 * newSize);
    return newSize;
}

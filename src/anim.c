#include "anim.h"
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include <windows.h>
#include <stdint.h>
#include <stdbool.h>
#include "game.h"

static HWND g_animationWindow = NULL;
static void* g_localBitsBuffer = NULL;
static void* g_globalGraphicsBuffer = NULL;
static DWORD g_bufferSize = 0;
static int g_maxScreenWidth = 0;
static int g_maxScreenHeight = 0;
static int g_bitDepth = 0;
static void* g_bitsBuffer = 0;

int g_totalFrames = 0;
int g_currentFrame = 0;
int g_frameCount = 0;
int g_animationDirection = 1;
bool g_isAnimationReverse = false;
uint8_t* g_screenBuffer = NULL;
void* dword_4C0790 = NULL;

int updateAnimations(void) {
    if (g_screenBuffer == NULL || g_animationDirection == 0) {
        return -1;  // Error: Screen buffer not initialized or animation not active
    }

    if (g_isAnimationReverse) {
        if (g_currentFrame <= 0) {
            disableAnimation();
            return -1;  // Animation complete (reverse)
        }
    }
    else {
        if (g_currentFrame >= g_totalFrames) {
            clearGlobalAnimationControl();
            return -1;  // Animation complete (forward)
        }
    }

    updateAnimationState(2);  // Update animation with grid size 2

    if (g_isAnimationReverse) {
        g_currentFrame--;
    }
    else {
        g_currentFrame++;
    }

    return 0;  // Success
}

int disableAnimation(void) {
    if (g_screenBuffer == NULL) {
        return -1;  // Error: Screen buffer not initialized
    }

    g_animationDirection = 0;
    g_frameCount = 0;
    g_isAnimationReverse = 0;
    g_totalFrames = 0;
    g_currentFrame = 0;

    return 0;  // Success
}

int updateAnimationState(int gridSize) {
    if (g_totalFrames <= 1 || g_currentFrame >= g_totalFrames)
        return -1;

    if (g_currentFrame == 0)
        return -1;

    int squaredGridSize = gridSize * gridSize;
    int pixelsToUpdate;

    if (g_totalFrames >= squaredGridSize) {
        if (g_totalFrames / squaredGridSize) {
            pixelsToUpdate = g_currentFrame / (g_totalFrames / squaredGridSize);
        }
        else {
            if (g_totalFrames - squaredGridSize > g_currentFrame)
                return 0;
            pixelsToUpdate = squaredGridSize + g_currentFrame - g_totalFrames;
        }
        if (pixelsToUpdate >= squaredGridSize)
            return -1;
    }
    else {
        pixelsToUpdate = g_currentFrame * (squaredGridSize / g_totalFrames);
    }

    int gridCountX = (g_maxScreenWidth + gridSize - 1) / gridSize;
    int gridCountY = (g_maxScreenHeight + gridSize - 1) / gridSize;

    for (int gridY = 0; gridY < gridCountY; gridY++) {
        for (int gridX = 0; gridX < gridCountX; gridX++) {
            int pixelCount = 0;
            int x = 0, y = 0;
            int direction = 0;  // 0: right, 1: down, 2: left, 3: up

            while (pixelCount < pixelsToUpdate) {
                int remainingSteps = gridSize - (direction % 2 == 0 ? x : y);

                for (int i = 0; i < remainingSteps && pixelCount < pixelsToUpdate; i++) {
                    setPixel(0, x + gridX * gridSize, y + gridY * gridSize);
                    pixelCount++;

                    switch (direction) {
                    case 0: x++; break;
                    case 1: y++; break;
                    case 2: x--; break;
                    case 3: y--; break;
                    }
                }

                direction = (direction + 1) % 4;
            }
        }
    }

    return 0;
}

int initializeAnimationParameters(int frameCount, bool isReverse, int totalFrames) {
    if (g_screenBuffer == NULL || totalFrames < 2) {
        return -1;  // Error: Invalid parameters
    }

    g_frameCount = frameCount;
    g_totalFrames = totalFrames - 1;
    g_animationDirection = 1;
    g_currentFrame = 0;
    g_isAnimationReverse = isReverse;

    updateAnimationState(2);

    if (g_isAnimationReverse) {
        g_currentFrame = g_totalFrames;
    }
    else {
        g_currentFrame = 1;
    }

    return 0;  // Success
}

void* InitAnimationControl(HWND hwnd, int width, int height, int bitDepth) {
    if (!hwnd || width <= 0 || height <= 0 || bitDepth <= 0) {
        return NULL;
    }

    g_animationWindow = hwnd;

    size_t bytesPerRow = (width * bitDepth / 8 + ((((width * bitDepth) >> 31) ^ abs(width * bitDepth) & 7) != (width * bitDepth) >> 31));
    size_t bufferSize = height * bytesPerRow;

    void* localBuffer = malloc(bufferSize);
    if (!localBuffer) {
        return NULL;
    }

    memset(localBuffer, 0, bufferSize);

    if (g_localBitsBuffer) {
        free(g_localBitsBuffer);
    }
    g_localBitsBuffer = localBuffer;

    void* globalBuffer = malloc(bufferSize);
    if (!globalBuffer) {
        free(localBuffer);
        return NULL;
    }

    memset(globalBuffer, 0, bufferSize);

    if (g_globalGraphicsBuffer) {
        free(g_globalGraphicsBuffer);
    }
    g_globalGraphicsBuffer = globalBuffer;

    g_bufferSize = (DWORD)bufferSize;
    g_maxScreenWidth = width;
    g_maxScreenHeight = height;
    g_bitDepth = bitDepth;

    return g_localBitsBuffer;
}

int isGraphicsSystemInitialized(void) {
    if (g_maxScreenWidth && g_maxScreenHeight && g_bitDepth && g_bitsBuffer && g_globalGraphicsBuffer) {
        return 0;  // Initialized
    }
    else {
        return -1;  // Not initialized
    }
}

int clearGlobalAnimationControl(void) {
    if (isGraphicsSystemInitialized() != 0) {
        return -1;  // Animation system not initialized
    }

    size_t totalBits = (size_t)g_maxScreenWidth * g_maxScreenHeight * g_bitDepth;
    size_t bufferSize = (totalBits + 7) / 8;  // Round up to nearest byte

    if (bufferSize > 0) {
        memset(g_bitsBuffer, 0, bufferSize);
    }

    return 0;  // Success
}

int fillRectColorWrapper(const int* data) {
    if (data == NULL) {
        return -1;  // Error: Invalid input
    }

    unsigned char color = (unsigned char)data[6];
    int startX = data[2];
    int startY = data[3];
    unsigned int width = (unsigned int)data[4];
    int height = data[5];

    return fillRectangleWithColor(color, startX, startY, width, height);
}

int fillRectangleWithColor(unsigned char color, int startX, int startY, unsigned int width, int height) {
    if (isGraphicsSystemInitialized())
        return -1;

    if (isRectangleWithinScreen(startX, startY, width, height))
        return -1;

    for (int currentY = startY; currentY < startY + height; currentY++) {
        if (currentY < 0)
            continue;

        if (currentY >= g_maxScreenHeight)
            return 0;

        char* screenPtr = g_screenBuffer + g_maxScreenWidth * currentY;

        int effectiveStartX = (startX < 0) ? 0 : startX;
        screenPtr += effectiveStartX;

        unsigned int copyWidth;
        if (startX < 0) {
            copyWidth = (unsigned int)startX + width;
            if (copyWidth <= 0)
                return 0;
        }
        else {
            copyWidth = ((unsigned int)(startX)+width > (unsigned int)g_maxScreenWidth) ? (unsigned int)g_maxScreenWidth - (unsigned int)startX : width;
        }

        // Fill the row with color
        memset(screenPtr, color, copyWidth);
    }

    return 0;
}

int renderPixelArea(char* sourceBuffer, char baseColor, int renderMode, int startX, int startY,
    int width, int height, int useDirectMemoryAccess) {
    char* renderSourceBuffer;
    char* destinationPtr;
    char* sourcePtr;
    int rowWidth;
    char sourcePixel;
    char* renderDestination;
    char* sourceBufferPtr;
    int destinationWidth;
    unsigned char* screenPtr;
    int i;
    unsigned char* screenPtrStart;
    bool isStartXNegative;
    unsigned int copyWidth;
    char* temporaryBuffer;
    char* lastRowStart;
    char* currentRowStart;
    int endYCoordinate;
    int totalHeight_1;
    int currentYCoordinate;
    int currentY;
    int currentY_1;
    char* temporaryRow = NULL;

    if (isGraphicsSystemInitialized())
        return -1;
    if (isRectangleWithinScreen(startX, startY, width, height))
        return -1;
    if (!sourceBuffer)
        return -1;

    renderSourceBuffer = sourceBuffer;
    if (renderMode) {
        renderSourceBuffer = (char*)g_bitsBuffer;  // Ensure g_bitsBuffer is cast to char*
        if (width * height > (intptr_t)dword_4C0790) {
            temporaryRow = (char*)malloc(width * height);
            if (!temporaryRow)
                return -1;
            renderSourceBuffer = temporaryRow;
        }
        destinationPtr = renderSourceBuffer;
        if (renderMode == 1) {
            if (height > 0) {
                totalHeight_1 = height;
                temporaryBuffer = &sourceBuffer[width - 1];
                do {
                    sourcePtr = temporaryBuffer;
                    if (width > 0) {
                        rowWidth = width;
                        do {
                            sourcePixel = *sourcePtr--;
                            *destinationPtr++ = sourcePixel;
                            --rowWidth;
                        } while (rowWidth);
                    }
                    temporaryBuffer += width;
                    --totalHeight_1;
                } while (totalHeight_1);
            }
        }
        else if (renderMode == 2) {
            if (height > 0) {
                currentYCoordinate = height;
                lastRowStart = &sourceBuffer[width * (height - 1)];
                do {
                    renderDestination = destinationPtr;
                    destinationPtr += width;
                    memcpy(renderDestination, lastRowStart, width);
                    lastRowStart -= width;
                    --currentYCoordinate;
                } while (currentYCoordinate);
            }
        }
        else if (renderMode == 3 && height > 0) {
            currentRowStart = &sourceBuffer[width * height - 1];
            currentY = height;
            do {
                sourceBufferPtr = currentRowStart;
                if (width > 0) {
                    destinationWidth = width;
                    do {
                        sourcePixel = *sourceBufferPtr--;
                        *destinationPtr++ = sourcePixel;
                        --destinationWidth;
                    } while (destinationWidth);
                }
                currentRowStart -= width;
                --currentY;
            } while (currentY);
        }
    }

    currentY_1 = height + startY;
    endYCoordinate = startY;
    if (useDirectMemoryAccess) {
        if (currentY_1 > startY) {
            do {
                if (endYCoordinate < 0)
                    goto LABEL_68;
                if (endYCoordinate >= g_maxScreenHeight)
                    break;
                screenPtrStart = (unsigned char*)((intptr_t)g_bitDepth + g_maxScreenWidth * endYCoordinate);  // Cast g_bitDepth to appropriate type
                isStartXNegative = startX < 0;
                if (startX > 0) {
                    if (startX < g_maxScreenWidth)
                        screenPtrStart += startX;
                    isStartXNegative = startX < 0;
                }
                if (isStartXNegative) {
                    copyWidth = startX + width;
                    if (startX + width <= 0)
                        break;
                }
                else {
                    copyWidth = width;
                    if ((unsigned int)(startX)+(unsigned int)(width) >= (unsigned int)(g_maxScreenWidth))  // Cast both startX and g_maxScreenWidth to unsigned
                        copyWidth = (unsigned int)(g_maxScreenWidth)-(unsigned int)(startX);
                }
                if (startX < 0)
                    renderSourceBuffer -= startX;
                memcpy(screenPtrStart, renderSourceBuffer, copyWidth);
                if (startX >= 0) {
                LABEL_68:
                    renderSourceBuffer += width;
                }
                else
                    renderSourceBuffer += startX + width;
                ++endYCoordinate;
            } while (currentY_1 > endYCoordinate);
        }
    }
    else if (currentY_1 > startY) {
        do {
            if (endYCoordinate < 0) {
                renderSourceBuffer += width;
                goto SKIP_LABEL;
            }
            if (endYCoordinate >= g_maxScreenHeight)
                break;
            screenPtr = (unsigned char*)((intptr_t)g_bitDepth + g_maxScreenWidth * endYCoordinate);  // Cast g_bitDepth to appropriate type
            if (startX > 0 && startX < g_maxScreenWidth)
                screenPtr += startX;
            for (i = startX; i < startX + width; ++renderSourceBuffer) {
                if (i >= 0) {
                    if (*renderSourceBuffer) {
                        if (i >= g_maxScreenWidth) {
                            renderSourceBuffer = &renderSourceBuffer[startX + width - i];
                            break;
                        }
                        *screenPtr = baseColor + *renderSourceBuffer;
                    }
                    ++screenPtr;
                }
                ++i;
            }
        SKIP_LABEL:
            ++endYCoordinate;
        } while (currentY_1 > endYCoordinate);
    }

    if (temporaryRow)
        free(temporaryRow);
    return 0;
}

int renderPixelAreaWrapper(int32_t* params) {
    if (params == NULL) {
        return -1;  // Error: Invalid input
    }

    return renderPixelArea(
        (char*)(intptr_t)params[2],      // sourceBuffer
        (char)params[7],       // baseColor
        params[8],             // renderMode
        params[3],             // startX
        params[4],             // startY
        params[5],             // width
        params[6],             // height
        params[9]              // useDirectMemoryAccess
    );
}
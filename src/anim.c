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
int dword_4C07C0 = 0;
int dword_4C07C4 = 0;


int dword_4C07B8;
int dword_4C07B4;
int dword_4C07BC;

int __cdecl InitAnimParams(int frameCount, char isReverse, int totalFrames)
{
    if (!g_bitDepth || totalFrames < 2)
        return -1;
    dword_4C07B8 = frameCount;
    dword_4C07C0 = totalFrames - 1;
    dword_4C07B4 = 1;
    dword_4C07C4 = 0;
    dword_4C07BC = isReverse & 1;
    UpdateAnimationState(2);
    if (dword_4C07BC)
        dword_4C07C4 = dword_4C07C0;
    else
        dword_4C07C4 = 1;
    return 0;
}

int InitAnimations()
{
    InitAnimParams(0, 1, 10);
    return 0;
}

int UpdateAnimationState(int pixelSize)
{
    int totalArea; // total area of the pixel size
    int xSegmentCount; // x segment counter
    int ySegmentCount; // y segment counter
    int currentX; // current x position
    int currentY; // current y position
    int tempX; // temporary x position
    int tempY; // temporary y position
    int prevX; // previous x position
    int prevY; // previous y position
    int initialX; // initial x position
    int initialY; // initial y position
    int maxScreenX; // maximum screen X value
    int pixelsToUpdate; // number of pixels to update
    int screenWidthSegments; // width segments of the screen
    int screenHeightSegments; // height segments of the screen
    int screenXCount; // number of segments in the x direction
    int totalPixels; // total number of pixels to process
    int processedPixels; // number of processed pixels

    if (dword_4C07C0 <= 1 || dword_4C07C4 >= dword_4C07C0)
        return -1;
    if (!dword_4C07C4)
        return -1;

    totalArea = pixelSize * pixelSize;
    if (dword_4C07C0 >= totalArea)
    {
        if (dword_4C07C0 / totalArea)
        {
            maxScreenX = dword_4C07C4 / (dword_4C07C0 / totalArea);
        }
        else
        {
            if (dword_4C07C0 - totalArea > dword_4C07C4)
                return 0;
            maxScreenX = totalArea + dword_4C07C4 - dword_4C07C0;
        }
        if (maxScreenX >= totalArea)
            return -1;
    }
    else
    {
        maxScreenX = dword_4C07C4 * (totalArea / dword_4C07C0);
    }

    screenXCount = (((uint64_t)(g_maxScreenWidth / pixelSize) - 1) >> 32) + 1;
    if (((((uint64_t)(g_maxScreenHeight / pixelSize) - 1) >> 32) + 1) > 0)
    {
        screenWidthSegments = 0;
        processedPixels = (((uint64_t)(g_maxScreenHeight / pixelSize) - 1) >> 32) + 1;
        do
        {
            if (screenXCount > 0)
            {
                xSegmentCount = 0;
                totalPixels = screenXCount;
                do
                {
                    ySegmentCount = 0;
                    currentX = 0;
                    currentY = 0;
                    pixelsToUpdate = 0;
                    while (1)
                    {
                        while (1)
                        {
                            screenHeightSegments = pixelSize - pixelsToUpdate;
                            if (pixelSize - pixelsToUpdate <= currentX)
                                break;
                            ++currentY;
                            setPixel(0, currentX + xSegmentCount, ySegmentCount + screenWidthSegments);
                            if (currentY >= maxScreenX)
                                goto LABEL_36;
                            ++currentX;
                        }
                        if (currentY >= maxScreenX)
                            break;
                        tempX = currentX - 1;
                        tempY = ySegmentCount + 1;
                        if (screenHeightSegments > tempY)
                        {
                            do
                            {
                                ++currentY;
                                setPixel(0, tempX + xSegmentCount, tempY + screenWidthSegments);
                                if (currentY >= maxScreenX)
                                    goto LABEL_36;
                            } while (screenHeightSegments > ++tempY);
                        }
                        if (currentY >= maxScreenX)
                            break;
                        prevX = tempY - 1;
                        prevY = tempX - 1;
                        if (pixelsToUpdate <= prevY)
                        {
                            do
                            {
                                ++currentY;
                                setPixel(0, prevY + xSegmentCount, prevX + screenWidthSegments);
                                if (currentY >= maxScreenX)
                                    goto LABEL_36;
                            } while (pixelsToUpdate <= --prevY);
                        }
                        if (currentY >= maxScreenX)
                            break;
                        initialX = prevY + 1;
                        initialY = prevX - 1;
                        if (++pixelsToUpdate <= initialY)
                        {
                            do
                            {
                                ++currentY;
                                setPixel(0, initialX + xSegmentCount, initialY + screenWidthSegments);
                                if (currentY >= maxScreenX)
                                    goto LABEL_36;
                            } while (pixelsToUpdate <= --initialY);
                        }
                        if (currentY >= maxScreenX)
                            break;
                        ySegmentCount = initialY + 1;
                        currentX = initialX + 1;
                    }
                LABEL_36:
                    xSegmentCount += pixelSize;
                    --totalPixels;
                } while (totalPixels);
            }
            screenWidthSegments += pixelSize;
            --processedPixels;
        } while (processedPixels);
    }
    return 0;
}

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

void* __cdecl InitAnimationControl(HWND hwnd, int width, int height, int bitDepth) {
    if (!hwnd || width <= 0 || height <= 0 || bitDepth <= 0) {
        return NULL;
    }

    g_animationWindow = hwnd;

    // Calculate bytesPerRow safely
    size_t bytesPerRow;
    if ((size_t)width > (SIZE_MAX - 7) / (size_t)bitDepth) {
        return NULL; // width * bitDepth would overflow
    }
    bytesPerRow = ((size_t)width * (size_t)bitDepth + 7) / 8;

    // Calculate bufferSize safely
    size_t bufferSize;
    if ((size_t)height > SIZE_MAX / bytesPerRow) {
        return NULL; // height * bytesPerRow would overflow
    }
    bufferSize = (size_t)height * bytesPerRow;

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

int renderPixelArea(char* sourceBuffer, char baseColor, int renderMode, int startX, int startY, int width, int height, int useDirectMemoryAccess) {
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

    if (isGraphicsSystemInitialized()) {
        return -1;
    }
    if (isRectangleWithinScreen(startX, startY, width, height)) {
        return -1;
    }
    if (!sourceBuffer) {
        return -1;
    }

    renderSourceBuffer = sourceBuffer;
    if (renderMode) {
        renderSourceBuffer = (char*)g_bitsBuffer;
        if ((size_t)width > SIZE_MAX / (size_t)height || (size_t)(width * height) > (size_t)dword_4C0790) {
            temporaryRow = (char*)malloc((size_t)width * (size_t)height);
            if (!temporaryRow) {
                return -1;
            }
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
                if (endYCoordinate < 0) {
                    goto LABEL_68;
                }
                if (endYCoordinate >= g_maxScreenHeight) {
                    break;
                }
                if (g_maxScreenWidth > 0 && endYCoordinate > 0 && (unsigned int)endYCoordinate <= SIZE_MAX / (unsigned int)g_maxScreenWidth) {
                    screenPtrStart = (unsigned char*)((intptr_t)g_bitDepth + (size_t)g_maxScreenWidth * (size_t)endYCoordinate); // Cast g_bitDepth to appropriate type
                }
                else {
                    // Handle the overflow case, possibly by returning an error or handling it appropriately
                    return -1;
                }
                isStartXNegative = startX < 0;
                if (startX > 0) {
                    if (startX < g_maxScreenWidth) {
                        screenPtrStart += startX;
                    }
                    isStartXNegative = startX < 0;
                }
                if (isStartXNegative) {
                    copyWidth = (unsigned int)(startX)+(unsigned int)width;
                    if ((int)(startX + width) <= 0) {
                        break;
                    }
                }
                else {
                    copyWidth = (unsigned int)width;
                    if ((unsigned int)(startX)+(unsigned int)width >= (unsigned int)g_maxScreenWidth) { // Cast both startX and g_maxScreenWidth to unsigned
                        copyWidth = (unsigned int)(g_maxScreenWidth)-(unsigned int)(startX);
                    }
                }
                if (startX < 0) {
                    renderSourceBuffer -= startX;
                }
                memcpy(screenPtrStart, renderSourceBuffer, copyWidth);
                if (startX >= 0) {
                LABEL_68:
                    renderSourceBuffer += width;
                }
                else {
                    renderSourceBuffer += startX + width;
                }
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
            if (endYCoordinate >= g_maxScreenHeight) {
                break;
            }
            if (g_maxScreenWidth > 0 && endYCoordinate > 0 && (size_t)endYCoordinate <= SIZE_MAX / (size_t)g_maxScreenWidth) {
                screenPtr = (unsigned char*)((intptr_t)g_bitDepth + (size_t)g_maxScreenWidth * (size_t)endYCoordinate); // Cast g_bitDepth to appropriate type
            }
            else {
                // Handle the overflow case, possibly by returning an error or handling it appropriately
                return -1;
            }
            if (startX > 0 && startX < g_maxScreenWidth) {
                screenPtr += startX;
            }
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

    if (temporaryRow) {
        free(temporaryRow);
    }
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
#include "FileUtils.h"
#include <stdio.h>
#include <ctype.h>
#include <windows.h>
#include <shlobj.h>  // For SHBrowseForFolder and SHGetPathFromIDList
#include <string.h>  // For memset
#include <mbstring.h> // For _mbsdec
#include <stdlib.h>

unsigned char byte_6822E8[0x1000];
extern int* dataStructurePtr;
extern char LIGHTS2_NCD[MAX_PATH_LENGTH];

// Helper function to check if a character is a path separator
static bool IsPathSeparator(char c) {
    return c == '/' || c == '\\';
}

int __cdecl DecompressData(
    int inputSize,
    unsigned __int8* inputData,
    char* decompressedData,
    unsigned int decompressStartIndex,
    int decompressLength)
{
    unsigned int bitBuffer = 0;
    unsigned int currentIndex = 0;
    int bufferIndex = 4078;
    unsigned __int8* inputPtr;
    unsigned __int8* inputPtr2;
    char currentByte;
    unsigned __int8* inputPtr3;
    unsigned __int8* inputPtr4;
    unsigned __int8 nextByte;
    int loopCounter;
    char bufferByte;
    __int16 firstByte;
    __int16 combinedBytes;
    int repeatCount;

    memset(byte_6822E8, 0, 0xFECu);
    *(int*)&byte_6822E8[4076] = 0;

    do
    {
        while (1)
        {
        LABEL_2:
            bitBuffer >>= 1;
            if ((bitBuffer & 0x100) == 0)
            {
                inputPtr = inputData++;
                if (--inputSize < 0)
                    return 0;
                bitBuffer = *inputPtr | 0xFF00;
            }
            if ((bitBuffer & 1) == 0)
                break;
            inputPtr2 = inputData++;
            currentByte = *inputPtr2;
            if (--inputSize < 0)
                return 0;
            if (decompressStartIndex <= currentIndex)
                *decompressedData++ = currentByte;
            if (decompressStartIndex + decompressLength == ++currentIndex)
                return 0;
            byte_6822E8[bufferIndex] = currentByte;
            bufferIndex = (bufferIndex + 1) & 0xFFF;
        }
        inputPtr3 = inputData;
        inputData = inputPtr3 + 1;
        firstByte = *inputPtr3;
        inputSize--;
        if (inputSize < 0)
            return 0;
        inputPtr4 = inputData;
        inputData = inputPtr4 + 1;
        nextByte = *inputPtr4;
        inputSize--;
        if (inputSize < 0)
            return 0;
        repeatCount = (nextByte & 0xF) + 2;
        combinedBytes = (16 * (nextByte & 0xF0)) | firstByte;
        loopCounter = 0;
    } while (repeatCount < 0);
    while (1)
    {
        bufferByte = byte_6822E8[(loopCounter + combinedBytes) & 0xFFF];
        if (decompressStartIndex <= currentIndex)
            *decompressedData++ = bufferByte;
        if (++currentIndex == decompressLength + decompressStartIndex)
            return 0;
        byte_6822E8[bufferIndex] = bufferByte;
        bufferIndex = (bufferIndex + 1) & 0xFFF;
        if (++loopCounter > repeatCount)
            goto LABEL_2;
    }
}

void* __cdecl PerformSpecialDataProcessing(
    int sourceParam,
    int operationType,
    int decompressionParam,
    size_t outputStart,
    size_t totalDataSize,
    size_t Size,
    size_t* processedSizeOutput)
{
    size_t v8;
    void* allocatedMemAddr;
    void* v10;

    if (totalDataSize > outputStart)
        return 0;

    v8 = Size;
    if (totalDataSize + Size > outputStart)
        v8 = outputStart - totalDataSize;

    if (processedSizeOutput)
        *processedSizeOutput = 0;

    allocatedMemAddr = malloc(v8);
    v10 = allocatedMemAddr;

    if (!allocatedMemAddr)
        return 0;

    if (HandleDataProcessing(
        sourceParam,
        operationType,
        decompressionParam,
        outputStart,
        allocatedMemAddr,
        totalDataSize,
        v8))
    {
        if (processedSizeOutput)
            *processedSizeOutput = v8;
        return v10;
    }
    else
    {
        free(v10);
        return 0;
    }
}

void convertToUppercaseShiftJIS(unsigned char* inputString)
{
    unsigned char* currentChar;
    unsigned char charValue;

    if (!inputString)
        return;

    for (currentChar = inputString; *currentChar; )
    {
        charValue = *currentChar;
        if (charValue < 0x80)
        {
            // For ASCII characters
            *currentChar = toupper((unsigned char)*currentChar);
            currentChar++;
        }
        else
        {
            // For Shift-JIS characters
            // Shift-JIS: First byte is 0x81-0x9F or 0xE0-0xFC
            if ((charValue >= 0x81 && charValue <= 0x9F) || (charValue >= 0xE0 && charValue <= 0xFC))
            {
                // It's a 2-byte character, skip to next
                currentChar += 2;
            }
            else
            {
                // Unexpected byte, skip it
                currentChar++;
            }
        }
    }
}

void _splitpath(const char* pathInput, char* driveOutput, char* directoryOutput, char* filenameOutput, char* extensionOutput)
{
    const char* pathCursor = pathInput;
    const char* lastSlashPosition = NULL;
    const char* lastDotPosition = NULL;
    size_t pathLength = strlen(pathInput);

    // Handle drive letter
    if (pathLength >= 2 && pathInput[1] == ':') {
        if (driveOutput) {
            strncpy_s(driveOutput, 3, pathInput, 2);
            driveOutput[2] = '\0';
        }
        pathCursor += 2;
    }
    else if (driveOutput) {
        driveOutput[0] = '\0';
    }

    // Find last slash and last dot
    for (const char* p = pathCursor; *p; ++p) {
        if (*p == '/' || *p == '\\') {
            lastSlashPosition = p + 1;
        }
        else if (*p == '.') {
            lastDotPosition = p;
        }
    }

    // Handle directory
    if (lastSlashPosition) {
        if (directoryOutput) {
            size_t dirLength = lastSlashPosition - pathCursor;
            if (dirLength >= MAX_PATH_LENGTH) dirLength = MAX_PATH_LENGTH - 1;
            strncpy_s(directoryOutput, MAX_PATH_LENGTH, pathCursor, dirLength);
            directoryOutput[dirLength] = '\0';
        }
        pathCursor = lastSlashPosition;
    }
    else if (directoryOutput) {
        directoryOutput[0] = '\0';
    }

    // Handle filename and extension
    if (lastDotPosition && lastDotPosition > pathCursor) {
        if (filenameOutput) {
            size_t nameLength = lastDotPosition - pathCursor;
            if (nameLength >= MAX_PATH_LENGTH) nameLength = MAX_PATH_LENGTH - 1;
            strncpy_s(filenameOutput, MAX_PATH_LENGTH, pathCursor, nameLength);
            filenameOutput[nameLength] = '\0';
        }
        if (extensionOutput) {
            strncpy_s(extensionOutput, MAX_PATH_LENGTH, lastDotPosition, _TRUNCATE);
        }
    }
    else {
        if (filenameOutput) {
            strncpy_s(filenameOutput, MAX_PATH_LENGTH, pathCursor, _TRUNCATE);
        }
        if (extensionOutput) {
            extensionOutput[0] = '\0';
        }
    }
}

int FindCDDrive() {
    // this is going to be -1 almost all the time these days
    UINT offset;
    char buf[256];

    for (offset = 0; offset < 0x1a; ++offset) {
        snprintf(buf, sizeof(buf), "%c:\\", (char)(offset + 'A'));
        int drive_type = GetDriveTypeA(buf);
        if (drive_type == 5) {
            return offset;
        }
    }
    return -1;
}

int __cdecl BrowseAndSetFolderPath(HWND windowHandle, TCHAR* folderPathBuffer) {
    BROWSEINFO browseInfo;
    LPITEMIDLIST itemIDList;
    if (!windowHandle || !folderPathBuffer)
        return -1;
    memset(&browseInfo, 0, sizeof(browseInfo));
    browseInfo.hwndOwner = windowHandle;
    browseInfo.lpszTitle = TEXT("Select a folder");
    browseInfo.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
    itemIDList = SHBrowseForFolder(&browseInfo);
    if (!itemIDList)
        return -1;
    if (!SHGetPathFromIDList(itemIDList, folderPathBuffer)) {
        CoTaskMemFree(itemIDList); // Free the memory allocated by the shell
        return -1;
    }
    CoTaskMemFree(itemIDList); // Free the memory allocated by the shell
    return 0;
}

int GetDriveIndex(unsigned char* inputChar)
{
    unsigned char firstChar;
    char currentDirectory[520];

    if (!inputChar)
        return -1;

    firstChar = *inputChar;

    if (firstChar >= 0x80u || firstChar <= 0x20u)
        return -1;

    if (firstChar >= 'a' && firstChar <= 'z')
        return firstChar - 'a';

    if (firstChar >= 'A' && firstChar <= 'Z')
        return firstChar - 'A';

    memset(currentDirectory, 0, sizeof(currentDirectory));

    if (!GetCurrentDirectoryA(0x208u, currentDirectory))
        return -1;

    if (currentDirectory[0] < 33)
        return -1;

    if (currentDirectory[0] >= 'a' && currentDirectory[0] <= 'z')
        return currentDirectory[0] - 'a';

    if (currentDirectory[0] < 'A' || currentDirectory[0] > 'Z')
        return -1;

    return currentDirectory[0] - 'A';
}

int CustomDirectoryProcessing(void)
{
    CHAR Buffer[520];
    if (GetCurrentDirectoryA(0x208u, Buffer))
        return GetDriveIndex((unsigned char*)Buffer);
    else
        return -1;
}

void __cdecl createFullPath(
    char* outputBuffer,
    const char* driveLetter,
    const char* directory,
    const char* filename,
    const char* fileExtension)
{
    char* currentBufferPosition; // esi
    const unsigned __int8* dirCursor; // ecx
    char dirCharacter; // dl
    unsigned int lastDirCharacter; // Corrected type
    const char* filenameCursor; // eax
    char filenameCharacter; // cl
    char fileExtensionCharacter; // cl
    byte* extensionStart; // eax
    char* extensionBufferPosition; // esi
    const char* extensionCursor; // edx
    char extensionCharacter; // cl
    char* extensionDestCursor; // eax

    if (driveLetter)
    {
        currentBufferPosition = outputBuffer;
        if (*driveLetter)
        {
            *outputBuffer = *driveLetter;
            outputBuffer[1] = 58;
            currentBufferPosition = outputBuffer + 2;
        }
    }
    else
    {
        currentBufferPosition = outputBuffer;
    }
    dirCursor = (const unsigned __int8*)directory;
    if (directory && *directory)
    {
        do
        {
            dirCharacter = *dirCursor++;
            *currentBufferPosition++ = dirCharacter;
        } while (*dirCursor);
        lastDirCharacter = _mbsdec((const unsigned char*)directory, dirCursor);
        if (lastDirCharacter != 47 && lastDirCharacter != 92)
            *currentBufferPosition++ = 92;
    }
    filenameCursor = filename;
    if (filename && *filename)
    {
        do
        {
            filenameCharacter = *filenameCursor++;
            *currentBufferPosition++ = filenameCharacter;
        } while (*filenameCursor);
    }
    if (fileExtension)
    {
        if (*fileExtension && *fileExtension != 46)
            *currentBufferPosition++ = 46;
        fileExtensionCharacter = *fileExtension;
        extensionStart = (byte*)currentBufferPosition;
        extensionBufferPosition = currentBufferPosition + 1;
        extensionCursor = fileExtension + 1;
        *extensionStart = *fileExtension;
        if (fileExtensionCharacter)
        {
            do
            {
                extensionCharacter = *extensionCursor;
                extensionDestCursor = extensionBufferPosition++;
                ++extensionCursor;
                *extensionDestCursor = extensionCharacter;
            } while (extensionCharacter);
        }
    }
    else
    {
        *currentBufferPosition = 0;
    }
}

DWORD __stdcall CustomFileOperation(
    LPCSTR lpFileName,
    LPVOID lpBuffer,
    unsigned int lDistanceToMove,
    DWORD nNumberOfBytesToRead)
{
    DWORD bytesRead = 0; // esi
    HANDLE fileHandle; // eax
    void* fileHandleVoid; // edi
    unsigned int fileSize; // eax
    DWORD fileSizeDW; // ebp
    DWORD returnBytesRead; // ebx
    DWORD actualBytesRead; // [esp+10h] [ebp-4h] BYREF

    bytesRead = 0;
    if (!lpFileName || !lpBuffer)
        return 0;
    if (!nNumberOfBytesToRead)
        return 0;
    fileHandle = CreateFileA(lpFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    fileHandleVoid = fileHandle;
    if (fileHandle == INVALID_HANDLE_VALUE)
        return 0;
    fileSize = GetFileSize(fileHandle, NULL);
    fileSizeDW = fileSize;
    if (fileSize == INVALID_FILE_SIZE)
    {
        returnBytesRead = 0;
        goto cleanup;
    }
    if (fileSize < lDistanceToMove)
    {
        returnBytesRead = 0;
        goto cleanup;
    }
    if (!LockFile(fileHandleVoid, 0, 0, fileSize, 0))
    {
        returnBytesRead = 0;
        goto cleanup;
    }
    bytesRead = fileSizeDW;
    if (!lDistanceToMove)
    {
    read_file:
        if (fileSizeDW < nNumberOfBytesToRead)
            nNumberOfBytesToRead = fileSizeDW;
        returnBytesRead = 0;
        actualBytesRead = 0;
        if (ReadFile(fileHandleVoid, lpBuffer, nNumberOfBytesToRead, &actualBytesRead, NULL))
            returnBytesRead = actualBytesRead;
        goto cleanup;
    }
    if (SetFilePointer(fileHandleVoid, lDistanceToMove, NULL, FILE_BEGIN) != INVALID_SET_FILE_POINTER)
    {
        fileSizeDW -= lDistanceToMove;
        goto read_file;
    }
    returnBytesRead = 0;
cleanup:
    if (bytesRead && !UnlockFile(fileHandleVoid, 0, 0, bytesRead, 0))
        returnBytesRead = 0;
    if (!CloseHandle(fileHandleVoid))
        return 0;
    return returnBytesRead;
}

int __cdecl CustomDataCopy(LPCSTR fileName, int dataPointer)
{
    char buffer[32]; // Temporary buffer for file data

    if (!fileName || !dataPointer)
        return -1;
    if (!CustomFileOperation(fileName, buffer, 0, 32))
        return -1;
    dataStructurePtr = dataPointer;
    memcpy(unk_67A1A8, buffer, 32);
    strcpy_s(LIGHTS2_NCD, MAX_PATH_LENGTH, fileName);
    return 0;
}

int __cdecl HandleDataProcessing(
    unsigned __int8* sourceData,
    int dataType,
    int decompressionType,
    size_t dataSize,
    void* destination,
    unsigned int offset,
    int length)
{
    int processedLength;
    unsigned int availableDataSize;
    char* tempBuffer;

    if (!sourceData || !destination) {
        return 0;
    }

    if (offset > dataSize) {
        return 0;
    }

    availableDataSize = length;
    if (length + offset > dataSize) {
        availableDataSize = dataSize - offset;
    }

    if (dataType) {
        if (dataType == 2) {
            tempBuffer = malloc(dataSize);
            if (tempBuffer) {
                DecompressData(decompressionType, sourceData, tempBuffer, 0, dataSize);
                memcpy(destination, &tempBuffer[offset], availableDataSize);
                free(tempBuffer);
                return availableDataSize;
            }
            else {
                return 0;
            }
        }
        else {
            return 0;
        }
    }
    else {
        processedLength = availableDataSize;
        memcpy(destination, &sourceData[offset], availableDataSize);
    }

    return processedLength;
}

void* ProcessAndFindMatchingEntry(const char* fileName, unsigned int fileOffset, size_t Size, size_t* processedSize) {
    int* pDatastructure;
    void* PortionOfFile;
    void* pResult;
    char UpperCaseFileName[260];

    if (!fileName || !LIGHTS2_NCD || !dataStructurePtr)
        return NULL;

    if (processedSize)
        *processedSize = 0;

    pDatastructure = dataStructurePtr;
    strncpy_s(UpperCaseFileName, sizeof(UpperCaseFileName), fileName, _TRUNCATE);
    convertToUppercaseShiftJIS((unsigned char*)UpperCaseFileName);

    if (*pDatastructure != -1) {
        while (pDatastructure[1] != -1 && pDatastructure[2] != -1 && pDatastructure[3] != -1) {
            if (strcmp(UpperCaseFileName, (const char*)pDatastructure[4]) == 0) {
                if (pDatastructure[3] < fileOffset)
                    return NULL;

                // Correctly cast LIGHTS2_NCD to LPCSTR
                PortionOfFile = ReadPortionOfFile((LPCSTR)LIGHTS2_NCD, *pDatastructure, pDatastructure[2], 0);
                if (!PortionOfFile)
                    return NULL;

                pResult = PerformSpecialDataProcessing(
                    (int)PortionOfFile,
                    pDatastructure[1],
                    pDatastructure[2],
                    fileOffset,
                    pDatastructure[3],
                    Size,
                    processedSize);

                free(PortionOfFile);
                return pResult;
            }
            pDatastructure += 5;
            if (*pDatastructure == -1)
                return NULL;
        }
    }
    return NULL;
}
void* __stdcall ReadPortionOfFile(
    LPCSTR lpFileName,
    unsigned int lDistanceToMove,
    DWORD nNumberOfBytesToRead,
    DWORD* lpNumberOfBytesRead)
{
    void* pBuffer = 0;
    DWORD fileSize = 0;
    DWORD bytesRead = 0;

    if (!lpFileName || !nNumberOfBytesToRead) {
        return 0;
    }

    HANDLE hFile = CreateFileA(lpFileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (hFile == INVALID_HANDLE_VALUE) {
        return 0;
    }

    DWORD fileSizeActual = GetFileSize(hFile, 0);
    if (fileSizeActual == INVALID_FILE_SIZE || lDistanceToMove > fileSizeActual) {
        CloseHandle(hFile);
        return 0;
    }

    if (!LockFile(hFile, 0, 0, fileSizeActual, 0)) {
        CloseHandle(hFile);
        return 0;
    }

    if (lDistanceToMove > 0) {
        if (SetFilePointer(hFile, lDistanceToMove, 0, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
            UnlockFile(hFile, 0, 0, fileSizeActual, 0);
            CloseHandle(hFile);
            return 0;
        }
    }

    DWORD remainingBytes = fileSizeActual - lDistanceToMove;
    if (nNumberOfBytesToRead > remainingBytes) {
        nNumberOfBytesToRead = remainingBytes;
    }

    void* pAllocatedBuffer = malloc(nNumberOfBytesToRead);
    if (!pAllocatedBuffer) {
        UnlockFile(hFile, 0, 0, fileSizeActual, 0);
        CloseHandle(hFile);
        return 0;
    }

    if (!ReadFile(hFile, pAllocatedBuffer, nNumberOfBytesToRead, &bytesRead, 0) || nNumberOfBytesToRead != bytesRead) {
        free(pAllocatedBuffer);
        pAllocatedBuffer = 0;
    }

    if (!UnlockFile(hFile, 0, 0, fileSizeActual, 0)) {
        if (pAllocatedBuffer) {
            free(pAllocatedBuffer);
        }
        pAllocatedBuffer = 0;
    }

    CloseHandle(hFile);

    if (!pAllocatedBuffer && pBuffer) {
        free(pBuffer);
        pBuffer = 0;
    }

    if (lpNumberOfBytesRead) {
        *lpNumberOfBytesRead = bytesRead;
    }

    return pAllocatedBuffer;
}
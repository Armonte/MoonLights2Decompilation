#pragma once
// fileutils.h


#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <stdbool.h>
#include <windows.h>
#include <stddef.h>  // For size_t

extern int cbMultiByte;
extern unsigned char* off_4C0858;
#define MAX_PATH_LENGTH 260

int* dataStructurePtr;
char unk_67A1A8[32];
char LIGHTS2_NCD[MAX_PATH_LENGTH];
extern int cbMultiByte;
extern unsigned char* off_4C0858;

// fileutils.h
#ifdef _WIN32
#ifdef BUILDING_DLL
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif
#else
#define DLL_EXPORT
#endif

#endif
int __cdecl DecompressData(
    int inputSize,
    unsigned __int8* inputData,
    char* decompressedData,
    unsigned int decompressStartIndex,
    int decompressLength);

void* __cdecl PerformSpecialDataProcessing(
    unsigned __int8* sourceParam,
    int operationType,
    int decompressionParam,
    size_t outputStart,
    size_t totalDataSize,
    size_t Size,
    size_t* processedSizeOutput);

void* __stdcall ReadPortionOfFile(
    LPCSTR lpFileName,
    unsigned int lDistanceToMove,
    DWORD nNumberOfBytesToRead,
    DWORD* lpNumberOfBytesRead);

int __cdecl HandleDataProcessing(
    unsigned __int8* sourceParam,
    int dataType,
    int decompressionType,
    size_t dataSize,
    void* destination,
    unsigned int offset,
    int length);

void* ProcessAndFindMatchingEntry(const char* fileName, unsigned int fileOffset, size_t Size, size_t* processedSize);

void* __stdcall ReadPortionOfFile(LPCSTR lpFileName, unsigned int lDistanceToMove, DWORD nNumberOfBytesToRead, DWORD* lpNumberOfBytesRead);

void* PerformSpecialDataProcessing(unsigned __int8* sourceParam, int operationType, int decompressionParam, size_t outputStart, size_t totalDataSize, size_t Size, size_t* processedSizeOutput);

void ReadFromFile();

void WriteToFile();

void LoadFile();

//@FUN_0042b5c0
int FindCDDrive();

void convertToUppercaseShiftJIS(unsigned char* inputString);

int BrowseAndSetFolderPath();
//void split_path(const char* pathInput, char* driveOutput, char* directoryOutput, char* filenameOutput, char* extensionOutput);
int GetDriveIndex(unsigned char* inputChar);

int CustomDirectoryProcessing(void);

void __cdecl createFullPath(
    char* outputBuffer,
    const char* driveLetter,
    const char* directory,
    const char* filename,
    const char* fileExtension);

void __cdecl createFullPath(
    char* outputBuffer,
    const char* driveLetter,
    const char* directory,
    const char* filename,
    const char* fileExtension);

DWORD __stdcall CustomFileOperation(
    LPCSTR lpFileName,
    LPVOID lpBuffer,
    unsigned int lDistanceToMove,
    DWORD nNumberOfBytesToRead);

int __cdecl CustomDataCopy(LPCSTR fileName, void* dataPointer);



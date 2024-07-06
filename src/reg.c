// reg.c
#include "reg.h"
#include <stdio.h>

#define SETTINGS_KEY "Software\\Morimoto Uchuuichi\\MoonLights3\\settings"
#define BROWSE_KEY "Software\\Morimoto Uchuuichi\\MoonLights3\\browse"
#define SETTINGS_VALUE "setting"
#define BROWSE_VALUE "browse"

int regManCheckLoadSettings(GameSettings* settings, char* FullPath)
{
    HKEY registryKey;
    DWORD dataSize, dataType, keyDisposition;
    LSTATUS status;

    // Check and load settings
    dataSize = sizeof(GameSettings);
    dataType = REG_BINARY;

    status = RegCreateKeyExA(HKEY_CURRENT_USER, SETTINGS_KEY, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &registryKey, &keyDisposition);
    if (status != ERROR_SUCCESS)
        return -1;

    if (keyDisposition == REG_CREATED_NEW_KEY)
    {
        status = RegSetValueExA(registryKey, SETTINGS_VALUE, 0, dataType, (BYTE*)settings, dataSize);
        RegCloseKey(registryKey);
        return (status == ERROR_SUCCESS) ? 0 : -1;
    }
    else
    {
        status = RegQueryValueExA(registryKey, SETTINGS_VALUE, NULL, &dataType, (BYTE*)settings, &dataSize);
        RegCloseKey(registryKey);
        if (status != ERROR_SUCCESS)
            return -1;
    }

    // Check and load browse path
    dataSize = MAX_PATH;
    dataType = REG_SZ;

    status = RegCreateKeyExA(HKEY_CURRENT_USER, BROWSE_KEY, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &registryKey, &keyDisposition);
    if (status != ERROR_SUCCESS)
        return -1;

    if (keyDisposition == REG_CREATED_NEW_KEY)
    {
        status = RegSetValueExA(registryKey, BROWSE_VALUE, 0, dataType, (BYTE*)FullPath, dataSize);
        RegCloseKey(registryKey);
        return (status == ERROR_SUCCESS) ? 0 : -1;
    }
    else
    {
        status = RegQueryValueExA(registryKey, BROWSE_VALUE, NULL, &dataType, (BYTE*)FullPath, &dataSize);
        RegCloseKey(registryKey);
        return (status == ERROR_SUCCESS) ? 0 : -1;
    }
}

int regManSaveSettingsAndPath(GameSettings* settings, const char* FullPath)
{
    HKEY registryKey;
    DWORD disposition;
    LSTATUS status;

    // Save settings
    if (RegCreateKeyExA(HKEY_CURRENT_USER, SETTINGS_KEY, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &registryKey, &disposition) != ERROR_SUCCESS)
        return -1;

    status = RegSetValueExA(registryKey, SETTINGS_VALUE, 0, REG_BINARY, (BYTE*)settings, sizeof(GameSettings));
    RegCloseKey(registryKey);

    if (status != ERROR_SUCCESS)
        return -1;

    // Save browse path
    DWORD fullPathLength = (DWORD)strlen(FullPath) + 1;

    if (RegCreateKeyExA(HKEY_CURRENT_USER, BROWSE_KEY, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &registryKey, &disposition) != ERROR_SUCCESS)
        return -1;

    status = RegSetValueExA(registryKey, BROWSE_VALUE, 0, REG_SZ, (BYTE*)FullPath, fullPathLength);
    RegCloseKey(registryKey);

    return (status == ERROR_SUCCESS) ? 0 : -1;
}
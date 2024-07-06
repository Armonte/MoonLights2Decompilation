#include "audio.h"
#include <stdlib.h>

#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

int CustomAudioControl()
{
    DWORD destinationIndex;
    HANDLE heapHandle;
    MIXERCONTROL* mixerControl;
    UINT controlIndex;
    DWORD* controlTypePointer;
    DWORD* controlStructPointer;
    int controlValue;
    DWORD channelCount;
    int multipleItems;
    HANDLE processHeap;
    int* detailData;
    HANDLE currentHeap;
    // unused HANDLE mixerHandle;
    SIZE_T mixerControlSize;
    SIZE_T detailDataSize;
    HMIXER mixer;
    MIXERLINECONTROLS mixerLineControls;
    MIXERCONTROLDETAILS mixerControlDetails;
    MIXERCAPS mixerCaps;
    MIXERLINE mixerLine;

    mixer = 0;
    if (!mixerGetNumDevs())
        return -1;

    if (mixerOpen(&mixer, 0, 0, 0, 0))
        mixerClose(mixer);

    destinationIndex = 0;
    mixerCaps.cDestinations = 0;
    mixerGetDevCaps(0, &mixerCaps, sizeof(mixerCaps));
    if (!mixerCaps.cDestinations)
        return -1;

    while (1)
    {
        mixerLine.cbStruct = sizeof(mixerLine);
        mixerLine.dwDestination = destinationIndex;
        if (!mixerGetLineInfo((HMIXEROBJ)mixer, &mixerLine, MIXER_GETLINEINFOF_DESTINATION) && mixerLine.dwComponentType == MIXERLINE_COMPONENTTYPE_DST_SPEAKERS)
            break;

        if (mixerCaps.cDestinations <= ++destinationIndex)
            return -1;
    }

    mixerLine.cbStruct = sizeof(mixerLine);
    if (!mixerGetLineInfo((HMIXEROBJ)mixer, &mixerLine, MIXER_GETLINEINFOF_SOURCE) &&
        mixerLine.cControls &&
        (mixerControlSize = sizeof(MIXERCONTROL) * mixerLine.cControls, heapHandle = GetProcessHeap(), (mixerControl = HeapAlloc(heapHandle, HEAP_ZERO_MEMORY, mixerControlSize)) != 0))
    {
        controlIndex = 0;
        mixerLineControls.dwLineID = mixerLine.dwLineID;
        mixerLineControls.dwControlID = 0;
        mixerLineControls.cControls = mixerLine.cControls;
        mixerLineControls.pamxctrl = mixerControl;
        mixerLineControls.cbStruct = sizeof(mixerLineControls);
        mixerLineControls.cbmxctrl = sizeof(MIXERCONTROL);

        if (mixerGetLineControls((HMIXEROBJ)mixer, &mixerLineControls, MIXER_GETLINECONTROLSF_ALL) || !mixerLineControls.cControls)
            goto Cleanup;

        controlTypePointer = &mixerControl->dwControlType;
        while (*controlTypePointer != MIXERCONTROL_CONTROLTYPE_VOLUME)
        {
            controlTypePointer += sizeof(MIXERCONTROL) / sizeof(DWORD);
            if (mixerLineControls.cControls <= ++controlIndex)
                goto Cleanup;
        }

        controlStructPointer = &mixerControl[controlIndex].cbStruct;
        if ((controlStructPointer[2] & MIXERCONTROL_CONTROLF_UNIFORM) == 0)
            goto Cleanup;

        controlValue = controlStructPointer[3];
        if (controlValue < 0)
            goto Cleanup;

        channelCount = mixerLine.cChannels;
        if ((controlValue & MIXERCONTROL_CONTROLF_MULTIPLE) != 0)
            channelCount = 1;

        multipleItems = 1;
        if ((controlValue & MIXERCONTROL_CONTROLF_MULTIPLE) != 0)
            multipleItems = controlStructPointer[4];

        detailDataSize = sizeof(DWORD) * channelCount * multipleItems;
        processHeap = GetProcessHeap();
        detailData = HeapAlloc(processHeap, HEAP_ZERO_MEMORY, detailDataSize);

        if (detailData)
        {
            mixerControlDetails.cbStruct = sizeof(mixerControlDetails);
            mixerControlDetails.dwControlID = controlStructPointer[1];
            mixerControlDetails.cChannels = channelCount;
            mixerControlDetails.cMultipleItems = multipleItems;
            mixerControlDetails.paDetails = detailData;
            mixerControlDetails.cbDetails = sizeof(DWORD);

            if (mixerGetControlDetails((HMIXEROBJ)mixer, &mixerControlDetails, MIXER_GETCONTROLDETAILSF_VALUE))
            {
                currentHeap = GetProcessHeap();
                HeapFree(currentHeap, 0, mixerControl);
                currentHeap = GetProcessHeap();
                HeapFree(currentHeap, 0, detailData);
                mixerClose(mixer);
                return -1;
            }
            else
            {
                controlValue = *detailData;
                currentHeap = GetProcessHeap();
                HeapFree(currentHeap, 0, mixerControl);
                currentHeap = GetProcessHeap();
                HeapFree(currentHeap, 0, detailData);
                mixerClose(mixer);
                return controlValue;
            }
        }
        else
        {
        Cleanup:
            currentHeap = GetProcessHeap();
            HeapFree(currentHeap, 0, mixerControl);
            mixerClose(mixer);
            return -1;
        }
    }
    else
    {
        mixerClose(mixer);
        return -1;
    }
}


int setVolume(unsigned short volume)
{
    HMIXER mixer = NULL;
    MIXERCAPS mixerCaps;
    MIXERLINE mixerLine;
    MIXERLINECONTROLS mixerLineControls;
    MIXERCONTROL* mixerControls = NULL;
    MIXERCONTROLDETAILS mixerControlDetails;
    DWORD* volumeDetails = NULL;

    if (mixerGetNumDevs() == 0)
        return -1;

    if (mixerOpen(&mixer, 0, 0, 0, 0))
    {
        mixerClose(mixer);
        return -1;
    }

    if (mixerGetDevCaps((UINT_PTR)mixer, &mixerCaps, sizeof(MIXERCAPS)) != MMSYSERR_NOERROR)
    {
        mixerClose(mixer);
        return -1;
    }

    for (UINT dest = 0; dest < mixerCaps.cDestinations; dest++)
    {
        mixerLine.cbStruct = sizeof(MIXERLINE);
        mixerLine.dwDestination = dest;

        if (mixerGetLineInfo((HMIXEROBJ)mixer, &mixerLine, MIXER_GETLINEINFOF_DESTINATION) == MMSYSERR_NOERROR &&
            mixerLine.dwComponentType == MIXERLINE_COMPONENTTYPE_DST_SPEAKERS)
        {
            break;
        }

        if (dest == mixerCaps.cDestinations - 1)
        {
            mixerClose(mixer);
            return -1;
        }
    }

    mixerControls = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(MIXERCONTROL) * mixerLine.cControls);
    if (!mixerControls)
    {
        mixerClose(mixer);
        return -1;
    }

    mixerLineControls.cbStruct = sizeof(MIXERLINECONTROLS);
    mixerLineControls.dwLineID = mixerLine.dwLineID;
    mixerLineControls.cControls = mixerLine.cControls;
    mixerLineControls.pamxctrl = mixerControls;
    mixerLineControls.cbmxctrl = sizeof(MIXERCONTROL);

    if (mixerGetLineControls((HMIXEROBJ)mixer, &mixerLineControls, MIXER_GETLINECONTROLSF_ALL) != MMSYSERR_NOERROR)
    {
        HeapFree(GetProcessHeap(), 0, mixerControls);
        mixerClose(mixer);
        return -1;
    }

    MIXERCONTROL* volumeControl = NULL;
    for (UINT i = 0; i < mixerLineControls.cControls; i++)
    {
        if (mixerControls[i].dwControlType == MIXERCONTROL_CONTROLTYPE_VOLUME)
        {
            volumeControl = &mixerControls[i];
            break;
        }
    }

    if (!volumeControl)
    {
        HeapFree(GetProcessHeap(), 0, mixerControls);
        mixerClose(mixer);
        return -1;
    }

    DWORD channelCount = (volumeControl->fdwControl & MIXERCONTROL_CONTROLF_UNIFORM) ? 1 : mixerLine.cChannels;
    DWORD valuesCount = channelCount * (volumeControl->cMultipleItems ? volumeControl->cMultipleItems : 1);

    volumeDetails = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(DWORD) * valuesCount);
    if (!volumeDetails)
    {
        HeapFree(GetProcessHeap(), 0, mixerControls);
        mixerClose(mixer);
        return -1;
    }

    mixerControlDetails.cbStruct = sizeof(MIXERCONTROLDETAILS);
    mixerControlDetails.dwControlID = volumeControl->dwControlID;
    mixerControlDetails.cChannels = channelCount;
    mixerControlDetails.cMultipleItems = volumeControl->cMultipleItems;
    mixerControlDetails.paDetails = volumeDetails;
    mixerControlDetails.cbDetails = sizeof(DWORD);

    if (mixerGetControlDetails((HMIXEROBJ)mixer, &mixerControlDetails, MIXER_GETCONTROLDETAILSF_VALUE) != MMSYSERR_NOERROR)
    {
        HeapFree(GetProcessHeap(), 0, mixerControls);
        HeapFree(GetProcessHeap(), 0, volumeDetails);
        mixerClose(mixer);
        return -1;
    }

    *volumeDetails = volume;

    MMRESULT result = mixerSetControlDetails((HMIXEROBJ)mixer, &mixerControlDetails, MIXER_SETCONTROLDETAILSF_VALUE);

    HeapFree(GetProcessHeap(), 0, mixerControls);
    HeapFree(GetProcessHeap(), 0, volumeDetails);
    mixerClose(mixer);

    return (result == MMSYSERR_NOERROR) ? 0 : -1;
}
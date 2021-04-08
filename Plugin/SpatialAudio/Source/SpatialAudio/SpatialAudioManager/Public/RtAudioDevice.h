#pragma once

#include "CoreMinimal.h"
#include "SARtAudio/Public/RtAudio.h"
#include "RtAudioDevice.generated.h"


USTRUCT()
struct FRtAudioDevice
{
    GENERATED_BODY()
public:
    
    FRtAudioDevice() = default;
    FRtAudioDevice(const RtAudio::DeviceInfo& deviceInfo);
    
    FString GetDeviceInfoString() const;

    
    UPROPERTY()
    bool Probed;
    
    UPROPERTY()
    FString Name;
    
    UPROPERTY()
    int32 NumOutputChannels;
    UPROPERTY()
    int32 NumInputChannels;
    
    UPROPERTY()
    bool bIsDefaultOutput;
    UPROPERTY()
    bool bIsDefaultInput;
    
    UPROPERTY()
    TArray<int32> SampleRates;
    
    UPROPERTY()
    int32 PreferredSampleRate;
};

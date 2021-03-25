#pragma once

#include "CoreMinimal.h"
#include "IAudioExtensionPlugin.h"

struct FSoundfieldEncoderInputData;


struct FSpatialzerLocationInfo
{
    FSpatialzerLocationInfo()
        : Yaw(0.0f)
        , Pitch(0.0f)
        , Distance(0.0f)
        , bInitialized(false) // location info is invalid if it has not been used yet.;
    {}
    
    void Set(float NewYaw, float NewPitch, float NewDistance)
    {
        this->Yaw = NewYaw;
        this->Pitch = NewPitch;
        this->Distance = NewDistance;
        bInitialized = true;
    }
    
    float Yaw;
    float Pitch;
    float Distance;
    bool bInitialized;
};



class FInfoEncodingSpatialization : public IAudioSpatialization
{
public:
    FInfoEncodingSpatialization();

    /** IAudioSpatialization implementation */
    virtual void Initialize(const FAudioPluginInitializationParams InitializationParams) override;
    virtual void Shutdown() override;
    virtual void OnInitSource(const uint32 SourceId, const FName& AudioComponentUserId, USpatializationPluginSourceSettingsBase* InSettings) override;
    virtual void OnReleaseSource(const uint32 SourceId) override;
    virtual void ProcessAudio(const FAudioPluginSourceInputData& InputData, FAudioPluginSourceOutputData& OutputData);
    
    virtual void OnAllSourcesProcessed() override;
    
    
    TMap<int32, FSpatialzerLocationInfo> SourceLocationInfo = {};
};

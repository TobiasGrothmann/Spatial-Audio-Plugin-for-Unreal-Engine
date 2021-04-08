#include "../Public/InfoEncodingSpatializer.h"
#include "DrawDebugHelpers.h"
#include "AudioDeviceManager.h"
#include "AudioDevice.h"
#include "ISoundfieldFormat.h"
#include "AudioMixerDevice.h"
#include "AudioMixerTypes.h"
#include "SpatialAudio/Public/SpatialAudioModule.h"
#include "SpatialAudio/SpatialAudioManager/Public/SpatialAudioManager.h"
#include "SpatialAudio/Data/Public/SAPacket.h"


FInfoEncodingSpatialization::FInfoEncodingSpatialization()
{
}

void FInfoEncodingSpatialization::Initialize(const FAudioPluginInitializationParams InitializationParams)
{
    SourceLocationInfo.Empty();
}

void FInfoEncodingSpatialization::Shutdown()
{
    SourceLocationInfo.Empty();
}

void FInfoEncodingSpatialization::OnInitSource(const uint32 SourceId, const FName& AudioComponentUserId, USpatializationPluginSourceSettingsBase* InSettings)
{
    if (!SourceLocationInfo.Contains(SourceId))
    {
        SourceLocationInfo.Add(SourceId, FSpatialzerLocationInfo());
    }
}

void FInfoEncodingSpatialization::OnReleaseSource(const uint32 SourceId)
{
    if (SourceLocationInfo.Contains(SourceId))
    {
        SourceLocationInfo.Remove(SourceId);
    }
}

void FInfoEncodingSpatialization::ProcessAudio(const FAudioPluginSourceInputData& Input, FAudioPluginSourceOutputData& Output)
{
    const int NumInChannels = Input.NumChannels;
    const int BufferSize = Input.AudioBuffer->Num() / NumInChannels;
    const int NumOutChannels = Output.AudioBuffer.Num() / BufferSize;
    
    const FSpatializationParams* Params = Input.SpatializationParams;

    
// OUTPUT AUDIO
    const int32 outC = 0; // we only sum all channels to output channel 0
    for (int32 s = 0; s < BufferSize; s++)
    {
        const int OutputIndex = s * NumOutChannels + outC;
        Output.AudioBuffer[OutputIndex] = 0.0f;

        // summing of all input channels
        for (int32 inC = 0; inC < NumInChannels; inC++)
        {
            const int InputIndex = s * NumInChannels + inC;
            Output.AudioBuffer[OutputIndex] += (*Input.AudioBuffer)[InputIndex];
        }
    }
    
// RESIZE FOR EXTRA SAMPLES
    const int32 ExtraSamples = 7;
    const int newOutputSize = BufferSize * NumOutChannels + ExtraSamples;
    Output.AudioBuffer.SetNumUninitialized(newOutputSize);


// ENCODE INFO
    ASpatialAudioManager* SAManager = ASpatialAudioManager::Instance;
    if (!IsValid(SAManager)) return;
    
    const FVector& UsedListenerLocation = SAManager->bFollowListenerLocation ? Params->ListenerPosition : SAManager->SpeakerSetupLocation;
    const FRotator& UsedListenerRotation = SAManager->bFollowListenerRotation ? Params->ListenerOrientation.Rotator() : SAManager->SpeakerSetupRotation;
    
    if (SAManager->bFollowListenerLocation)
    {
        SAManager->SpeakerSetupLocation = UsedListenerLocation;
    }
    if (SAManager->bFollowListenerRotation)
    {
        SAManager->SpeakerSetupRotation = UsedListenerRotation;
    }
    
    const FRotator Direction = (Params->EmitterWorldPosition - UsedListenerLocation).Rotation() - UsedListenerRotation;
    const float Yaw = Direction.Yaw;
    const float Pitch = Direction.Pitch;
    const float Distance = Params->Distance;
    FSpatialzerLocationInfo& LastLocationInfo = SourceLocationInfo[Input.SourceId];
    
    Output.AudioBuffer[Output.AudioBuffer.Num() - ExtraSamples + 0] = Yaw;
    Output.AudioBuffer[Output.AudioBuffer.Num() - ExtraSamples + 1] = Pitch;
    Output.AudioBuffer[Output.AudioBuffer.Num() - ExtraSamples + 2] = Distance;
    Output.AudioBuffer[Output.AudioBuffer.Num() - ExtraSamples + 3] = LastLocationInfo.bInitialized ? 1.0f : 0.0f;
    Output.AudioBuffer[Output.AudioBuffer.Num() - ExtraSamples + 4] = LastLocationInfo.Yaw;
    Output.AudioBuffer[Output.AudioBuffer.Num() - ExtraSamples + 5] = LastLocationInfo.Pitch;
    Output.AudioBuffer[Output.AudioBuffer.Num() - ExtraSamples + 6] = LastLocationInfo.Distance;
    
    LastLocationInfo.Set(Yaw, Pitch, Distance);
}


void FInfoEncodingSpatialization::OnAllSourcesProcessed()
{
}

#include "../Public/SAEncoder.h"
#include "AudioMixerDevice.h"
#include "AudioMixerTypes.h"
#include "SpatialAudio/InfoEncodingSpatializer/Public/InfoEncodingSpatializer.h"
#include "SpatialAudio/Data/Public/SAPacket.h"


void FSAEncoder::Encode(const FSoundfieldEncoderInputData& Input, ISoundfieldAudioPacket& OutputDataUncast)
{
    OutputDataUncast.Reset();
    EncodeAndMixIn(Input, OutputDataUncast);
}




void FSAEncoder::EncodeAndMixIn(const FSoundfieldEncoderInputData& Input, ISoundfieldAudioPacket& OutputDataUncast)
{
    if (Input.AudioBuffer.Num() == 0) return; // this can happen, unsure of when and why
    
    FSAPacket& OutPacket = DowncastSoundfieldRef<FSAPacket>(OutputDataUncast);
    
    const FAudioPlatformSettings& Settings = FAudioPlatformSettings::GetPlatformSettings(FPlatformProperties::GetRuntimeSettingsClassName());
    const int32 SingleChannelBufferSize = Settings.CallbackBufferFrameSize;
    
    const int32 ExpectedExtraSamples = 7;
    
    if (Input.AudioBuffer.Num() != SingleChannelBufferSize * Input.NumChannels &&
        Input.AudioBuffer.Num() != SingleChannelBufferSize * Input.NumChannels + ExpectedExtraSamples)
    {
        UE_LOG(LogSpatialAudio, Error, TEXT("Input AudioBuffer has unexpected length: %i, Expected: %i or %i"), Input.AudioBuffer.Num(), SingleChannelBufferSize * Input.NumChannels, SingleChannelBufferSize * Input.NumChannels + ExpectedExtraSamples);
        return;
    }
    
    ASpatialAudioManager* SAManager = ASpatialAudioManager::Instance;
    if (!IsValid(SAManager))
    {
        UE_LOG(LogSpatialAudio, Error, TEXT("SAManager is invalid."));
        return;
    }
    const int32 NumSpeakers = SAManager->GetNumChannels();
    
    Audio::AlignedFloatBuffer* OutputAudio = new Audio::AlignedFloatBuffer();
    OutputAudio->SetNum(SingleChannelBufferSize);
    const int OnlyInputChanUsed = 0; // spatializer sums input to channel 0 so we only need that
    for (int32 s = 0; s < SingleChannelBufferSize; s++)
    {
        (*OutputAudio)[s] = Input.AudioBuffer[s * Input.NumChannels + OnlyInputChanUsed];
    }
    
    int32 ExtraSamples = Input.AudioBuffer.Num() - Input.NumChannels * SingleChannelBufferSize;
    if (ExtraSamples == 0) // packet contains no spatial information and is played back on all speakers
    {
        TMap<int32, FInterpolatedGain> GainsMap = {};
        const float Gain = 1.0f / sqrt(NumSpeakers);
        for (int32 c = 0; c < NumSpeakers; c++)
        {
            GainsMap.Add(c, { Gain, Gain });
        }
        OutPacket.Sources.Add({ MakeShareable(OutputAudio), GainsMap });
        return;
    }
    else if (ExtraSamples != ExpectedExtraSamples)
    {
        UE_LOG(LogSpatialAudio, Error, TEXT("Trying to encode to custom soundfield but %i (instead of %i or 0) extra samples found."), ExtraSamples, ExpectedExtraSamples);
        delete OutputAudio;
        return;
    }

// DECODE SPATIAL INFO
    const float Azi = Input.AudioBuffer[Input.AudioBuffer.Num() - ExtraSamples + 0];
    const float Ele = Input.AudioBuffer[Input.AudioBuffer.Num() - ExtraSamples + 1];
    const float Dist = Input.AudioBuffer[Input.AudioBuffer.Num() - ExtraSamples + 2];
    const bool bLastLocationInitialized = !FMath::IsNearlyZero(Input.AudioBuffer[Input.AudioBuffer.Num() - ExtraSamples + 3]);
    float LastAzi, LastEle, LastDist;
    if (bLastLocationInitialized)
    {
        LastAzi = Input.AudioBuffer[Input.AudioBuffer.Num() - ExtraSamples + 4];
        LastEle = Input.AudioBuffer[Input.AudioBuffer.Num() - ExtraSamples + 5];
        LastDist = Input.AudioBuffer[Input.AudioBuffer.Num() - ExtraSamples + 6];
    }
    else
    {
        LastAzi = Azi;
        LastEle = Ele;
        LastDist = Dist;
    }

    // compute gains
    if (FSpatialAudioModule::Vbap == nullptr)
    {
        UE_LOG(LogSpatialAudio, Error, TEXT("Vbap Object is invalid. Cannot compute gains."));
        return;
    }
    
    float* Gains = new float[NumSpeakers];
    FSpatialAudioModule::Vbap->Perform(Azi, Ele, Gains);
    
    float* LastGains = new float[NumSpeakers];
    FSpatialAudioModule::Vbap->Perform(LastAzi, LastEle, LastGains);
    
    // TODO: fix the fact that we are performing the vbap calculations twice
    
    TMap<int32, FInterpolatedGain> GainsMap = {};
    for (int32 c = 0; c < NumSpeakers; c++)
    {
        if (!FMath::IsNearlyZero(Gains[c]) || !FMath::IsNearlyZero(LastGains[c]))
        {
            GainsMap.Add(c, { LastGains[c], Gains[c] }); // will be interpolated between
        }
    }
    
    OutPacket.Sources.Add({ MakeShareable(OutputAudio), GainsMap });
    
    delete[] Gains;
    delete[] LastGains;
}



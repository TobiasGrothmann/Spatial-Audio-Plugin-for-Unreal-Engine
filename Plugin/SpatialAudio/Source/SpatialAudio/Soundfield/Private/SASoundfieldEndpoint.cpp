#include "../Public/SASoundfieldEndpoint.h"
#include "Misc/ScopeTryLock.h"
#include "ISoundfieldEndpoint.h"
#include "ISoundfieldFormat.h"
#include "SpatialAudio/Data/Public/SAPacket.h"
#include "../Public/SAEndpointSettings.h"
#include "../Public/SAEncodingSettings.h"
#include "../Public/SAMixer.h"
#include "../Public/SAEncoder.h"
#include "../Public/SADecoder.h"
#include "../Public/SATranscoder.h"
#include "SpatialAudio/SpatialAudioManager/Public/SpatialAudioManager.h"



FSASoundfieldEndpoint::FSASoundfieldEndpoint(int32 NumRenderCallbacksToBuffer)
    : ISoundfieldEndpoint(NumRenderCallbacksToBuffer)
{
}

FSASoundfieldEndpoint::~FSASoundfieldEndpoint()
{
}



bool FSASoundfieldEndpoint::EndpointRequiresCallback() const
{
    return true;
}

void FSASoundfieldEndpoint::OnAudioCallback(TUniquePtr<ISoundfieldAudioPacket>&& InPacket, const ISoundfieldEndpointSettingsProxy* InSettings)
{
    if (InPacket == nullptr)
    {
        UE_LOG(LogSpatialAudio, Error, TEXT("null packet call"));
        return;
    };
    
    // This is the incoming packet that gets enqueued in the ringbuffer. This packet is summed and mixed by our encoder/mixer/transcoder.
    const FSAPacket& InPacketCast = DowncastSoundfieldRef<const FSAPacket>(*InPacket);
    if (InPacketCast.Sources.Num() == 0) return;
    
    if (FSpatialAudioModule::RingBuffer)
    {
        FSpatialAudioModule::RingBuffer->Write(InPacketCast);
    }
    
    // show gain in debug speakers
    ASpatialAudioManager* SAManager = ASpatialAudioManager::Instance;
    if (IsValid(SAManager) && SAManager->bShowDebugSpeakerAmp && InPacketCast.GetBufferLength() > 0)
    {
        FScopeTryLock DebugAmpLock(&SAManager->DebugSpeakerAmplitudesCriticalSection);
        
        if (DebugAmpLock.IsLocked())
        {
            for (int32 c = 0; c < SAManager->DebugSpeakerAmplitudes.Num(); c++)
            {
                SAManager->DebugSpeakerAmplitudes[c] = 0.0f;
            }
            for (const FSpatializedSource& Source : InPacketCast.Sources)
            {
                for (const TPair<int32, FInterpolatedGain>& ChanGainPair : Source.Gains)
                {
                    SAManager->DebugSpeakerAmplitudes[ChanGainPair.Key] += ChanGainPair.Value.Lerp(0.5f);
                }
            }
        }
    }
}





FName FSASoundfieldEndpointFactory::SoundfieldEndpointName = FName(TEXT("SpatialAudio-SoundfieldEndpoint"));

FSASoundfieldEndpointFactory::FSASoundfieldEndpointFactory()
{
    ISoundfieldEndpointFactory::RegisterSoundfieldFormat(this);
    ISoundfieldEndpointFactory::RegisterEndpointType(this);
}

FName FSASoundfieldEndpointFactory::GetEndpointTypeName()
{
    return FSASoundfieldEndpointFactory::SoundfieldEndpointName;
}

FName FSASoundfieldEndpointFactory::GetSoundfieldFormatName()
{
    return FSASoundfieldEndpointFactory::SoundfieldEndpointName;
}

TUniquePtr<ISoundfieldEndpoint> FSASoundfieldEndpointFactory::CreateNewEndpointInstance(const FAudioPluginInitializationParams& InitInfo, const ISoundfieldEndpointSettingsProxy& InitialSettings)
{
    return TUniquePtr<ISoundfieldEndpoint>(new FSASoundfieldEndpoint(4));
}

UClass* FSASoundfieldEndpointFactory::GetCustomEndpointSettingsClass() const
{
    return USAEndpointSettings::StaticClass();
}


USoundfieldEndpointSettingsBase* FSASoundfieldEndpointFactory::GetDefaultEndpointSettings()
{
    return NewObject<USAEndpointSettings>();
}



TUniquePtr<ISoundfieldEncoderStream> FSASoundfieldEndpointFactory::CreateEncoderStream(const FAudioPluginInitializationParams& InitInfo, const ISoundfieldEncodingSettingsProxy& InitialSettings)
{
    return TUniquePtr<FSAEncoder>(new FSAEncoder());
}

TUniquePtr<ISoundfieldDecoderStream> FSASoundfieldEndpointFactory::CreateDecoderStream(const FAudioPluginInitializationParams& InitInfo, const ISoundfieldEncodingSettingsProxy& InitialSettings)
{
    return TUniquePtr<FSADecoder>(new FSADecoder());
}

TUniquePtr<ISoundfieldTranscodeStream> FSASoundfieldEndpointFactory::CreateTranscoderStream(const FName SourceFormat, const ISoundfieldEncodingSettingsProxy& InitialSourceSettings, const FName DestinationFormat, const ISoundfieldEncodingSettingsProxy& InitialDestinationSettings, const FAudioPluginInitializationParams& InitInfo)
{
    return TUniquePtr<FSATranscoder>(new FSATranscoder());
}

TUniquePtr<ISoundfieldMixerStream> FSASoundfieldEndpointFactory::CreateMixerStream(const ISoundfieldEncodingSettingsProxy& InitialSettings)
{
    return TUniquePtr<FSAMixer>(new FSAMixer());
}

TUniquePtr<ISoundfieldAudioPacket> FSASoundfieldEndpointFactory::CreateEmptyPacket()
{
    return TUniquePtr<FSAPacket>(new FSAPacket());
}

bool FSASoundfieldEndpointFactory::CanTranscodeFromSoundfieldFormat(FName SourceFormat, const ISoundfieldEncodingSettingsProxy& SourceEncodingSettings)
{
    return true;
}

bool FSASoundfieldEndpointFactory::CanTranscodeToSoundfieldFormat(FName DestinationFormat, const ISoundfieldEncodingSettingsProxy& DestinationEncodingSettings)
{
    return true;
}

bool FSASoundfieldEndpointFactory::ShouldEncodeAllStreamsIndependently(const ISoundfieldEncodingSettingsProxy& EncodingSettings)
{
    return true;
}



UClass* FSASoundfieldEndpointFactory::GetCustomEncodingSettingsClass() const
{
    return USAEncodingSettings::StaticClass();
}

const USoundfieldEncodingSettingsBase* FSASoundfieldEndpointFactory::GetDefaultEncodingSettings()
{
    return NewObject<USAEncodingSettings>();
}

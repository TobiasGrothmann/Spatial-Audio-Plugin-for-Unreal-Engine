#pragma once

#include "Sound/SoundSubmix.h"
#include "ISoundfieldEndpoint.h"



class SPATIALAUDIO_API FSASoundfieldEndpoint : public ISoundfieldEndpoint
{
public:
    
    FSASoundfieldEndpoint(int32 NumRenderCallbacksToBuffer);
    virtual ~FSASoundfieldEndpoint() override;
    
    
    virtual bool EndpointRequiresCallback() const override;

    virtual void OnAudioCallback(TUniquePtr<ISoundfieldAudioPacket>&& InPacket, const ISoundfieldEndpointSettingsProxy* InSettings) override;
};



class SPATIALAUDIO_API FSASoundfieldEndpointFactory : public ISoundfieldEndpointFactory
{
public:
    
    FSASoundfieldEndpointFactory();
    
    static FName SoundfieldEndpointName;
    virtual FName GetEndpointTypeName() override;
    virtual FName GetSoundfieldFormatName() override;
    
    virtual TUniquePtr<ISoundfieldEndpoint> CreateNewEndpointInstance(const FAudioPluginInitializationParams& InitInfo, const ISoundfieldEndpointSettingsProxy& InitialSettings) override;
    
    virtual UClass* GetCustomEndpointSettingsClass() const override;

    virtual USoundfieldEndpointSettingsBase* GetDefaultEndpointSettings() override;
    
    
    
    virtual TUniquePtr<ISoundfieldEncoderStream> CreateEncoderStream(const FAudioPluginInitializationParams& InitInfo, const ISoundfieldEncodingSettingsProxy& InitialSettings) override;
    virtual TUniquePtr<ISoundfieldDecoderStream> CreateDecoderStream(const FAudioPluginInitializationParams& InitInfo, const ISoundfieldEncodingSettingsProxy& InitialSettings) override;
    virtual TUniquePtr<ISoundfieldTranscodeStream> CreateTranscoderStream(const FName SourceFormat, const ISoundfieldEncodingSettingsProxy& InitialSourceSettings, const FName DestinationFormat, const ISoundfieldEncodingSettingsProxy& InitialDestinationSettings, const FAudioPluginInitializationParams& InitInfo) override;
    virtual TUniquePtr<ISoundfieldMixerStream> CreateMixerStream(const ISoundfieldEncodingSettingsProxy& InitialSettings) override;
    virtual TUniquePtr<ISoundfieldAudioPacket> CreateEmptyPacket() override;

    virtual bool CanTranscodeFromSoundfieldFormat(FName SourceFormat, const ISoundfieldEncodingSettingsProxy& SourceEncodingSettings) override;
    virtual bool CanTranscodeToSoundfieldFormat(FName DestinationFormat, const ISoundfieldEncodingSettingsProxy& DestinationEncodingSettings) override;
    
    virtual bool ShouldEncodeAllStreamsIndependently(const ISoundfieldEncodingSettingsProxy& EncodingSettings) override;
    
    virtual UClass* GetCustomEncodingSettingsClass() const override;

    virtual const USoundfieldEncodingSettingsBase* GetDefaultEncodingSettings() override;
};

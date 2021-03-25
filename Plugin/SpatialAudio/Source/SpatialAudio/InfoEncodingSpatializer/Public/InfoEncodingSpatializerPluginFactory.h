#pragma once

#include "CoreMinimal.h"


class SPATIALAUDIO_API FInfoEncodingSpatializerPluginFactory : public IAudioSpatializationFactory
{
public:
    
    static FString GetSpatializerPluginName();
    
    virtual TAudioSpatializationPtr CreateNewSpatializationPlugin(FAudioDevice* OwningDevice) override;
    
    virtual FString GetDisplayName() override;
    virtual bool SupportsPlatform(const FString& PlatformName) override;
    virtual int32 GetMaxSupportedChannels() override;

//    virtual UClass* GetCustomSpatializationSettingsClass() const override;

};

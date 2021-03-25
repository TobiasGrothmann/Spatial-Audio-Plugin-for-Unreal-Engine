#include "../Public/InfoEncodingSpatializerPluginFactory.h"
#include "../Public/InfoEncodingSpatializer.h"

#include "CoreMinimal.h"



FString FInfoEncodingSpatializerPluginFactory::GetSpatializerPluginName()
{
    return "Spatial Audio Spatializer";
}

FString FInfoEncodingSpatializerPluginFactory::GetDisplayName()
{
    return FInfoEncodingSpatializerPluginFactory::GetSpatializerPluginName();
}

bool FInfoEncodingSpatializerPluginFactory::SupportsPlatform(const FString& PlatformName)
{
    return true;
}

TAudioSpatializationPtr FInfoEncodingSpatializerPluginFactory::CreateNewSpatializationPlugin(FAudioDevice* OwningDevice)
{
    return TAudioSpatializationPtr(new FInfoEncodingSpatialization());
};

int32 FInfoEncodingSpatializerPluginFactory::GetMaxSupportedChannels()
{
    return 128;
}

//UClass* FInfoEncodingSpatializerPluginFactory::GetCustomSpatializationSettingsClass() const
//{
//    return
//}

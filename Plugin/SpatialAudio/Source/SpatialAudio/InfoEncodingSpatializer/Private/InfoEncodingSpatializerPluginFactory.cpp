#include "../Public/InfoEncodingSpatializerPluginFactory.h"
#include "../Public/InfoEncodingSpatializer.h"

#include "CoreMinimal.h"



FString FInfoEncodingSpatializerPluginFactory::GetSpatializerPluginName()
{
    // this is the name that is displayed in the project settings when selecting the spatializer plugin for the platform:
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

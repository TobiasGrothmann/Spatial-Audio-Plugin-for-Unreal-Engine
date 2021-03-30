#include "../Public/SpatialAudioModule.h"

#include "Features/IModularFeatures.h"
#include "Engine/World.h"

#include "SpatialAudio/Data/Public/SAPacket.h"
#include "SpatialAudio/SpatialAudioManager/Public/SpatialAudioManager.h"

DEFINE_LOG_CATEGORY(LogSpatialAudio);


FSAringBuffer* FSpatialAudioModule::RingBuffer = nullptr;
FVbap* FSpatialAudioModule::Vbap = nullptr;

void FSpatialAudioModule::StartupModule()
{
    IModularFeatures::Get().RegisterModularFeature(FInfoEncodingSpatializerPluginFactory::GetModularFeatureName(), &PluginFactory);
}

void FSpatialAudioModule::ShutdownModule()
{
    if (FSpatialAudioModule::RingBuffer)
    {
        delete FSpatialAudioModule::RingBuffer;
    }
}


IMPLEMENT_MODULE(FSpatialAudioModule, SpatialAudio)

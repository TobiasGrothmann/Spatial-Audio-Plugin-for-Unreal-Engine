#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSpatialAudio, Log, All);

#include "SpatialAudio/InfoEncodingSpatializer/Public/InfoEncodingSpatializerPluginFactory.h"
#include "SpatialAudio/Soundfield/Public/SASoundfieldEndpoint.h"
#include "SpatialAudio/Data/Public/SARingBuffer.h"
#include "SpatialAudio/Vbap/Public/Vbap.h"

class FToolBarBuilder;
class FMenuBuilder;
class FSAPacket;



class SPATIALAUDIO_API FSpatialAudioModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** This function will be bound to Command (by default it will bring up plugin window) */
	void PluginButtonClicked();
	
private:

	void RegisterMenus();

	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);

	TSharedPtr<class FUICommandList> PluginCommands;

    
    
// *** Factories to register classes with the engine ***
    FInfoEncodingSpatializerPluginFactory PluginFactory;
    FSASoundfieldEndpointFactory SoundfieldEndpointFactory;
    
    
    
public:

// *** AUDIO OUTPUT ROLLING BUFFER ***
    static FSAringBuffer* RingBuffer;
    
// *** VBAP ***
    static FVbap* Vbap;
};

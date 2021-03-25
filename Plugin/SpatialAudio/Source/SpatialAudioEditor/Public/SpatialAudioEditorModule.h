#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

#include "Components/AudioComponent.h"

#include "../SpatialAudioSetupTab/Public/SpatialAudioSetupTab.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSpatialAudioEditor, Log, All);

class FToolBarBuilder;
class FMenuBuilder;
class FSAPacket;



class FSpatialAudioEditorModule : public IModuleInterface
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
};

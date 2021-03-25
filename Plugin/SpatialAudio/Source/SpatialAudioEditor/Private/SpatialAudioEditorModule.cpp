#include "../Public/SpatialAudioEditorModule.h"

#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"
#include "Features/IModularFeatures.h"

#include "../SpatialAudioManagerDetails/Public/SpatialAudioManagerDetails.h"
#include "../Public/SpatialAudioEditorCommands.h"
#include "../Public/SpatialAudioEditorStyle.h"


DEFINE_LOG_CATEGORY(LogSpatialAudioEditor);

static const FName SpatialAudioEditorTabName("Spatial Audio Setup");


void FSpatialAudioEditorModule::StartupModule()
{
    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    PropertyModule.RegisterCustomClassLayout("SpatialAudioManager", FOnGetDetailCustomizationInstance::CreateStatic(&FSpatialAudioManagerDetails::MakeInstance));
    
    FSpatialAudioEditorStyle::Initialize();
    FSpatialAudioEditorStyle::ReloadTextures();

    FSpatialAudioEditorCommands::Register();
    
    PluginCommands = MakeShareable(new FUICommandList);

    PluginCommands->MapAction(
        FSpatialAudioEditorCommands::Get().OpenPluginWindow,
        FExecuteAction::CreateRaw(this, &FSpatialAudioEditorModule::PluginButtonClicked),
        FCanExecuteAction());

    UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FSpatialAudioEditorModule::RegisterMenus));
    
    FGlobalTabmanager::Get()->RegisterNomadTabSpawner(SpatialAudioEditorTabName, FOnSpawnTab::CreateRaw(this, &FSpatialAudioEditorModule::OnSpawnPluginTab))
        .SetDisplayName(FText::FromString("Spatial Audio Setup"))
        .SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FSpatialAudioEditorModule::ShutdownModule()
{
    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

    PropertyModule.UnregisterCustomClassLayout("SpatialAudioManager");
    
    
    
    UToolMenus::UnRegisterStartupCallback(this);
    UToolMenus::UnregisterOwner(this);
    
    FSpatialAudioEditorStyle::Shutdown();
    FSpatialAudioEditorCommands::Unregister();

    FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(SpatialAudioEditorTabName);
}

void FSpatialAudioEditorModule::PluginButtonClicked()
{
    FGlobalTabmanager::Get()->InvokeTab(SpatialAudioEditorTabName);
}

void FSpatialAudioEditorModule::RegisterMenus()
{
    // Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
    FToolMenuOwnerScoped OwnerScoped(this);

    {
        UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
        {
            FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
            Section.AddMenuEntryWithCommandList(FSpatialAudioEditorCommands::Get().OpenPluginWindow, PluginCommands);
        }
    }

    {
        UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
        {
            FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
            {
                FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FSpatialAudioEditorCommands::Get().OpenPluginWindow));
                Entry.SetCommandList(PluginCommands);
            }
        }
    }
}

TSharedRef<SDockTab> FSpatialAudioEditorModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
    return SNew(SDockTab)
    .TabRole(ETabRole::NomadTab)
    [
        SNew(SSpatialAudioSetupTab)
    ];
}





IMPLEMENT_MODULE(FSpatialAudioEditorModule, SpatialAudioEditor)

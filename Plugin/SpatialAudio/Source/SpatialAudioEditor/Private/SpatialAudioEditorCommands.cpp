#include "SpatialAudioEditorCommands.h"

#define LOCTEXT_NAMESPACE "FSpatialAudioEditorModule"


void FSpatialAudioEditorCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "Spatial Audio Setup", "Bring up Spatial Audio Setup window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE

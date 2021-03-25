#include "SpatialAudioCommands.h"

#define LOCTEXT_NAMESPACE "FSpatialAudioModule"


void FSpatialAudioCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "SpatialAudio", "Bring up SpatialAudio window", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE

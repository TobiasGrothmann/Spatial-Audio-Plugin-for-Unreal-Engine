#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "SpatialAudioStyle.h"


class SPATIALAUDIO_API FSpatialAudioCommands : public TCommands<FSpatialAudioCommands>
{
public:

	FSpatialAudioCommands()
		: TCommands<FSpatialAudioCommands>(TEXT("SpatialAudio"), NSLOCTEXT("Contexts", "SpatialAudio", "SpatialAudio Plugin"), NAME_None, FSpatialAudioStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};

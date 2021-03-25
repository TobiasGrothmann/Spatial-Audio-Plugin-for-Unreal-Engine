#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "SpatialAudioEditorStyle.h"


class FSpatialAudioEditorCommands : public TCommands<FSpatialAudioEditorCommands>
{
public:

	FSpatialAudioEditorCommands()
		: TCommands<FSpatialAudioEditorCommands>(TEXT("SpatialAudioEditor"), NSLOCTEXT("Contexts", "SpatialAudioEditor", "SpatialAudioEditor Plugin"), NAME_None, FSpatialAudioEditorStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};

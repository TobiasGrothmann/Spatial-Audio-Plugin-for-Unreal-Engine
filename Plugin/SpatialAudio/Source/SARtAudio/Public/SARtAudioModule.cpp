// Copyright Epic Games, Inc. All Rights Reserved.

#include "CoreMinimal.h"
#include "AudioCaptureDeviceInterface.h"
#include "Features/IModularFeatures.h"
#include "Modules/ModuleManager.h"


class FSARtAudioModule : public IModuleInterface
{
private:

public:
    virtual void StartupModule() override
    {
    }

    virtual void ShutdownModule() override
    {
    }
};

IMPLEMENT_MODULE(FSARtAudioModule, SARtAudio)

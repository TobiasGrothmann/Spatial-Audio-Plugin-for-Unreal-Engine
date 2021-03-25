#pragma once

#include "CoreMinimal.h"
#include "Editor/DetailCustomizations/Public/DetailCustomizations.h"
#include "Editor/PropertyEditor/Public/IDetailCustomization.h"

struct FSpeakerPosition;


class FSpatialAudioManagerDetails : public IDetailCustomization
{
public:

    static TSharedRef<IDetailCustomization> MakeInstance();
    
    void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder);
    
private:
    
    void LoadIosono();
    void LoadCsv();
    
    void SetSpeakerPositions(const TArray<FSpeakerPosition>& ParsedPositions);
    
    bool OpenFileDialog(const FString& FileTypes, FString& OutPath);
};

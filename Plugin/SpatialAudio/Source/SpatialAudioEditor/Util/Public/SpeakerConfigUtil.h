#pragma once

#include "CoreMinimal.h"
#include "XmlFile.h"


class SPATIALAUDIOEDITOR_API SpeakerConfigUtil
{
public:
    
    static bool FromXmlFile(const FString& FilePath, TArray<FSpeakerPosition>& OutPositions);
    static bool FromCsvFile(const FString& FilePath, TArray<FSpeakerPosition>& OutPositions);
    
private:
    
    static TArray<FXmlNode*> GetChildrenNodesByTag(const FXmlNode* Node, const FString& RequiredTag);
};

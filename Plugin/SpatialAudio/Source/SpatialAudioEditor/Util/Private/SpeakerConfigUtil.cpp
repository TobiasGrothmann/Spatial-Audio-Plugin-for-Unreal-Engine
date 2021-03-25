#include "../Public/SpeakerConfigUtil.h"
#include "Misc/FileHelper.h"
#include "Serialization/Csv/CsvParser.h"


 bool SpeakerConfigUtil::FromXmlFile(const FString& FilePath, TArray<FSpeakerPosition>& OutPositions)
{
    OutPositions.Empty();
    
    FXmlFile File;
    if (!File.LoadFile(FilePath)) return false;
    
    FXmlNode* LoudspeakerSetup = File.GetRootNode();
    if (LoudspeakerSetup->GetTag() != "LoudspeakerSetup") return false;
    
    for (const FXmlNode* SpeakerGroupList : GetChildrenNodesByTag(LoudspeakerSetup, "SpeakerGroupList"))
    {
        for (const FXmlNode* SpeakerGroup : GetChildrenNodesByTag(SpeakerGroupList, "SpeakerGroup"))
        {
            for (const FXmlNode* Speaker : GetChildrenNodesByTag(SpeakerGroup, "Speaker"))
            {
                float X = 0.0f;
                float Y = 0.0f;
                float Z = 0.0f;
                
                for (const FXmlAttribute& Attribute : Speaker->GetAttributes())
                {
                    if (Attribute.GetTag() == "x")
                    {
                        X = FCString::Atof(*Attribute.GetValue());
                    }
                    else if (Attribute.GetTag() == "y")
                    {
                        Y = FCString::Atof(*Attribute.GetValue());
                    }
                    else if (Attribute.GetTag() == "z")
                    {
                        Z = FCString::Atof(*Attribute.GetValue());
                    }
                }
                
                UE_LOG(LogSpatialAudioEditor, Log, TEXT("Found Speaker at: %f, %f, %f"), X, Y, Z);
                OutPositions.Add(FSpeakerPosition::FromCartesian(X, Y, Z));
            }
        }
    }
    
    return true;
}


TArray<FXmlNode*> SpeakerConfigUtil::GetChildrenNodesByTag(const FXmlNode* Node, const FString& RequiredTag)
{
    TArray<FXmlNode*> Out = {};
    for (FXmlNode* Child : Node->GetChildrenNodes())
    {
        if (Child->GetTag() == RequiredTag)
        {
            Out.Add(Child);
        }
    }
    return Out;
}




bool SpeakerConfigUtil::FromCsvFile(const FString& FilePath, TArray<FSpeakerPosition>& OutPositions)
{
    OutPositions.Empty();
    
    FString FileContentString;
    if (!FFileHelper::LoadFileToString(FileContentString, *FilePath))
    {
        return false;
    }

    FCsvParser CSVParser(FileContentString);
    const FCsvParser::FRows& Rows = CSVParser.GetRows();

    for (const TArray<const TCHAR*>& Row : Rows)
    {
        TArray<float> Values = {};
        for (const TCHAR* Cell : Row)
        {
            try {
                Values.Add(FCString::Atof(Cell));
            } catch (...) {
                UE_LOG(LogSpatialAudioEditor, Error, TEXT("Could not convert '%s' to number."), Cell);
                return false;
            }
        }
        
        if (Values.Num() == 2)
        {
            OutPositions.Add(FSpeakerPosition::FromPolar(Values[0], Values[1], 1.0f));
        }
        else if (Values.Num() == 3)
        {
            OutPositions.Add(FSpeakerPosition::FromPolar(Values[0], Values[1], Values[2]));
        }
        else
        {
            UE_LOG(LogSpatialAudioEditor, Error, TEXT("Found %i number of cells in row but only 2 (azi(deg), ele(deg)) or 3 (azi(deg), ele(deg), dist(m)) are allowed."), Values.Num());
            return false;
        }
    }
    
    return true;
}

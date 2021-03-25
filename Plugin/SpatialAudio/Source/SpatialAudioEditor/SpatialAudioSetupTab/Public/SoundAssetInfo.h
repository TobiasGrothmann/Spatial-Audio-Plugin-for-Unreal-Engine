#pragma once

#include "CoreMinimal.h"
#include "Sound/SoundSubmix.h"

#include "SpatialAudio/Soundfield/Public/SASoundfieldEndpoint.h"
#include "SpatialAudioEditor/Util/Public/EditorUtil.h"


class SPATIALAUDIOEDITOR_API FSoundAssetInfo
{
public:
    
    FSoundAssetInfo(const FAssetData& Asset)
    {
        Name = Asset.AssetName.ToString();
        PackageName = Asset.PackageName.ToString();
        
        Sound = Cast<USoundBase>(Asset.GetAsset());
    }
    
    FString Name = {};
    FString PackageName = {};
    
    USoundBase* Sound = nullptr;
    
    bool GetUsesCorrectSoundfieldEndpoint()
    {
        if (!IsValid(Sound)) return false;
        USoundSubmixBase* SoundSubmix = Sound->GetSoundSubmix();
        if (!IsValid(SoundSubmix)) return false;
        
        USoundfieldEndpointSubmix* SoundfieldEndpoint = Cast<USoundfieldEndpointSubmix>(SoundSubmix);
        if (!IsValid(SoundfieldEndpoint)) return false;
        
        return SoundfieldEndpoint->SoundfieldEndpointType == FSASoundfieldEndpointFactory::SoundfieldEndpointName;
    }
    
    void SetUsesCorrectSoundfieldEndpoint(bool newValue)
    {
        if (!IsValid(Sound)) return;
        
        if (!newValue)
        {
            Sound->SoundSubmixObject = nullptr;
        }
        else
        {
            USoundfieldEndpointSubmix* CorrectSoundfieldSubmix = nullptr;
            
            TArray<USoundfieldEndpointSubmix*> SFSubmixes = EditorUtil::GetObjectsOfClass<USoundfieldEndpointSubmix>();
            for (USoundfieldEndpointSubmix* SFSubmix : SFSubmixes)
            {
                if (SFSubmix->SoundfieldEndpointType == FSASoundfieldEndpointFactory::SoundfieldEndpointName)
                {
                    CorrectSoundfieldSubmix = SFSubmix;
                }
            }
            
            if (IsValid(CorrectSoundfieldSubmix))
            {
                Sound->SoundSubmixObject = CorrectSoundfieldSubmix;
            }
            
            Sound->Modify(true);
        }
    }
};

#pragma once

#include "CoreMinimal.h"

#include "SpatialAudio/Soundfield/Public/SASoundfieldEndpoint.h"


class SPATIALAUDIOEDITOR_API FAudioComponentInfo
{
public:
    
    FAudioComponentInfo(UAudioComponent* AudioComponent)
    : AudioComponent(AudioComponent)
    {
        Actor = AudioComponent->GetOwner();
    }
    
    UAudioComponent* AudioComponent = nullptr;
    AActor* Actor = nullptr;
    
    
    bool IsValidInfo() const
    {
        if (!IsValid(AudioComponent)) return false;
        if (!IsValid(Actor)) return false;
        return true;
    }
    
    
    
    FText GetActorName() const
    {
        if (!IsValid(Actor)) return FText::FromString("invalid");
        return FText::FromString(Actor->GetActorLabel());
    }
    
    FText GetComponentName() const
    {
        if (!IsValid(AudioComponent)) return FText::FromString("invalid");
        return FText::FromName(AudioComponent->GetFName());
    }
    
    bool GetUsesSpat() const
    {
        if (!IsValid(AudioComponent)) return false;
        
        if (   AudioComponent->bAllowSpatialization
            && AudioComponent->bOverrideAttenuation
            && AudioComponent->AttenuationOverrides.bSpatialize
            && AudioComponent->AttenuationOverrides.SpatializationAlgorithm == ESoundSpatializationAlgorithm::SPATIALIZATION_HRTF
            )
        {
            return true;
        }
        
        return false;
    }
    
    void SetUsesSpat(bool newValue)
    {
        if (!IsValid(AudioComponent)) return;
        
        AudioComponent->bAllowSpatialization = newValue;
        
        if (newValue)
        {
            AudioComponent->bOverrideAttenuation = true;
            AudioComponent->AttenuationOverrides.bSpatialize = true;
            AudioComponent->AttenuationOverrides.SpatializationAlgorithm = ESoundSpatializationAlgorithm::SPATIALIZATION_HRTF;
        }
        
        AudioComponent->Modify(true);
    }
    
};

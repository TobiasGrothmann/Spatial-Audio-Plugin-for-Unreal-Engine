#pragma once

#include "CoreMinimal.h"
#include "SpeakerPosition.generated.h"


USTRUCT()
struct SPATIALAUDIO_API FSpeakerPosition
{
    GENERATED_BODY()
public:
    
    FSpeakerPosition() = default;
    
    static FSpeakerPosition FromPolar(float Azimuth, float Elevation, float Distance);
    static FSpeakerPosition FromCartesian(float X, float Y, float Z);
    
    
    
// *** SETTINGS ***
    UPROPERTY(EditAnywhere, Category = "Location")
    float Azimuth = 0.0f; // in degrees
    
    UPROPERTY(EditAnywhere, Category = "Location")
    float Elevation = 0.0f; // in degrees
    
    UPROPERTY(EditAnywhere, Category = "Location")
    float Distance = 1.0f;
    
    
    
// *** UTIL ***
    FVector GetAsVector(const FRotator& AddRotation = FRotator::ZeroRotator) const;
};

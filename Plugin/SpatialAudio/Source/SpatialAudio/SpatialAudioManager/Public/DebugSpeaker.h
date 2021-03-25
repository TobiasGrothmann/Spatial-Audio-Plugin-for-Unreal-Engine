#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/StaticMesh.h"
#include "DebugSpeaker.generated.h"


UCLASS()
class ADebugSpeaker : public AActor
{
    GENERATED_BODY()
public:
    
    ADebugSpeaker();
    
    void SetAmp(float amp); // current amplitude through this speaker for debugging
    
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* SpeakerMesh = nullptr;
};

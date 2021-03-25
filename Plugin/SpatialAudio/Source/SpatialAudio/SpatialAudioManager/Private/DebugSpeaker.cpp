#include "../Public/DebugSpeaker.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/CollisionProfile.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"


ADebugSpeaker::ADebugSpeaker()
{
    ConstructorHelpers::FObjectFinder<UStaticMesh> SpeakerMeshRef(TEXT("/SpatialAudio/SpeakerPreview/Speaker"));

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("root"));
    
    if (SpeakerMeshRef.Succeeded())
    {
        SpeakerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("mesh"));
        SpeakerMesh->SetStaticMesh(SpeakerMeshRef.Object);
        SpeakerMesh->SetupAttachment(RootComponent);
        SpeakerMesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
    }
    
     SetActorEnableCollision(false);
}

void ADebugSpeaker::SetAmp(float amp)
{
    static const float MinScale = 0.2f;
    static const float MaxScale = 2.5f;
    static const float Factor = 0.75f; // per source at gain 1
    float Scale = FMath::Min(MinScale + FMath::Max(amp, 0.0f) * Factor, MaxScale);
    if (IsValid(SpeakerMesh))
    {
        SpeakerMesh->SetRelativeScale3D(FVector(Scale, Scale, Scale));
    }
}

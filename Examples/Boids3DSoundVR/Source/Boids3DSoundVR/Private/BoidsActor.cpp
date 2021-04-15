#include "BoidsActor.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/StaticMesh.h"
#include "Math/UnrealMathUtility.h"
#include "Sound/SoundCue.h"
#include "Math/Quat.h"


ABoidsActor::ABoidsActor()
{
	PrimaryActorTick.bCanEverTick = true;
    
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    
    // create sphere that defines the vision range
    VisionRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("VisisbleSphere"));
    VisionRangeSphere->SetSphereRadius(VisibleRangeDistance);
    VisionRangeSphere->SetupAttachment(RootComponent);
    
    // register overlap events for VisionRangeSphere
    VisionRangeSphere->OnComponentBeginOverlap.AddDynamic(this, &ABoidsActor::OnBeginOverlap);
    VisionRangeSphere->OnComponentEndOverlap.AddDynamic(this, &ABoidsActor::OnEndOverlap);
    bGenerateOverlapEventsDuringLevelStreaming = true;
    
    // create sphere mesh that will be visible in the world
    UStaticMeshComponent* Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh>MeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Cone.Cone'"));
    if (MeshAsset.Succeeded())
    {
        Mesh->SetStaticMesh(MeshAsset.Object);
        Mesh->SetupAttachment(RootComponent);
        Mesh->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
        const float Scale = 0.5;
        Mesh->SetRelativeScale3D(FVector(Scale, Scale, Scale));
    }
    
    // create audio component
    Audio = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio"));
    Audio->SetupAttachment(RootComponent);
    static ConstructorHelpers::FObjectFinder<USoundCue>SoundCueAsset(TEXT("SoundCue'/Game/sound/droneCue.droneCue'"));
    if (SoundCueAsset.Succeeded())
    {
        Audio->SetSound(SoundCueAsset.Object);
    }
}

void ABoidsActor::BeginPlay()
{
	Super::BeginPlay();
    
    VisionRangeSphere->SetSphereRadius(VisibleRangeDistance);
    
    // start with random velocity
    Velocity = FVector(FMath::RandRange(-1.0f, 1.0f), FMath::RandRange(-1.0f, 1.0f), FMath::RandRange(-1.0f, 1.0f));
    
    // start with no other actors in visible range
    VisibleOthers.Empty();
    
    Audio->Play(FMath::RandRange(0.0, 1.0f));
}

void ABoidsActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    
    // the velocity target should preferrably be calculated for all boids actors first and then be applied afterwards, but it does not seem to make a big difference
    const FVector Alignment = GetAlignmentTarget().GetClampedToMaxSize(MaxForce);
    const FVector Cohesion = GetCohesionTarget().GetClampedToMaxSize(MaxForce);
    const FVector Separation = GetSeparationTarget().GetClampedToMaxSize(MaxForce);
    const FVector VelocityTarget =
        Alignment * 2.0f +
        Cohesion * 1.0f +
        Separation * 5.0f +
        GetOutOfBoundsTarget() * 2.5f;
    const FVector NewVelocity = Velocity * (1.0 - Agility) + VelocityTarget * Agility;
    const float TimeFactor = FMath::Clamp(DeltaTime * 5.0f, 0.0f, 1.0f);
    Velocity = Velocity * (1.0f - TimeFactor) + NewVelocity * TimeFactor;
    Velocity = Velocity.GetClampedToSize(15.0f, 100.0f);
    
    SetActorRotation(Velocity.Rotation());
    SetActorLocation(GetActorLocation() + Velocity * Speed * TimeFactor);
    
    VisionRangeSphere->SetSphereRadius(VisibleRangeDistance);
}



void ABoidsActor::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    ABoidsActor* Other = Cast<ABoidsActor>(OtherActor);
    if (IsValid(Other) && !VisibleOthers.Contains(Other))
    {
        // another boids actor is in visible range
        VisibleOthers.Add(Other);
    }
}

void ABoidsActor::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    ABoidsActor* Other = Cast<ABoidsActor>(OtherActor);
    if (IsValid(Other) && VisibleOthers.Contains(Other))
    {
        // previously visible other actors moved out of visible range
        VisibleOthers.Remove(Other);
    }
}




FVector ABoidsActor::GetAlignmentTarget() const
{
    // force towards the same direction
    FVector Target = FVector::ZeroVector;
    for (ABoidsActor* Other : VisibleOthers)
    {
        Target += Other->Velocity / VisibleOthers.Num();
    }
    return Target;
}

FVector ABoidsActor::GetCohesionTarget() const
{
    // force towards the neighbours common center
    FVector Target = FVector::ZeroVector;
    for (ABoidsActor* Other : VisibleOthers)
    {
        Target += (Other->GetActorLocation() - GetActorLocation()) / VisibleOthers.Num();
    }
    return Target;
}

FVector ABoidsActor::GetSeparationTarget() const
{
    // force away from each other if too close
    FVector Target = FVector::ZeroVector;
    for (ABoidsActor* Other : VisibleOthers)
    {
        const float Dist = FVector::Distance(GetActorLocation(), Other->GetActorLocation());
        if (Dist < SeparationDistance)
        {
            const FVector Away = GetActorLocation() - Other->GetActorLocation();
            Target += Away * (SeparationDistance - Dist);
        }
    }
    return Target;
}

FVector ABoidsActor::GetOutOfBoundsTarget() const
{
    // force back to center if too far away
    FVector Location = GetActorLocation();
    if (GetActorLocation().Size() > Range)
    {
        return (FVector::ZeroVector - GetActorLocation()).GetClampedToMaxSize(50.0f);
    }
    return FVector::ZeroVector;
}

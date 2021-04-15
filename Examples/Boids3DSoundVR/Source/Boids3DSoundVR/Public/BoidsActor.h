#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Components/AudioComponent.h"
#include "BoidsActor.generated.h"


UCLASS()
class BOIDS3DSOUNDVR_API ABoidsActor : public AActor
{
	GENERATED_BODY()
public:
    
	ABoidsActor();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
    
    
    UFUNCTION()
    void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
    
    UFUNCTION()
    void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
    
    FVector GetAlignmentTarget() const;
    FVector GetCohesionTarget() const;
    FVector GetSeparationTarget() const;
    FVector GetOutOfBoundsTarget() const;
    
    
    UPROPERTY(EditAnywhere, Category="Components")
    USphereComponent* VisionRangeSphere = nullptr;
    
    UPROPERTY(EditAnywhere, Category="Components")
    UAudioComponent* Audio = nullptr;
    
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boids")
    float Speed = 0.40;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boids")
    float Agility = 0.12f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boids")
    float MaxForce = 20.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boids")
    float VisibleRangeDistance = 170.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boids")
    float SeparationDistance = 130.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boids")
    float Range = 900.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boids")
    float AlignmentForce = 2.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boids")
    float CohesionForce = 1.0f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boids")
    float SeparationForce = 6.0f;
    
    
    UPROPERTY(VisibleAnywhere, Category = "BoidsAlgorithm")
    FVector Velocity = FVector::ZeroVector;
    
    UPROPERTY(VisibleAnywhere, Category = "BoidsAlgorithm")
    TArray<ABoidsActor*> VisibleOthers = {};
};

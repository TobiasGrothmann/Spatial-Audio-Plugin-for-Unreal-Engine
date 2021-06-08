#pragma once
#include "../Public/SpeakerPosition.h"
#include "CoreMinimal.h"

FSpeakerPosition FSpeakerPosition::FromPolar(float Azimuth, float Elevation, float Distance)
{
    FSpeakerPosition NewPos = FSpeakerPosition();
    NewPos.Azimuth = Azimuth;
    NewPos.Elevation = Elevation;
    NewPos.Distance = Distance;
    return NewPos;
}
    
FSpeakerPosition FSpeakerPosition::FromCartesian(float X, float Y, float Z)
{
    FSpeakerPosition NewPos = FSpeakerPosition();
    const FVector Location = FVector(X, Y, Z);
    const FRotator Rotation = Location.Rotation();
    NewPos.Azimuth = Rotation.Yaw;
    NewPos.Elevation = Rotation.Pitch;
    NewPos.Distance = Location.Size();
    return NewPos;
}

FVector FSpeakerPosition::GetAsVector(const FRotator& AddRotation) const
{
    FVector Dir = FRotator(Elevation, Azimuth, 0.0f).Vector() * (Distance * 100.0f);
    FQuat Rot = FQuat(AddRotation);
    return Rot.RotateVector(Dir);
}

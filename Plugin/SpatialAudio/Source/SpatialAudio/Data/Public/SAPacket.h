#pragma once

#include "CoreMinimal.h"
#include "ISoundfieldFormat.h"
#include "SAPacket.generated.h"


struct FInterpolatedGain
{
    float Start;
    float End;
    
    // This means that the fades on fast movements are buffer size dependent. That's probably ok for now
    float Lerp(float Factor) const // Factor from 0.0f (Start) to 1.0f (End)
    {
        return Start + (End - Start) * Factor;
    }
};

USTRUCT()
struct SPATIALAUDIO_API FSpatializedSource
{
    GENERATED_BODY();
    
    TSharedPtr<Audio::AlignedFloatBuffer> Buffer;
    TMap<int32, FInterpolatedGain> Gains = {};
};


class SPATIALAUDIO_API FSAPacket : public ISoundfieldAudioPacket
{
public:
    
    FSAPacket() = default;
    FSAPacket(const FSAPacket* Other);
    virtual ~FSAPacket() override;
    
    virtual void Serialize(FArchive& Ar) override;

    virtual TUniquePtr<ISoundfieldAudioPacket> Duplicate() const override;

    virtual void Reset() override;
    
    inline int32 GetBufferLength() const;
    
    
    TArray<FSpatializedSource> Sources = {};
};

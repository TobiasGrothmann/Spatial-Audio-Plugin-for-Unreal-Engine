#include "../Public/SAPacket.h"
#include "SpatialAudio/Public/SpatialAudioModule.h"



FSAPacket::FSAPacket(const FSAPacket* Other)
{
    for (const FSpatializedSource& Source : Other->Sources)
    {
        this->Sources.Add({ Source.Buffer, Source.Gains });
    }
}

FSAPacket::~FSAPacket()
{
    this->Reset();
}

void FSAPacket::Serialize(FArchive& Ar)
{
    UE_LOG(LogSpatialAudio, Error, TEXT("Serialize called but is not implemented."));
}

TUniquePtr<ISoundfieldAudioPacket> FSAPacket::Duplicate() const
{
    FSAPacket* Copy = new FSAPacket(this);
    return TUniquePtr<ISoundfieldAudioPacket>(Copy);
}

void FSAPacket::Reset()
{
    Sources.Empty();
}

inline int32 FSAPacket::GetBufferLength() const
{
    for (const FSpatializedSource& Source : Sources)
    {
        if (!Source.Buffer.IsValid()) continue;
        return Source.Buffer->Num(); // assuming that buffers for all channels are the same length
    }
    return 0;
}

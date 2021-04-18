#pragma once

#include "CoreMinimal.h"
#include "HAL/ThreadSafeCounter.h"
#include "SAPacket.h"


class SPATIALAUDIO_API FSAringBuffer
{
public:
    
    FSAringBuffer() = default;
    FSAringBuffer(int32 NumChannels, int32 BufferSize);
    ~FSAringBuffer();
    
    void Write(const FSAPacket& InPacket);
    void Read(float* Out, int32 BufferFrames, int32 NumOutChannels, float MasterGain);
    
    FORCEINLINE bool IncrementWriteIndex();
    FORCEINLINE bool IncrementReadIndex();
    
    int32 GetBufferSizeUsed() const;
    
    int32 GetBufferSize() const;
    int32 GetNumChannels() const;
    
private:
    
    int32 NumChannels = 0;
    int32 BufferSize = 0;
    
    TArray<Audio::AlignedFloatBuffer*> ChannelBuffers = {};
    
    FThreadSafeCounter ReadIndex;
    FThreadSafeCounter WriteIndex;
};

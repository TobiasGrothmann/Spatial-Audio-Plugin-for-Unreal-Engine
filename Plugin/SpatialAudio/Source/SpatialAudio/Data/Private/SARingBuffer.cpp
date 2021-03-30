#include "../Public/SARingBuffer.h"
#include "SpatialAudio/Public/SpatialAudioModule.h"


FSAringBuffer::FSAringBuffer(int32 NumChannels, int32 BufferSize)
    : NumChannels(NumChannels)
    , BufferSize(BufferSize)
{
    for (int32 c = 0; c < NumChannels; c++)
    {
        Audio::AlignedFloatBuffer* NewBuffer = new Audio::AlignedFloatBuffer();
        NewBuffer->AddZeroed(BufferSize);
        ChannelBuffers.Add(NewBuffer);
    }

    ReadIndex.Reset();
    WriteIndex.Reset();
}

FSAringBuffer::~FSAringBuffer()
{
    for (int32 c = 0; c < ChannelBuffers.Num(); c++)
    {
        if (ChannelBuffers[c] != nullptr)
        {
            delete ChannelBuffers[c];
            ChannelBuffers[c] = nullptr;
        }
    }
    ChannelBuffers.Empty();
}

void FSAringBuffer::Write(const FSAPacket& InPacket)
{
    if (this->BufferSize == 0 || this->NumChannels == 0) return;
    const int32 InBufferSize = InPacket.GetBufferLength();
    
    for (int32 s = 0; s < InBufferSize; s++)
    {
        const float InterpolationFactor = (float)s / InBufferSize;
        for (int32 c = 0; c < this->NumChannels; c++)
        {
            (*this->ChannelBuffers[c])[this->WriteIndex.GetValue()] = 0.0f;
            
            for (const FSpatializedSource& Source : InPacket.Sources)
            {
                if (!Source.Gains.Contains(c)) continue;
                
                const float Gain = Source.Gains[c].Lerp(InterpolationFactor);
                (*this->ChannelBuffers[c])[this->WriteIndex.GetValue()] += (*Source.Buffer)[s] * Gain;
            }
        }
        if (!IncrementWriteIndex())
        {
            UE_LOG(LogSpatialAudio, Warning, TEXT("Failed to write to RingBuffer - Buffer is full"));
            return;
        }
    }
}


void FSAringBuffer::Read(float* Out, int32 ReadBufferFrames, int32 NumOutChannels)
{
    if (this->BufferSize == 0 || this->NumChannels == 0) return;
    check(NumOutChannels == this->NumChannels);
    
    // Output silence if there is not enough to read from the buffer. This will ensure that we buffer a little bit before playing.
    if (GetBufferSizeUsed() < ReadBufferFrames)
    {
        for (int32 s = 0; s < ReadBufferFrames * NumOutChannels; s++)
        {
            Out[s] = 0.0f;
        }
        return;
    }
    
    for (int32 s = 0; s < ReadBufferFrames; s++)
    {
        if (!IncrementReadIndex())
        {
            UE_LOG(LogSpatialAudio, Warning, TEXT("Failed to read from RingBuffer - Buffer is empty."));
            break;
        }
        
        for (int32 c = 0; c < NumOutChannels; c++)
        {
            const int OutIndex = (s * NumOutChannels) + c;
            Out[OutIndex] = (*this->ChannelBuffers[c])[ReadIndex.GetValue()];
        }
    }
}

inline bool FSAringBuffer::IncrementWriteIndex()
{
    if ((WriteIndex.GetValue() + 1) % BufferSize == ReadIndex.GetValue())
    {
        return false;
    }
    
    WriteIndex.Increment();
    if (WriteIndex.GetValue() >= BufferSize)
    {
        WriteIndex.Reset();
    }
    return true;
}

inline bool FSAringBuffer::IncrementReadIndex()
{
    if (ReadIndex.GetValue() == WriteIndex.GetValue()) // Ringbuffer is empty
    {
        return false;
    }
    
    ReadIndex.Increment();
    if (ReadIndex.GetValue() >= BufferSize)
    {
        ReadIndex.Reset();
    }
    return true;
}

int32 FSAringBuffer::GetBufferSizeUsed() const
{
    if (WriteIndex.GetValue() >= ReadIndex.GetValue())
    {
        // [     read....write    ]
        return WriteIndex.GetValue() - ReadIndex.GetValue();
    }
    else
    {
        // [.....write    read....]
        return WriteIndex.GetValue() + (BufferSize - 1 - ReadIndex.GetValue());
    }
}


int32 FSAringBuffer::GetBufferSize() const
{
    return this->BufferSize;
}
int32 FSAringBuffer::GetNumChannels() const
{
    return this->NumChannels;
}

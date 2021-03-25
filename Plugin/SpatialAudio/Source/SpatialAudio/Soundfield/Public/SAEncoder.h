#pragma once

#include "ISoundfieldFormat.h"



class FSAEncoder : public ISoundfieldEncoderStream
{
public:
    
    virtual void Encode(const FSoundfieldEncoderInputData& InputData, ISoundfieldAudioPacket& OutputData) override;
    virtual void EncodeAndMixIn(const FSoundfieldEncoderInputData& InputData, ISoundfieldAudioPacket& OutputData) override;
};


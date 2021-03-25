#pragma once

#include "ISoundfieldFormat.h"



class FSADecoder : public ISoundfieldDecoderStream
{
public:
    
    virtual void Decode(const FSoundfieldDecoderInputData& InputData, FSoundfieldDecoderOutputData& OutputData) override;
    virtual void DecodeAndMixIn(const FSoundfieldDecoderInputData& InputData, FSoundfieldDecoderOutputData& OutputData) override;
};


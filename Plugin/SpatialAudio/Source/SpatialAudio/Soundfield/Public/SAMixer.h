#pragma once

#include "ISoundfieldFormat.h"



class FSAMixer : public ISoundfieldMixerStream
{
public:
    
    virtual void MixTogether(const FSoundfieldMixerInputData& InputData, ISoundfieldAudioPacket& PacketToMixInto) override;
    
};


#pragma once

#include "ISoundfieldFormat.h"



class FSATranscoder : public ISoundfieldTranscodeStream
{
public:
    
    virtual void Transcode(const ISoundfieldAudioPacket& InputData, const ISoundfieldEncodingSettingsProxy& InputSettings, ISoundfieldAudioPacket& OutputData, const ISoundfieldEncodingSettingsProxy& OutputSettings) override;
    virtual void TranscodeAndMixIn(const ISoundfieldAudioPacket& InputData, const ISoundfieldEncodingSettingsProxy& InputSettings, ISoundfieldAudioPacket& PacketToMixTo, const ISoundfieldEncodingSettingsProxy& OutputSettings) override;
    
    // TODO: implement transcode from FOA, HOA
};


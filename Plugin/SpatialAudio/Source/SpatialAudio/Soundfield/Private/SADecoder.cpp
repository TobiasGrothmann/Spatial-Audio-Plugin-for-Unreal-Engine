#include "../Public/SADecoder.h"
#include "AudioMixerDevice.h"
#include "AudioMixerTypes.h"
#include "SpatialAudio/InfoEncodingSpatializer/Public/InfoEncodingSpatializer.h"
#include "SpatialAudio/Data/Public/SAPacket.h"


void FSADecoder::Decode(const FSoundfieldDecoderInputData& InputData, FSoundfieldDecoderOutputData& OutputData)
{
    DecodeAndMixIn(InputData, OutputData);
}




void FSADecoder::DecodeAndMixIn(const FSoundfieldDecoderInputData& InputData, FSoundfieldDecoderOutputData& OutputData)
{
    UE_LOG(LogSpatialAudio, Error, TEXT("FSADecoder::DecodeAndMixIn called but is not implemented"));
}



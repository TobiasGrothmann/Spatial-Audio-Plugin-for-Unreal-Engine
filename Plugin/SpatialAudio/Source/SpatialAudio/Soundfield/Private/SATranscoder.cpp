#include "../Public/SATranscoder.h"




void FSATranscoder::Transcode(
     const ISoundfieldAudioPacket& InputData,
     const ISoundfieldEncodingSettingsProxy& InputSettings,
     ISoundfieldAudioPacket& OutputData,
     const ISoundfieldEncodingSettingsProxy& OutputSettings)
{
    UE_LOG(LogSpatialAudio, Warning, TEXT("Transcode called but is not implemented."));
}

void FSATranscoder::TranscodeAndMixIn(
     const ISoundfieldAudioPacket& InputData,
     const ISoundfieldEncodingSettingsProxy& InputSettings,
     ISoundfieldAudioPacket& PacketToMixTo,
     const ISoundfieldEncodingSettingsProxy& OutputSettings)
{
    UE_LOG(LogSpatialAudio, Warning, TEXT("TranscodeAndMixIn called but is not implemented."));
}

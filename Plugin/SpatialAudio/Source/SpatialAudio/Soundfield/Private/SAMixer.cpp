#include "../Public/SAMixer.h"




void FSAMixer::MixTogether(const FSoundfieldMixerInputData& InputData, ISoundfieldAudioPacket& PacketToMixInto)
{
    const FSAPacket& InPacket = DowncastSoundfieldRef<const FSAPacket>(InputData.InputPacket);
    FSAPacket& OutPacketSummed = DowncastSoundfieldRef<FSAPacket>(PacketToMixInto);
    
    for (const FSpatializedSource& Source : InPacket.Sources)
    {
        OutPacketSummed.Sources.Add({ Source.Buffer, Source.Gains });
    }
}

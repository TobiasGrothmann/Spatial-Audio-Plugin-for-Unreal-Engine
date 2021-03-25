#include "../Public/RtAudioDevice.h"


FRtAudioDevice::FRtAudioDevice(const RtAudio::DeviceInfo& deviceInfo)
{
    this->Probed = deviceInfo.probed;
    this->Name = FString(deviceInfo.name.c_str());
    this->NumOutputChannels = deviceInfo.outputChannels;
    this->NumInputChannels = deviceInfo.inputChannels;
    this->bIsDefaultOutput = deviceInfo.isDefaultOutput;
    this->bIsDefaultInput = deviceInfo.isDefaultInput;
    this->SampleRates = {};
    for (unsigned int sampleRate : deviceInfo.sampleRates)
    {
        this->SampleRates.Add(sampleRate);
    }
    this->PreferredSampleRate = deviceInfo.preferredSampleRate;
}

FString FRtAudioDevice::GetDeviceInfoString() const
{
    FString DefaultInString = FString(bIsDefaultInput ? TEXT("DEFAULT_IN ") : TEXT(""));
    FString DefaultOutString = FString(bIsDefaultOutput ? TEXT("DEFAULT_OUT ") : TEXT(""));
    return FString::Printf(TEXT("'%s' %s%s(in: %i, out: %i, %i)"),
                           *this->Name,
                           *DefaultInString,
                           *DefaultOutString,
                           this->NumInputChannels,
                           this->NumOutputChannels,
                           this->PreferredSampleRate
                           );
}

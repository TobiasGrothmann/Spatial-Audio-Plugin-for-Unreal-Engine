#include "../Public/SpatialAudioManager.h"
#include "Misc/ScopeTryLock.h"
#include "../Public/DebugSpeaker.h"
#include "../Public/RtAudioDevice.h"
#include "SpatialAudio/Util/Public/Util.h"
#include "SpatialAudio/Data/Public/SAPacket.h"



ASpatialAudioManager* ASpatialAudioManager::Instance = nullptr;


ASpatialAudioManager::ASpatialAudioManager()
    : AActor()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.SetTickFunctionEnable(true);
    PrimaryActorTick.bStartWithTickEnabled = true;
    SetActorTickEnabled(true);
}

void ASpatialAudioManager::OnConstruction(const FTransform& Transform)
{
    ASpatialAudioManager::Instance = this;
    
    Super::OnConstruction(Transform);
    CleanupDAC();
}




void ASpatialAudioManager::BeginPlay()
{
    ASpatialAudioManager::Instance = this;
    Super::BeginPlay();
    
    LastVerboseTick = 0.0;
    
    CleanupVbap();
    CleanupDAC();
    
    if (!GetLevel()->IsPersistentLevel())
    {
        UE_LOG(LogSpatialAudio, Error, TEXT("Setup failed: SpatialAudioManager must be in persistent level!"));
        return;
    }
    
    // SETUP Vbap
    FString ErrorMsg = {};
    if (!SetupVbap(ErrorMsg))
    {
        UE_LOG(LogSpatialAudio, Warning, TEXT("Errors during VBAP setup calculation with %i speakers: %s"), SpeakerPositions.Num(), *ErrorMsg);
    }

    
    // SETUP RtAudio
    std::vector<RtAudio::Api> Apis;
    RtAudio::getCompiledApi(Apis);
    for (RtAudio::Api Api : Apis)
    {
        UE_LOG(LogSpatialAudio, Log, TEXT("Rtaudio API found: %s"), *ASpatialAudioManager::ApiToString(Api));
    }

    ErrorMsg = {};
    if (!InitStream(ErrorMsg))
    {
        UE_LOG(LogSpatialAudio, Error, TEXT("Error initializing rtAudio stream: %s"), *ErrorMsg);
        CleanupDAC();
        return;
    }

    ErrorMsg = {};
    if (!StartStream(ErrorMsg))
    {
        UE_LOG(LogSpatialAudio, Error, TEXT("Error starting rtAudio stream: %s"), *ErrorMsg);
        CleanupDAC();
        return;
    }
    
    FScopeLock DebugAmpLock(&DebugSpeakerAmplitudesCriticalSection);
    DebugSpeakerAmplitudes.SetNumZeroed(GetNumChannels(), true);
}

void ASpatialAudioManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    LastVerboseTick += DeltaTime;
    if (LastVerboseTick >= VerboseTickInterval)
    {
        VerboseTick();
        LastVerboseTick = 0.0f;
    }
    
    if (bShowDebugSpeakerAmp)
    {
        FScopeTryLock DebugAmpLock(&DebugSpeakerAmplitudesCriticalSection);
        
        if (DebugAmpLock.IsLocked())
        {
            for (int c = 0; c < GetNumChannels(); c++)
            {
                if (!DebugSpeakers.IsValidIndex(c) ||
                    !IsValid(DebugSpeakers[c]) ||
                    !DebugSpeakerAmplitudes.IsValidIndex(c))
                {
                    continue;
                }
                DebugSpeakers[c]->SetAmp(DebugSpeakerAmplitudes[c]);
            }
        }
    }
    
    if (bFollowListenerLocation || bFollowListenerRotation)
    {
        UpdateDebugSpeakerLocation();
    }
}

void ASpatialAudioManager::VerboseTick()
{
    if (bLogRingBufferReadWrite && FSpatialAudioModule::RingBuffer)
    {
        UE_LOG(LogSpatialAudio, Log, TEXT("Buffer Used: %i / %i"),
               FSpatialAudioModule::RingBuffer->GetBufferSizeUsed(),
               FSpatialAudioModule::RingBuffer->GetBufferSize()
        );
    }
}

void ASpatialAudioManager::EndPlay(EEndPlayReason::Type Reason)
{
    Super::EndPlay(Reason);
    
    FString ErrorMsg = {};
    if (!EndStream(ErrorMsg))
    {
        UE_LOG(LogSpatialAudio, Error, TEXT("Error stopping rtAudio stream: %s"), *ErrorMsg);
    }
    
    if (bFollowListenerRotation)
    {
        SpeakerSetupRotation = FRotator::ZeroRotator;
    }
    if (bFollowListenerLocation)
    {
        SpeakerSetupLocation = FVector::ZeroVector;
    }
    
    CleanupDAC();
    CleanupVbap();
}

void ASpatialAudioManager::BeginDestroy()
{
    Super::BeginDestroy();
    
    TArray<ASpatialAudioManager*> SAManagers = Util::GetObjectsOfClass<ASpatialAudioManager>();
    for (ASpatialAudioManager* SAManager : SAManagers)
    {
        if (!IsValid(SAManager)) continue;
        if (SAManager == this) continue;
        if (SAManager->GetWorld()->StreamingLevelsPrefix.Len() != 0) continue; // non empty in PIE and standalone preview
        
        ASpatialAudioManager::Instance = SAManager;
        break;
    }
    
    for (ADebugSpeaker* DebugSpeaker : DebugSpeakers)
    {
        if (!IsValid(DebugSpeaker)) continue;
        DebugSpeaker->Destroy();
    }
}

bool ASpatialAudioManager::InitStream(FString& ErrorMsg)
{
    std::vector<RtAudio::Api> Apis;
    RtAudio::getCompiledApi(Apis);
    
    if (Apis.size() <= 0)
    {
        ErrorMsg = "Failed to find a RtAudio api.";
        return false;
    }
    
    if (DAC != nullptr) return true;
    if (DAC == nullptr)
    {
        try
        {
            DAC = new RtAudio(Apis[0]);
        }
        catch (RtAudioError& e)
        {
            DAC = nullptr;
            ErrorMsg = FString(e.what());
            return false;
        }
    }
    return DAC != nullptr;
}

bool ASpatialAudioManager::StartStream(FString& ErrorMsg)
{
    if (DAC->getDeviceCount() == 0)
    {
        ErrorMsg = TEXT("Error: No audio devices available.");
        return false;
    };
    
    for (const FString& Name : GetOutputDeviceNames())
    {
        UE_LOG(LogSpatialAudio, Log, TEXT("%s"), *Name);
    }
    
    int UsedOutDevice = DAC->getDefaultOutputDevice();
    bool bFoundSelectedAudioDevice = false;
    for (int32 device = 0; device < (int32)DAC->getDeviceCount(); device++)
    {
        RtAudio::DeviceInfo deviceInfo = DAC->getDeviceInfo(device);
        FRtAudioDevice Device = FRtAudioDevice(deviceInfo);
        UE_LOG(LogSpatialAudio, Log, TEXT("found AudioDevice %i: %s"), device, *Device.GetDeviceInfoString());
        if (Device.Name == AudioOutputDeviceName)
        {
            UsedOutDevice = device;
            bFoundSelectedAudioDevice = true;
        }
    }
    
    if (AudioOutputDeviceName != "" && !bFoundSelectedAudioDevice)
    {
        ErrorMsg = FString::Printf(TEXT("Failed to find selected audio device: '%s'"), *AudioOutputDeviceName);
        return false;
    }
    
    int32 UESampleRate = FAudioDeviceManager::Get()->GetActiveAudioDevice().GetAudioDevice()->SampleRate;
    SampleRate = UESampleRate; // use same SampleRate that is used in UE
    UE_LOG(LogSpatialAudio, Log, TEXT("UE SampleRate: %i"), UESampleRate);
    
    OutputParams.deviceId = UsedOutDevice;
    OutputParams.nChannels = GetNumChannels();
    
    const FString OutDeviceName = FString(DAC->getDeviceInfo(UsedOutDevice).name.c_str());
    UE_LOG(LogSpatialAudio, Log, TEXT("Using Audio Device: '%s'"), *OutDeviceName);
    StreamOptions.numberOfBuffers = 5;
    
    const int32 OutDeviceOutputChannels = DAC->getDeviceInfo(UsedOutDevice).outputChannels;
    if (GetNumChannels() > (int32)DAC->getDeviceInfo(UsedOutDevice).outputChannels)
    {
        ErrorMsg = FString::Printf(TEXT("Invalid number of speakers in speaker setup: %i. Output device '%s' supports a maximum of %i speakers."), GetNumChannels(), *OutDeviceName, OutDeviceOutputChannels);
        return false;
    }
    else if (GetNumChannels() <= 0)
    {
        ErrorMsg = "Failed to start stream with 0 speakers/channels";
        return false;
    }
    
    try
    {
        DAC->openStream(&OutputParams, /* output params */
                        nullptr, /* input params */
                        RTAUDIO_FLOAT32, /* format */
                        SampleRate,
                        &BufferFrames,
                        &OnAudioCallback,
                        this, /* user data */
                        &StreamOptions,
                        &OnAudioErrorCallback /* error callback */
                        );
    }
    catch (RtAudioError& e)
    {
        ErrorMsg = FString(e.what());
        return false;
    }
    
    UE_LOG(LogSpatialAudio, Log, TEXT("RtAudio elected to use BufferSize of: %i"), BufferFrames);
    
    try
    {
        DAC->startStream();
    }
    catch (RtAudioError& e)
    {
        ErrorMsg = FString(e.what());
        return false;
    }
    
    return true;
}

bool ASpatialAudioManager::EndStream(FString& ErrorMsg)
{
    if (!DAC) return true;
    
    if (!DAC->isStreamOpen())
    {
        ErrorMsg = TEXT("DAC stream is not open.");
        return false;
    }
    
    DAC->abortStream();
    DAC->closeStream();
    
    return true;
}

void ASpatialAudioManager::CleanupDAC()
{
    if (FSpatialAudioModule::RingBuffer)
    {
        delete FSpatialAudioModule::RingBuffer;
        FSpatialAudioModule::RingBuffer = nullptr;
    }
    if (DAC)
    {
        delete DAC;
        DAC = nullptr;
    }
}


bool ASpatialAudioManager::SetupVbap(FString& ErrorMsg)
{
    FSpatialAudioModule::Vbap = new FVbap();
    const size_t NumSpeakers = GetNumChannels();
    if (NumSpeakers < 3)
    {
        ErrorMsg = "Can't compute VBAP speaker configuration with less than 3 speakers.";
        return false;
    }
    
    float* Angles = new float[NumSpeakers * 2]();
    for (int i = 0; i < NumSpeakers; i++)
    {
        Angles[i * 2] = SpeakerPositions[i].Azimuth;
        Angles[i * 2 + 1] = SpeakerPositions[i].Elevation; // in degrees
    }
    
    char Error = FSpatialAudioModule::Vbap->Prepare(NumSpeakers, Angles);
    if (Error != 0)
    {
        ErrorMsg = FString::Printf(TEXT("Error Nr %i"), Error);
    }
    delete[] Angles;
    return Error == 0;
}

void ASpatialAudioManager::CleanupVbap()
{
    if (FSpatialAudioModule::Vbap != nullptr)
    {
        delete FSpatialAudioModule::Vbap;
        FSpatialAudioModule::Vbap = nullptr;
    }
}




int32 ASpatialAudioManager::OnAudioCallback(void *OutBuffer, void* InBuffer, uint32 BufferFrames, double StreamTime, RtAudioStreamStatus AudioStreamStatus, void* InUserData)
{
    if (AudioStreamStatus == RTAUDIO_INPUT_OVERFLOW)
    {
        UE_LOG(LogSpatialAudio, Error, TEXT("RtAudio: RTAUDIO_INPUT_OVERFLOW"));
    }
    if (AudioStreamStatus == RTAUDIO_OUTPUT_UNDERFLOW)
    {
        UE_LOG(LogSpatialAudio, Error, TEXT("RtAudio: RTAUDIO_OUTPUT_UNDERFLOW"));
    }
    ASpatialAudioManager* AudioManager = (ASpatialAudioManager*)InUserData;
    if (!IsValid(AudioManager))
    {
        UE_LOG(LogSpatialAudio, Error, TEXT("RtAudio: AudioManager invalid"));
        return 0;
    }
    if (!FSpatialAudioModule::RingBuffer)
    {
        UE_LOG(LogSpatialAudio, Log, TEXT("Initializing RingBuffer"));
        
        FSpatialAudioModule::RingBuffer = new FSAringBuffer(AudioManager->GetNumChannels(), 4096);
        return 0;
    }
    
// *** OUTPUT AUDIO ***
    float* Out = (float*) OutBuffer;
    const int NumOutChannels = AudioManager->OutputParams.nChannels;
    
    FSpatialAudioModule::RingBuffer->Read(Out, BufferFrames, NumOutChannels);
    
    return 0;
}

void ASpatialAudioManager::OnAudioErrorCallback(RtAudioError::Type Type, const std::string& ErrorText)
{
    UE_LOG(LogSpatialAudio, Error, TEXT("Error: %s"), *FString(ErrorText.c_str()));
}

void ASpatialAudioManager::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
    Super::PostEditChangeChainProperty(PropertyChangedEvent);
    
    UpdateDebugSpeakerNumber();
    UpdateDebugSpeakerLocation();
    
    for (int i = 0; i < SpeakerPositions.Num(); i++)
    {
        for (int j = i + 1; j < SpeakerPositions.Num(); j++)
        {
            if (FVector::Distance(SpeakerPositions[i].GetAsVector(), SpeakerPositions[j].GetAsVector()) < 0.01)
            {
                UE_LOG(LogSpatialAudio, Error, TEXT("Speaker-Positions at index %i and %i are the same / very close"), i, j);
            }
        }
    }
}

void ASpatialAudioManager::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);
    
    const FName& PropertyName = PropertyChangedEvent.Property->GetFName();

    if (PropertyName == GET_MEMBER_NAME_CHECKED(ASpatialAudioManager, bShowDebugSpeakers))
    {
        if (!bShowDebugSpeakers)
        {
            bShowDebugSpeakerAmp = false;
        }
        UpdateDebugSpeakerNumber();
    }
}

bool ASpatialAudioManager::CanEditChange(const FProperty* InProperty) const
{
    bool bIsEditable = Super::CanEditChange(InProperty);
    
    if (bIsEditable && InProperty)
    {
        const FName& PropertyName = InProperty->GetFName();

        if (PropertyName == GET_MEMBER_NAME_CHECKED(ASpatialAudioManager, SpeakerSetupLocation))
        {
            return !bFollowListenerLocation;
        }
        else if (PropertyName == GET_MEMBER_NAME_CHECKED(ASpatialAudioManager, SpeakerSetupRotation))
        {
            return !bFollowListenerRotation;
        }
        else if (PropertyName == GET_MEMBER_NAME_CHECKED(ASpatialAudioManager, bShowDebugSpeakerAmp))
        {
            return bShowDebugSpeakers;
        }
    }

    return bIsEditable;
}

void ASpatialAudioManager::UpdateDebugSpeakerNumber()
{
    FScopeLock DebugAmpLock(&DebugSpeakerCriticalSection);
    
    int32 NewNumDebugSpeakers = bShowDebugSpeakers ? SpeakerPositions.Num() : 0;
    
    // create debug speaker
    for (int i = 0; i < NewNumDebugSpeakers; i++)
    {
        if (!DebugSpeakers.IsValidIndex(i))
        {
#if WITH_EDITOR
            DebugSpeakers.Add(GEditor->GetEditorWorldContext().World()->SpawnActor<ADebugSpeaker>());
#else
            DebugSpeakers.Add(GetWorld()->SpawnActor<ADebugSpeaker>());
#endif
        }
        else if (!IsValid(DebugSpeakers[i])) // spawn new and overwrite
        {
#if WITH_EDITOR
            DebugSpeakers[i] = GEditor->GetEditorWorldContext().World()->SpawnActor<ADebugSpeaker>();
#else
            DebugSpeakers[i] = GetWorld()->SpawnActor<ADebugSpeaker>();
#endif
        }
    }
    
    int32 Destroyed = 0;
    // delete leftover debug previews if speaker list got smaller
    for (int i = DebugSpeakers.Num() - 1; i >= NewNumDebugSpeakers; i--)
    {
        if (IsValid(DebugSpeakers[i]))
        {
            DebugSpeakers[i]->Destroy();
            DebugSpeakers[i] = nullptr;
            Destroyed++;
        }
    }
    
    DebugSpeakers.SetNum(NewNumDebugSpeakers, true);
    
#if WITH_EDITOR
    for (int i = 0; i < DebugSpeakers.Num(); i++)
    {
        if (!IsValid(DebugSpeakers[i]))
        {
            UE_LOG(LogSpatialAudio, Warning, TEXT("Invalid DebugSpeaker at index: %i"), i);
        };
        
        DebugSpeakers[i]->SetActorLabel(FString::Printf(TEXT("DebugSpeaker_%s"), *Util::ZeroPad(i, 3)));
        DebugSpeakers[i]->SetFolderPath("SA_DebugSpeakers");
    }
#endif
    
    DebugSpeakerAmplitudes.SetNumZeroed(GetNumChannels(), true);
}

void ASpatialAudioManager::UpdateDebugSpeakerLocation()
{
    int32 NewNumDebugSpeakers = bShowDebugSpeakers ? SpeakerPositions.Num() : 0;
    
    for (int i = 0; i < NewNumDebugSpeakers; i++)
    {
        if (!DebugSpeakers.IsValidIndex(i))
        {
            UE_LOG(LogSpatialAudio, Error, TEXT("Invalid index for debug speaker list: %i"), i);
            continue;
        }
        
        ADebugSpeaker* DebugSpeaker = DebugSpeakers[i];
        if (!IsValid(DebugSpeaker))
        {
            UE_LOG(LogSpatialAudio, Warning, TEXT("Invalid debug speaker at index: %i"), i);
        }
        
        const FVector& NewLocation = SpeakerSetupLocation + SpeakerPositions[i].GetAsVector(SpeakerSetupRotation) * 100.0f * 5.0f;
        
        DebugSpeaker->SetActorLocation(NewLocation);
        DebugSpeaker->SetActorRotation((SpeakerSetupLocation - NewLocation).Rotation());
    }
}

int32 ASpatialAudioManager::GetNumChannels() const
{
    return this->SpeakerPositions.Num();
}


void ASpatialAudioManager::SetSpeakerPositions(const TArray<FSpeakerPosition>& NewPositions)
{
    SpeakerPositions.Empty();
    SpeakerPositions = NewPositions;
    UpdateDebugSpeakerNumber();
    UpdateDebugSpeakerLocation();
}


FString ASpatialAudioManager::ApiToString(RtAudio::Api api)
{
    switch (api) {
        case RtAudio::Api::UNSPECIFIED:    /*!< Search for a working compiled API. */
            return TEXT("UNSPECIFIED");
            break;
        case RtAudio::Api::LINUX_ALSA:     /*!< The Advanced Linux Sound Architecture API. */
            return TEXT("LINUX_ALSA");
            break;
        case RtAudio::Api::LINUX_PULSE:    /*!< The Linux PulseAudio API. */
            return TEXT("LINUX_PULSE");
            break;
        case RtAudio::Api::LINUX_OSS:      /*!< The Linux Open Sound System API. */
            return TEXT("LINUX_OSS");
            break;
        case RtAudio::Api::UNIX_JACK:      /*!< The Jack Low-Latency Audio Server API. */
            return TEXT("UNIX_JACK");
            break;
        case RtAudio::Api::MACOSX_CORE:    /*!< Macintosh OS-X Core Audio API. */
            return TEXT("MACOSX_CORE");
            break;
        case RtAudio::Api::WINDOWS_WASAPI: /*!< The Microsoft WASAPI API. */
            return TEXT("WINDOWS_WASAPI");
            break;
        case RtAudio::Api::WINDOWS_ASIO:   /*!< The Steinberg Audio Stream I/O API. */
            return TEXT("WINDOWS_ASIO");
            break;
        case RtAudio::Api::WINDOWS_DS:     /*!< The Microsoft Direct Sound API. */
            return TEXT("WINDOWS_DS");
            break;
        case RtAudio::Api::RTAUDIO_DUMMY:   /*!< A compilable but non-functional API. */
            return TEXT("RTAUDIO_DUMMY");
            break;
        default:
            return TEXT("UNKNOWN");
            break;
    }
}

TArray<FString> ASpatialAudioManager::GetOutputDeviceNames()
{
    TArray<FString> OutDeviceNames = {};
    RtAudio DAC = RtAudio(RtAudio::Api::MACOSX_CORE);
    
    for (int32 device = 0; device < (int32)DAC.getDeviceCount(); device++)
    {
        if (DAC.getDeviceInfo(device).outputChannels <= 0) continue;
        RtAudio::DeviceInfo deviceInfo = DAC.getDeviceInfo(device);
        FRtAudioDevice Device = FRtAudioDevice(deviceInfo);
        OutDeviceNames.Add(Device.Name);
    }
    
    return OutDeviceNames;
}

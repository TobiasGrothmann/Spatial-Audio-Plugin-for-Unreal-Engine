#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RtAudio.h"

#include "SpatialAudio/Data/Public/SpeakerPosition.h"
#include "SpatialAudio/Vbap/Public/Vbap.h"

#include "SpatialAudioManager.generated.h"

struct FSpeakerPosition;
class FSAPacket;
class ADebugSpeaker;



UCLASS()
class SPATIALAUDIO_API ASpatialAudioManager : public AActor
{
    GENERATED_BODY()
public:
    
    ASpatialAudioManager();
private:
    static TSoftObjectPtr<ASpatialAudioManager> Instance;
public:
    static ASpatialAudioManager* GetInstance();
    
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void OnConstruction(const FTransform& Transform) override;
    virtual void EndPlay(EEndPlayReason::Type Reason) override;
    virtual void BeginDestroy() override;
    
    float VerboseTickInterval = 0.75f;
    float LastVerboseTick = 0.0f;
    void VerboseTick();
    
    int32 GetNumChannels() const;
    
    void SetSpeakerPositions(const TArray<FSpeakerPosition>& NewPositions);
    
	UPROPERTY(EditAnywhere, Category = "Audio")
	float MasterGain = 0.5f;

    UPROPERTY(VisibleAnywhere, Category = "Audio")
    FString AudioOutputDeviceName = "";
    
    UPROPERTY(EditAnywhere, Category = "SpeakerTransformMode")
    bool bFollowListenerLocation = true;
    
    UPROPERTY(EditAnywhere, Category = "SpeakerTransformMode")
    bool bFollowListenerRotation = true;
    
    UPROPERTY(EditAnywhere, Category = "SpeakerTransformMode")
    FVector SpeakerSetupLocation = FVector::ZeroVector;
    
    UPROPERTY(EditAnywhere, Category = "SpeakerTransformMode")
    FRotator SpeakerSetupRotation = FRotator::ZeroRotator;
    
    // DEBUGGING
    UPROPERTY(EditAnywhere, Category = "Debugging")
    bool bLogRingBufferReadWrite = false;
    
    UPROPERTY(EditAnywhere, Category = "Debugging")
    bool bShowDebugSpeakers = false;
    
    UPROPERTY(EditAnywhere, Category = "Debugging")
    bool bShowDebugSpeakerAmp = false;
    
private:
    
    bool InitStream(FString& ErrorMsg);
    bool StartStream(FString& ErrorMsg);
    bool SetupVbap(FString& ErrorMsg);
    
    bool EndStream(FString& ErrorMsg);
    void CleanupDAC();
    void CleanupVbap();
    
    virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual bool CanEditChange(const FProperty* InProperty) const override;
    
private:
    
// *** RTAUDIO ***
    RtAudio* DAC;
    
// RTAUDI CALLBACKS
    static int32 OnAudioCallback(void *OutBuffer, void* InBuffer, uint32 BufferFrames, double StreamTime, RtAudioStreamStatus AudioStreamStatus, void* InUserData);
    
    static void OnAudioErrorCallback(RtAudioError::Type Type, const std::string& ErrorText);
    
// RTAUDI SETTINGS
    RtAudio::StreamParameters OutputParams;
    unsigned int SampleRate; // will be chosen based on the used SampleRate in UE
    unsigned int BufferFrames = 1024; // suggestion for RtAudio, RtAudio might change this value
    RtAudio::StreamOptions StreamOptions;
    
    
    
// *** UTIL ***
    static FString ApiToString(RtAudio::Api api);
    
    

// *** SPEAKER POSITONS ***
    UPROPERTY(EditAnywhere, Category = "Settings")
    TArray<FSpeakerPosition> SpeakerPositions = {};
    
    
    
// *** DEBUG SPEAKER POSITIONS ***
    UPROPERTY()
    TArray<ADebugSpeaker*> DebugSpeakers = {};
    
public:
    UPROPERTY()
    TArray<float> DebugSpeakerAmplitudes = {};
    
    FCriticalSection DebugSpeakerAmplitudesCriticalSection = {};
    FCriticalSection DebugSpeakerCriticalSection = {};
private:
    
    void UpdateDebugSpeakerNumber();
    void UpdateDebugSpeakerLocation();
    
public:
    static TArray<FString> GetOutputDeviceNames();
};

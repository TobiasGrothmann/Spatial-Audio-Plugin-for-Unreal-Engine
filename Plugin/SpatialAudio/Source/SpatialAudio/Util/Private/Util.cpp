#include "../Public/Util.h"


FString Util::ZeroPad(int32 Number, int32 NumDigits)
{
    FString Output = FString::FromInt(Number);
    while (Output.Len() < NumDigits)
    {
        Output = FString(TEXT("0")) + Output;
    }
    return Output;
}

template<class T>
TArray<T*> Util::GetObjectsOfClass()
{
    TArray<T*> Objects = {};
    for (TObjectIterator<T> It; It; ++It)
    {
        T* Object = Cast<T>(*It);
        if (!IsValid(Object)) continue;
        
        Objects.Add(Object);
    }
    return Objects;
}


void Util::CreateSuccessNotification(const FString& Action, const FString& Message)
{
    TSharedPtr<SNotificationItem> Notification = FSlateNotificationManager::Get().AddNotification(GetNofificationInfo(Action, Message));
    Notification->SetCompletionState(SNotificationItem::CS_Success);
    UE_LOG(LogSpatialAudio, Log, TEXT("Success: %s - %s"), *Action, *Message);
    Notification->ExpireAndFadeout();
}

void Util::CreateFailNotification(const FString& Action, const FString& Message)
{
    TSharedPtr<SNotificationItem> Notification = FSlateNotificationManager::Get().AddNotification(GetNofificationInfo(Action, Message));
    Notification->SetCompletionState(SNotificationItem::CS_Fail);
    UE_LOG(LogSpatialAudio, Error, TEXT("%s - %s"), *Action, *Message);
    Notification->ExpireAndFadeout();
}

FNotificationInfo Util::GetNofificationInfo(const FString& Action, const FString& Message)
{
    FNotificationInfo Info(FText::FromString(FString::Printf(TEXT("%s - %s"), *Action, *Message)));
    Info.FadeInDuration = 0.05f;
    Info.FadeOutDuration = 1.5f;
    Info.ExpireDuration = 8.0f;
    Info.bUseThrobber = false;
    Info.bFireAndForget = true;
    Info.bAllowThrottleWhenFrameRateIsLow = false;
    Info.bUseSuccessFailIcons = true;
    return Info;
}

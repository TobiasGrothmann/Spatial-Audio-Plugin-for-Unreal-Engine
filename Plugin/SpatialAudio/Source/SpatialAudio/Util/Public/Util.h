#pragma once

#include "CoreMinimal.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"


class SPATIALAUDIO_API Util
{
public:
    static FString ZeroPad(int32 Number, int32 NumDigits);
    
    template<class T>
    static TArray<T*> GetObjectsOfClass();
    
    // Notifications
    static void CreateSuccessNotification(const FString& Action, const FString& Message);
    static void CreateFailNotification(const FString& Action, const FString& Message);
    
private:
    static FNotificationInfo GetNofificationInfo(const FString& Action, const FString& Message);
};

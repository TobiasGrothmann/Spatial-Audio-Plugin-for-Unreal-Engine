#pragma once

#include "CoreMinimal.h"


class SPATIALAUDIO_API Util
{
public:
    static FString ZeroPad(int32 Number, int32 NumDigits);
    
    template<class T>
    static TArray<T*> GetObjectsOfClass();
};

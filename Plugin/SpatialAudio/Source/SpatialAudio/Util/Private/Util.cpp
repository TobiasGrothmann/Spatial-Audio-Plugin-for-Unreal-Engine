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

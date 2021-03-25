#pragma once

#include "CoreMinimal.h"
#include "XmlFile.h"


class SPATIALAUDIOEDITOR_API EditorUtil
{
public:
    
    template<class T>
    static TArray<T*> GetObjectsOfClass();
    
    static bool IsInfoEncodingSpatializerTheCurrentSpatPlugin();
    static void SetInfoEncodingSpatializerTheCurrentSpatPlugin(bool Value);
    
    static bool IsExactlyOneSAManagerInWorld();
    static void SetExactlyOneSAManagerInWorld(bool Value);
    
};

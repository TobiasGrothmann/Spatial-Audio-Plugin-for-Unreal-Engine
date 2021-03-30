#include "../Public/EditorUtil.h"

#include "AudioPluginUtilities.h"
#include "Engine/World.h"

#include "SpatialAudio/SpatialAudioManager/Public/SpatialAudioManager.h"
#include "SpatialAudio/InfoEncodingSpatializer/Public/InfoEncodingSpatializerPluginFactory.h"


template<class T>
TArray<T*> EditorUtil::GetObjectsOfClass()
{
    TArray<T*> Objects = {};
    for (TObjectIterator<T> It; It; ++It)
    {
        T* Object = Cast<T>(*It);
        if (!IsValid(Object)) continue;
        
        if (Object->StaticClass()->GetDefaultObject() == Object) continue; // skip CDO
        
        Objects.Add(Object);
    }
    return Objects;
}

bool EditorUtil::IsInfoEncodingSpatializerTheCurrentSpatPlugin()
{
    return AudioPluginUtilities::GetDesiredPluginName(EAudioPlugin::SPATIALIZATION).Equals(FInfoEncodingSpatializerPluginFactory::GetSpatializerPluginName());
}

void EditorUtil::SetInfoEncodingSpatializerTheCurrentSpatPlugin(bool Value)
{
//  TODO: cannot be done as it seems
}


bool EditorUtil::IsExactlyOneSAManagerInWorld()
{
    TArray<ASpatialAudioManager*> SAManagers = GetObjectsOfClass<ASpatialAudioManager>();
    if (SAManagers.Num() != 1) return false;
    
    if (!SAManagers[0]->GetLevel()->IsPersistentLevel()) return false;
    return true;
}

void EditorUtil::SetExactlyOneSAManagerInWorld(bool Value)
{
    TArray<ASpatialAudioManager*> SAManagers = GetObjectsOfClass<ASpatialAudioManager>();
    
    if (Value)
    {
        if (SAManagers.Num() == 0)
        {
            UWorld* World = GEditor->GetEditorWorldContext().World();
            if (IsValid(World))
            {
                ASpatialAudioManager* SAManager = World->SpawnActor<ASpatialAudioManager>();
                SAManager->SetFolderPath("#_SpatialAudioManager");
            }
        }
        else if (SAManagers.Num() > 1)
        {
            for (int i = 1; i < SAManagers.Num(); i++)
            {
                SAManagers[i]->ConditionalBeginDestroy();
            }
        }
    }
    else
    {
        for (int i = 0; i < SAManagers.Num(); i++)
        {
            SAManagers[i]->Destroy();
        }
    }
}

#include "../Public/EditorUtil.h"

#include "AudioPluginUtilities.h"
#include "Engine/World.h"

#include "SpatialAudio/Util/Public/Util.h"
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
                Util::CreateSuccessNotification(FString(TEXT("Create SpatialAudioManager")), FString(TEXT("succeeded")));
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

float EditorUtil::GainToDb(float Gain)
{
    return 20.0f * FMath::LogX(10, Gain);
}

float EditorUtil::DbToGain(float Db)
{
    return FMath::Pow(10.0f, Db / 20.0f);
}

FString EditorUtil::GetFloatAsStringWithPrecision(float Value, int32 Precision)
{
    float Rounded = FMath::RoundToFloat(Value);
	if (FMath::Abs(Value - Rounded) < FMath::Pow(10, -1 * Precision))
	{
		Value = Rounded;
	}
    FNumberFormattingOptions NumberFormat;
    NumberFormat.MinimumIntegralDigits = 1;
	NumberFormat.MaximumIntegralDigits = 1000;
	NumberFormat.MinimumFractionalDigits = Precision;
	NumberFormat.MaximumFractionalDigits = Precision;
    NumberFormat.UseGrouping = false;
	return FText::AsNumber(Value, &NumberFormat).ToString().Replace(TEXT(","), TEXT("."));
}
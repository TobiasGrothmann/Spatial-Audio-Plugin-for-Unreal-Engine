#include "../Public/SpatialAudioSetupTab.h"

#include "AssetRegistryModule.h"
#include "UObject/SoftObjectPtr.h"
#include "UObject/SoftObjectPath.h"
#include "Sound/SoundWave.h"

#include "Widgets/Input/SButton.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Views/SListView.h"
#include "Styling/CoreStyle.h"

#include "SpatialAudio/SpatialAudioManager/Public/SpatialAudioManager.h"
#include "SpatialAudio/Data/Public/SpeakerPosition.h"
#include "SpatialAudio/InfoEncodingSpatializer/Public/InfoEncodingSpatializerPluginFactory.h"

#include "SpatialAudioEditor/Util/Public/SpeakerConfigUtil.h"
#include "../Public/AudioComponentInfo.h"
#include "../Public/AudioComponentRow.h"
#include "../Public/SoundAssetInfo.h"
#include "../Public/SoundAssetRow.h"

#define LOCTEXT_NAMESPACE "SpatialAudioEditor"



void SSpatialAudioSetupTab::Construct(const FArguments& InArgs)
{
    static TSoftClassPtr<USoundfieldEndpointSubmix> SAEndpoint = TSoftClassPtr<USoundfieldEndpointSubmix>(FSoftObjectPath(TEXT("SoundfieldEndpointSubmix'/SpatialAudio/Sound/SASoundfieldEndpoint.SASoundfieldEndpoint'")));
    SAEndpoint.LoadSynchronous();
    
    ChildSlot
    .VAlign(VAlign_Fill)
    .HAlign(HAlign_Fill)
    [
        SNew(SBox)
        .Padding(5)
        [
            SNew(SVerticalBox)
            // MENU
            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew(SHorizontalBox)
                + SHorizontalBox::Slot()
                .Padding(5)
                .AutoWidth()
                [
                    SNew(SButton)
                    .HAlign(HAlign_Center)
                    .OnClicked(this, &SSpatialAudioSetupTab::Refresh)
                    [
                        SNew(STextBlock)
                        .Text(LOCTEXT("refresh", "↻")) // refresh ↻
                        .Font(FCoreStyle::GetDefaultFontStyle("Regular", 16))
                    ]
                ]
                + SHorizontalBox::Slot()
                .Padding(70, 15, 5, 5)
                .AutoWidth()
                [
                    SNew(STextBlock)
                    .Text(FText::FromString(FString::Printf(TEXT("Spatializer is '%s' (project settings):"), *FInfoEncodingSpatializerPluginFactory::GetSpatializerPluginName())))
                ]
                + SHorizontalBox::Slot()
                .Padding(5)
                .AutoWidth()
                [
                    SAssignNew(SSpatPluginCheckBox, SCheckBox)
                    .IsChecked(EditorUtil::IsInfoEncodingSpatializerTheCurrentSpatPlugin())
                    .OnCheckStateChanged(this, &SSpatialAudioSetupTab::OnSpatPluginSetChanged)
                    .IsEnabled(false)
                ]
                + SHorizontalBox::Slot()
                .Padding(25, 15, 5, 5)
                .AutoWidth()
                [
                    SNew(STextBlock)
                    .Text(FText::FromString("There is exactly one SpatialAudioManager in the persistent level:"))
                ]
                + SHorizontalBox::Slot()
                .Padding(5)
                .AutoWidth()
                [
                    SAssignNew(SAManagerCheckbox, SCheckBox)
                    .IsChecked(EditorUtil::IsExactlyOneSAManagerInWorld())
                    .OnCheckStateChanged(this, &SSpatialAudioSetupTab::OnSAManagerSetChanged)
                ]
                + SHorizontalBox::Slot() [ SNew(SSpacer) ]
            ]
            // LIST VIEWS
            + SVerticalBox::Slot()
            [
                SNew(SHorizontalBox)
                // LEFT
                + SHorizontalBox::Slot()
                .Padding(5)
                [
                    SAssignNew(AudioComponentListView, SListView<TSharedPtr<FAudioComponentInfo>>)
                    .ListItemsSource(&AudioComponentInfos)
                    .OnGenerateRow(this, &SSpatialAudioSetupTab::GeneratedAudioComponentItem)
                    .OnSelectionChanged(this, &SSpatialAudioSetupTab::AudioComponentSelectionChanged)
                    .HeaderRow
                    (
                        SNew(SHeaderRow)
                        + SHeaderRow::Column("actor")
                        .FillWidth(0.35f)
                        [
                            SNew(SBox).Padding(5.0f) [ SNew(STextBlock).Text(FText::FromString("Actor")) ]
                        ]
                        + SHeaderRow::Column("component")
                        .FillWidth(0.35f)
                        [
                            SNew(SBox).Padding(5.0f) [ SNew(STextBlock).Text(FText::FromString("Component")) ]
                        ]
                        + SHeaderRow::Column("spatialized")
                        .FillWidth(0.15f)
                        [
                            SNew(SBox).Padding(5.0f) [ SNew(STextBlock).Text(FText::FromString("spatialized")) ]
                        ]
                    )
                ]
                // RIGHT
                + SHorizontalBox::Slot()
                .Padding(5)
                [
                    SAssignNew(SoundAssetListView, SListView<TSharedPtr<FSoundAssetInfo>>)
                    .ListItemsSource(&SoundAssetInfos)
                    .OnGenerateRow(this, &SSpatialAudioSetupTab::GeneratedSoundAssetItem)
                    .OnSelectionChanged(this, &SSpatialAudioSetupTab::SoundAssetSelectionChanged)
                    .HeaderRow
                    (
                        SNew(SHeaderRow)
                        + SHeaderRow::Column("name")
                        .FillWidth(0.35f)
                        [
                            SNew(SBox).Padding(5.0f) [ SNew(STextBlock).Text(FText::FromString("Asset name")) ]
                        ]
                     
                        + SHeaderRow::Column("package")
                        .FillWidth(0.5f)
                        [
                            SNew(SBox).Padding(5.0f) [ SNew(STextBlock).Text(FText::FromString("Package path")) ]
                        ]
                        
                        + SHeaderRow::Column("soundfield")
                        .FillWidth(0.15f)
                        [
                            SNew(SBox).Padding(5.0f) [ SNew(STextBlock).Text(FText::FromString("Endpoint set")) ]
                        ]
                    )
                ]
            ]
        ]
    ];
    
    FTimerDelegate TimerCallback;
    TimerCallback.BindLambda([this]
    {
        Refresh();
    });

    FTimerHandle Handle;
    GEditor->GetTimerManager()->SetTimer(Handle, TimerCallback, 0.5f, false);
}



void SSpatialAudioSetupTab::OnSpatPluginSetChanged(ECheckBoxState NewState)
{
    EditorUtil::SetInfoEncodingSpatializerTheCurrentSpatPlugin(NewState == ECheckBoxState::Checked);
}

void SSpatialAudioSetupTab::OnSAManagerSetChanged(ECheckBoxState NewState)
{
    EditorUtil::SetExactlyOneSAManagerInWorld(NewState == ECheckBoxState::Checked);
}



TSharedRef<ITableRow> SSpatialAudioSetupTab::GeneratedAudioComponentItem(TSharedPtr<FAudioComponentInfo> InItem, const TSharedRef<STableViewBase>& OwnerTable)
{
    return SNew(SAudioComponentRow, OwnerTable)
    .AudioComponentInfo(InItem)
    .ListView(AudioComponentListView)
    .Tab(this);
}

void SSpatialAudioSetupTab::AudioComponentSelectionChanged(TSharedPtr<FAudioComponentInfo> ItemChangingState, ESelectInfo::Type SelectInfo)
{
    if (bUpdatingViews) return;
    bUpdatingViews = true;
    if (!AudioComponentListView.IsValid()) return;
    
    TArray<TSharedPtr<FAudioComponentInfo>> SelectedAudioComponents = {};
    AudioComponentListView->GetSelectedItems(SelectedAudioComponents);
    
    // get all used sound assets from sound components selection
    TArray<TSharedPtr<FSoundAssetInfo>> UsedSoundAssets = {};
    
    for (const TSharedPtr<FAudioComponentInfo>& ComponentInfo : SelectedAudioComponents)
    {
        if (!ComponentInfo.IsValid() || !IsValid(ComponentInfo->AudioComponent)) continue;
        
        for (const TSharedPtr<FSoundAssetInfo>& AssetInfo : SoundAssetInfos)
        {
            if (!AssetInfo.IsValid()) continue;
            USoundBase* ComponentSoundBase = Cast<USoundBase>(ComponentInfo->AudioComponent->Sound);
            USoundBase* AssetSoundBase = Cast<USoundBase>(AssetInfo->Sound);
            
            if (   IsValid(ComponentSoundBase)
                && IsValid(AssetSoundBase)
                && ComponentSoundBase == AssetSoundBase)
            {
                UsedSoundAssets.Add(AssetInfo);
            }
        }
    }
    
    // change selection in SoundAssetListView
    SoundAssetListView->ClearSelection();
    SoundAssetListView->SetItemSelection(UsedSoundAssets, true, ESelectInfo::Type::Direct);
    
    // select actors in world outliner
    GEditor->Cleanse(true, false, FText::FromString("cleanup"));
    for (const TSharedPtr<FAudioComponentInfo>& ComponentInfo : SelectedAudioComponents)
    {
        if (!ComponentInfo.IsValid() || !ComponentInfo->IsValidInfo()) continue;
        
        GEditor->SelectActor(ComponentInfo->Actor, true, true, false, true);
    }
    
    bUpdatingViews = false;
}



TSharedRef<ITableRow> SSpatialAudioSetupTab::GeneratedSoundAssetItem(TSharedPtr<FSoundAssetInfo> InItem, const TSharedRef<STableViewBase>& OwnerTable)
{
    return SNew(SSoundAssetRow, OwnerTable)
    .SoundAssetInfo(InItem)
    .ListView(SoundAssetListView)
    .Tab(this);
}

void SSpatialAudioSetupTab::SoundAssetSelectionChanged(TSharedPtr<FSoundAssetInfo> ItemChangingState, ESelectInfo::Type SelectInfo)
{
    if (bUpdatingViews) return;
    bUpdatingViews = true;
    if (!SoundAssetListView.IsValid()) return;
    
    TArray<TSharedPtr<FSoundAssetInfo>> SelectedSoundAssets = {};
    SoundAssetListView->GetSelectedItems(SelectedSoundAssets);
    
    // get all used referenced AudioComponents from sound assets selection
    TArray<TSharedPtr<FAudioComponentInfo>> UsedComponents = {};
    
    for (const TSharedPtr<FSoundAssetInfo>& AssetInfo : SelectedSoundAssets)
    {
        if (!AssetInfo.IsValid()) continue;
        
        for (const TSharedPtr<FAudioComponentInfo>& ComponentInfo : AudioComponentInfos)
        {
            if (!ComponentInfo.IsValid() || !IsValid(ComponentInfo->AudioComponent)) continue;
            
            if (ComponentInfo->AudioComponent->Sound == AssetInfo->Sound)
            {
                UsedComponents.Add(ComponentInfo);
            }
        }
    }
    
    // change selection in SoundAssetListView
    AudioComponentListView->ClearSelection();
    AudioComponentListView->SetItemSelection(UsedComponents, true, ESelectInfo::Type::Direct);
    
    bUpdatingViews = false;
}











void SSpatialAudioSetupTab::UpdateAudioComponents()
{
    AudioComponentInfos.Empty();

    for (UAudioComponent* AudioComponent : EditorUtil::GetObjectsOfClass<UAudioComponent>())
    {
        if (!IsValid(AudioComponent)) continue;
        if (!IsValid(AudioComponent->GetOwner())) continue;
        
        // skip CDO
        if (AudioComponent->GetOwner() == AudioComponent->GetOwner()->StaticClass()->GetDefaultObject()) continue;
        if (AudioComponent == AudioComponent->StaticClass()->GetDefaultObject()) continue;
        
        if (!IsValid(AudioComponent->GetWorld())) continue;
        if (!AudioComponent->GetWorld()->IsEditorWorld()) continue;
        
        FAudioComponentInfo* ComponentInfo = new FAudioComponentInfo(AudioComponent);
        AudioComponentInfos.Add(MakeShareable(ComponentInfo));
    }
    
    AudioComponentInfos.Sort([](const TSharedPtr<FAudioComponentInfo>& A, const TSharedPtr<FAudioComponentInfo>& B)
    {
        return A->Actor->GetName().Compare(B->Actor->GetName()) > 0;
    });
    
    if (AudioComponentListView.IsValid()) AudioComponentListView->RequestListRefresh();
}

void SSpatialAudioSetupTab::UpdateSoundAssets()
{
    SoundAssetInfos.Empty();
    
    FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
    TArray<FAssetData> AssetData = {};
    FARFilter Filter;
    Filter.ClassNames.Add(USoundBase::StaticClass()->GetFName());
    Filter.bRecursiveClasses = true;
    Filter.PackagePaths.Add("/Game");
    Filter.bRecursivePaths = true;
    AssetRegistryModule.Get().GetAssets(Filter, AssetData);

    for (const FAssetData& Asset : AssetData)
    {
        FSoundAssetInfo* AssetInfo = new FSoundAssetInfo(Asset);
        SoundAssetInfos.Add(MakeShareable(AssetInfo));
    }
    
    SoundAssetInfos.Sort([](const TSharedPtr<FSoundAssetInfo>& A, const TSharedPtr<FSoundAssetInfo>& B)
    {
        return A->PackageName.Compare(B->PackageName) > 0;
    });

    if (SoundAssetListView.IsValid()) SoundAssetListView->RequestListRefresh();
}


FReply SSpatialAudioSetupTab::Refresh()
{
    UpdateAudioComponents();
    UpdateSoundAssets();
    
    SSpatPluginCheckBox->SetIsChecked(EditorUtil::IsInfoEncodingSpatializerTheCurrentSpatPlugin() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
    SAManagerCheckbox->SetIsChecked(EditorUtil::IsExactlyOneSAManagerInWorld() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
    
    return FReply::Handled();
}





TSharedPtr<FSoundAssetInfo> SSpatialAudioSetupTab::GetSoundAssetForAudioComponent(const TSharedPtr<FAudioComponentInfo>& ComponentInfo) const
{
    for (const TSharedPtr<FSoundAssetInfo>& AssetInfo : SoundAssetInfos)
    {
        if (!AssetInfo.IsValid()) continue;
        
    }
    return nullptr;
}


#undef LOCTEXT_NAMESPACE

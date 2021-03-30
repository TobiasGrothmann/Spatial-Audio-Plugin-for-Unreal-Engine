#pragma once

#include "CoreMinimal.h"

#include "Widgets/SCompoundWidget.h"

#include "SoundAssetInfo.h"
#include "AudioComponentInfo.h"


class SPATIALAUDIOEDITOR_API SSpatialAudioSetupTab : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SSpatialAudioSetupTab)
    {}
    SLATE_END_ARGS()
    
    void Construct(const FArguments& InArgs);
    
    
    // Menu
    void OnSpatPluginSetChanged(ECheckBoxState NewState);
    void OnSAManagerSetChanged(ECheckBoxState NewState);
    
    TSharedPtr<SCheckBox> SAManagerCheckbox = nullptr;
    TSharedPtr<SCheckBox> SSpatPluginCheckBox = nullptr;
    
    // Audio Components
    TSharedPtr<SListView<TSharedPtr<FAudioComponentInfo>>> AudioComponentListView = nullptr;
    TArray<TSharedPtr<FAudioComponentInfo>> AudioComponentInfos = {};
    
    TSharedRef<ITableRow> GeneratedAudioComponentItem(TSharedPtr<FAudioComponentInfo> InItem, const TSharedRef<STableViewBase>& OwnerTable);
    void AudioComponentSelectionChanged(TSharedPtr<FAudioComponentInfo> ItemChangingState, ESelectInfo::Type SelectInfo);
    
    // Sound Assets
    TSharedPtr<SListView<TSharedPtr<FSoundAssetInfo>>> SoundAssetListView = nullptr;
    TArray<TSharedPtr<FSoundAssetInfo>> SoundAssetInfos = {};
    
    TSharedRef<ITableRow> GeneratedSoundAssetItem(TSharedPtr<FSoundAssetInfo> InItem, const TSharedRef<STableViewBase>& OwnerTable);
    void SoundAssetSelectionChanged(TSharedPtr<FSoundAssetInfo> ItemChangingState, ESelectInfo::Type SelectInfo);
    
    
    // refresh
    FReply Refresh();
    
    void UpdateAudioComponents();
    void UpdateSoundAssets();
    
    bool bUpdatingViews = false; // used to avoid infinite loop because selection changes in one list view change the other
    
    
    TSharedPtr<FSoundAssetInfo> GetSoundAssetForAudioComponent(const TSharedPtr<FAudioComponentInfo>& ComponentInfo) const;
    
    
    // output device
    FText GetAudioOutputDeviceText() const;
    TSharedRef<SWidget> OnGenerateAudioDeviceRow(TSharedPtr<FString> InItem);
    void OnAudioDeviceSelectionChanged(TSharedPtr<FString> InItem, ESelectInfo::Type InSeletionInfo);

    TArray<TSharedPtr<FString>> GetOutputDeviceOptions();
    TArray<TSharedPtr<FString>> OutputDeviceOptions = {};
    const FString DefaultAudioDeviceString = FString(TEXT("-- default --"));
};

#pragma once

#include "CoreMinimal.h"

#include "Widgets/Views/STableRow.h"
#include "Styling/SlateTypes.h"
#include "Templates/SharedPointer.h"

#include "SoundAssetInfo.h"



class SPATIALAUDIOEDITOR_API SSoundAssetRow : public SMultiColumnTableRow<TSharedPtr<FSoundAssetInfo>>
{
public:
    SLATE_BEGIN_ARGS(SSoundAssetRow) {}
        SLATE_ARGUMENT(TSharedPtr<FSoundAssetInfo>, SoundAssetInfo)
        SLATE_ARGUMENT(TSharedPtr<SListView<TSharedPtr<FSoundAssetInfo>>>, ListView)
        SLATE_ARGUMENT(SSpatialAudioSetupTab*, Tab)
    SLATE_END_ARGS()
    
    TSharedPtr<FSoundAssetInfo> SoundAssetInfo = nullptr;
    TSharedPtr<SListView<TSharedPtr<FSoundAssetInfo>>> ListView = nullptr;
    SSpatialAudioSetupTab* Tab = nullptr;
    
    void Construct(const FArguments& Args, const TSharedRef<STableViewBase>& OwnerTableView)
    {
        SoundAssetInfo = Args._SoundAssetInfo;
        ListView = Args._ListView;
        Tab = Args._Tab;
        
        SMultiColumnTableRow<TSharedPtr<FSoundAssetInfo>>::Construct(
            FSuperRowType::FArguments()
            .Padding(1.0f),
            OwnerTableView
        );
    }

    virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override
    {
        if (ColumnName == "name")
        {
            return SNew(STextBlock)
            .Text(FText::FromString(SoundAssetInfo->Name));
        }
        
        if (ColumnName == "package")
        {
            return SNew(STextBlock)
            .Text(FText::FromString(SoundAssetInfo->PackageName));
        }
        
        if (ColumnName == "soundfield")
        {
            return SNew(SBox)
            .HAlign(HAlign_Center)
            [
                SNew(SCheckBox)
                .IsChecked(SoundAssetInfo->GetUsesCorrectSoundfieldEndpoint())
                .OnCheckStateChanged(this, &SSoundAssetRow::OnEndpointStateChanged)
            ];
        }
        
        return SNullWidget::NullWidget;
    }
    
    void OnEndpointStateChanged(ECheckBoxState NewState)
    {
        if (!SoundAssetInfo.IsValid()) return;
        SoundAssetInfo->SetUsesCorrectSoundfieldEndpoint(NewState == ECheckBoxState::Checked);
        
        if (!ListView.IsValid()) return;
        TArray<TSharedPtr<FSoundAssetInfo>> SelectedSoundAssets = {};
        ListView->GetSelectedItems(SelectedSoundAssets);
        
        for (TSharedPtr<FSoundAssetInfo>& Info : SelectedSoundAssets)
        {
            if (!Info.IsValid()) return;
            Info->SetUsesCorrectSoundfieldEndpoint(NewState == ECheckBoxState::Checked);
        }
        
        if (Tab)
        {
            Tab->UpdateSoundAssets();
        }
    }
};

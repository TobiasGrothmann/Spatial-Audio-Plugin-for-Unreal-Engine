#pragma once

#include "CoreMinimal.h"

#include "Widgets/Views/STableRow.h"
#include "Styling/SlateTypes.h"

#include "AudioComponentInfo.h"



class SPATIALAUDIOEDITOR_API SAudioComponentRow : public SMultiColumnTableRow<TSharedPtr<FAudioComponentInfo>>
{
public:
    SLATE_BEGIN_ARGS(SAudioComponentRow) {}
        SLATE_ARGUMENT(TSharedPtr<FAudioComponentInfo>, AudioComponentInfo)
        SLATE_ARGUMENT(TSharedPtr<SListView<TSharedPtr<FAudioComponentInfo>>>, ListView)
        SLATE_ARGUMENT(SSpatialAudioSetupTab*, Tab)
    SLATE_END_ARGS()
    
    TSharedPtr<FAudioComponentInfo> AudioComponentInfo = nullptr;
    TSharedPtr<SListView<TSharedPtr<FAudioComponentInfo>>> ListView = nullptr;
    SSpatialAudioSetupTab* Tab = nullptr;

    
    void Construct(const FArguments& Args, const TSharedRef<STableViewBase>& OwnerTableView)
    {
        AudioComponentInfo = Args._AudioComponentInfo;
        ListView = Args._ListView;
        Tab = Args._Tab;
        
        SMultiColumnTableRow<TSharedPtr<FAudioComponentInfo>>::Construct(
            FSuperRowType::FArguments()
            .Padding(1.0f),
            OwnerTableView
        );
    }

    virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override
    {
        if (!AudioComponentInfo.IsValid()) return SNullWidget::NullWidget;
        if (!AudioComponentInfo->IsValidInfo()) return SNullWidget::NullWidget;
        
        
        if (ColumnName == "actor")
        {
            return SNew(STextBlock)
            .Text(AudioComponentInfo->GetActorName());
        }
        
        if (ColumnName == "component")
        {
            return SNew(STextBlock)
            .Text(AudioComponentInfo->GetComponentName());
        }
        
        if (ColumnName == "spatialized")
        {
            return SNew(SBox)
            .HAlign(HAlign_Center)
            [
                SNew(SCheckBox)
                .IsChecked(AudioComponentInfo->GetUsesSpat())
                .OnCheckStateChanged(this, &SAudioComponentRow::OnSpatStateChanged)
            ];
        }
        
        return SNullWidget::NullWidget;
    }
    
    void OnSpatStateChanged(ECheckBoxState NewState)
    {
        if (!AudioComponentInfo.IsValid()) return;
        AudioComponentInfo->SetUsesSpat(NewState == ECheckBoxState::Checked);
        
        if (!ListView.IsValid()) return;
        TArray<TSharedPtr<FAudioComponentInfo>> SelectedAudioComponents = {};
        ListView->GetSelectedItems(SelectedAudioComponents);
        
        for (TSharedPtr<FAudioComponentInfo>& Info : SelectedAudioComponents)
        {
            if (!Info.IsValid()) return;
            Info->SetUsesSpat(NewState == ECheckBoxState::Checked);
        }
        
        if (Tab)
        {
            Tab->UpdateAudioComponents();
        }
    }
};

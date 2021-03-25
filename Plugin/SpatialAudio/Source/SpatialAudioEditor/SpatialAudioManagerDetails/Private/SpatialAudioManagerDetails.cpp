#include "../Public/SpatialAudioManagerDetails.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "DesktopPlatformModule.h"
#include "EditorDirectories.h"

#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SComboBox.h"

#include "SpatialAudio/SpatialAudioManager/Public/SpatialAudioManager.h"
#include "SpatialAudio/Data/Public/SpeakerPosition.h"

#include "SpatialAudioEditor/Util/Public/SpeakerConfigUtil.h"

#define LOCTEXT_NAMESPACE "SpatialAudioEditor"


TSharedRef<IDetailCustomization> FSpatialAudioManagerDetails::MakeInstance()
{
    return MakeShareable(new FSpatialAudioManagerDetails());
}

void FSpatialAudioManagerDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
    IDetailCategoryBuilder& DeviceCategory = DetailBuilder.EditCategory(TEXT("Audio Output Device"));
    
    IDetailCategoryBuilder& LoadCategory = DetailBuilder.EditCategory(TEXT("Load speaker-setups"));
    
    FText IosonoLoad = LOCTEXT("IosonoLoadButton", "load Iosono .xml");
    LoadCategory.AddCustomRow(IosonoLoad)
    .WholeRowContent()
    [
        SNew(SButton)
        .Text(IosonoLoad)
        .OnClicked_Lambda([this] () {
            LoadIosono();
            return FReply::Handled();
        })
    ];
    
    FText CsvLoad = LOCTEXT("CsvLodButton", "load .csv");
    LoadCategory.AddCustomRow(CsvLoad)
    .WholeRowContent()
    [
        SNew(SButton)
        .Text(CsvLoad)
        .OnClicked_Lambda([this] () {
            LoadCsv();
            return FReply::Handled();
        })
    ];
}

void FSpatialAudioManagerDetails::LoadIosono()
{
    FString FilePath = {};
    bool bOpened = OpenFileDialog(TEXT("iosono.xml|*.xml"), FilePath);
    if (!bOpened) return;
    
    TArray<FSpeakerPosition> ParsedPositions = {};
    bool bSuccess = SpeakerConfigUtil::FromXmlFile(FilePath, ParsedPositions);
    if (!bSuccess)
    {
        UE_LOG(LogSpatialAudioEditor, Error, TEXT("Failed to parse XML"));
        return;
    }
    
    SetSpeakerPositions(ParsedPositions);
}

void FSpatialAudioManagerDetails::LoadCsv()
{
    FString FilePath = {};
    bool bOpened = OpenFileDialog(TEXT("csv|*.csv"), FilePath);
    if (!bOpened) return;
    
    TArray<FSpeakerPosition> ParsedPositions = {};
    bool bSuccess = SpeakerConfigUtil::FromCsvFile(FilePath, ParsedPositions);
    if (!bSuccess)
    {
        UE_LOG(LogSpatialAudioEditor, Error, TEXT("Failed to parse CSV"));
        return;
    }
    
    SetSpeakerPositions(ParsedPositions);
}

void FSpatialAudioManagerDetails::SetSpeakerPositions(const TArray<FSpeakerPosition>& ParsedPositions)
{
    ASpatialAudioManager* SAM = ASpatialAudioManager::Instance;
    if (!IsValid(SAM))
    {
        UE_LOG(LogSpatialAudioEditor, Error, TEXT("Spatial Audio Manager is invalid. This should not have happened."));
        return;
    }
    
    UE_LOG(LogSpatialAudioEditor, Log, TEXT("Applying %i speaker positions"), ParsedPositions.Num())
    SAM->SetSpeakerPositions(ParsedPositions);
}

bool FSpatialAudioManagerDetails::OpenFileDialog(const FString& FileTypes, FString& OutPath)
{
//  FileTypes should be a "|" delimited list of (Description|Extensionlist) pairs. Extensionlists are ";" delimited.
    IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
    const void* ParentWindowWindowHandle = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);
    TArray<FString> OpenFilenames = {};
    int32 FilterIndex = 0;
    static const FString SessionInfoFilename = TEXT("Speaker.xml");
    bool bOpened = DesktopPlatform->OpenFileDialog(
        ParentWindowWindowHandle,
        LOCTEXT("ImportDialogTitle", "Load speaker-setup from Iosono xml file.").ToString(),
        FEditorDirectories::Get().GetLastDirectory(ELastDirectory::GENERIC_IMPORT), // Default path.
        SessionInfoFilename, // Default file
        FileTypes, // Filter
        EFileDialogFlags::None,
        OpenFilenames,
        FilterIndex
    );
    if (bOpened && OpenFilenames.IsValidIndex(0))
    {
        OutPath = OpenFilenames[0];
    }
    return bOpened;
}


#undef LOCTEXT_NAMESPACE
